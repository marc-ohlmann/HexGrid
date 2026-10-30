// Copyright 2025 Marc Ohlmann, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MCommon.h"
#include "AssetProviderInterface.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Templates/SharedPointer.h"

DECLARE_DELEGATE_OneParam(FOnAllAssetsLoaded, TArray<TSharedPtr<FStreamableHandle>> const& /* LoadedAssetHandles */);

/*
 * a "fire-and-forget" utility that asynchronously loads a UDataAsset and all of its nested asset dependencies.
 * Automatically manages its own memory lifetime using TSharedFromThis.
 */
class FChainedAssetLoader : public TSharedFromThis<FChainedAssetLoader>
{
public:

    template<typename AssetType>
    static void LoadAssetsRecursively(TSoftObjectPtr<AssetType> const& DataAssetPtr, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        LoadAssetsRecursively(TArray<FSoftObjectPath>{ DataAssetPtr.ToSoftObjectPath() }, OnComplete, MaxRecursionDepth);
    }

    template<typename AssetType>
    static void LoadAssetsRecursively(TArray<TSoftObjectPtr<AssetType>> const& DataAssetPtrs, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        TArray<FSoftObjectPath> Paths;
        Paths.Reserve(DataAssetPtrs.Num());
        for (const auto& Ptr : DataAssetPtrs) { Paths.Add(Ptr.ToSoftObjectPath()); }
        LoadAssetsRecursively(Paths, OnComplete, MaxRecursionDepth);
    }

    template<typename AssetType>
    static void LoadAssetsRecursively(TSoftClassPtr<AssetType> const& DataAssetClassPtr, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        LoadAssetsRecursively(TArray<FSoftObjectPath>{ DataAssetClassPtr.ToSoftObjectPath() }, OnComplete, MaxRecursionDepth);
    }

    template<typename AssetType>
    static void LoadAssetsRecursively(TArray<TSoftClassPtr<AssetType>> const& DataAssetClassPtrs, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        TArray<FSoftObjectPath> Paths;
        Paths.Reserve(DataAssetClassPtrs.Num());
        for (const auto& Ptr : DataAssetClassPtrs) { Paths.Add(Ptr.ToSoftObjectPath()); }
        LoadAssetsRecursively(Paths, OnComplete, MaxRecursionDepth);
    }

    static void LoadAssetsRecursively(FSoftObjectPath const& DataAssetPath, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        LoadAssetsRecursively(TArray<FSoftObjectPath>{ DataAssetPath }, OnComplete, MaxRecursionDepth);
    }

    static void LoadAssetsRecursively(TArray<FSoftObjectPath> const& DataAssetPaths, FOnAllAssetsLoaded OnComplete, const int MaxRecursionDepth = -1)
    {
        TSharedRef<FChainedAssetLoader> Loader = MakeShared<FChainedAssetLoader>(DataAssetPaths, OnComplete, MaxRecursionDepth);
        Loader->Start();
    }

    static void ReleaseAssets(TArray<TSharedPtr<FStreamableHandle>>& Assets, const int Slack = 0)
    {
        for (int i = 0; i < Assets.Num(); i++)
        {
            if (Assets[i].IsValid())
            {
                Assets[i]->ReleaseHandle();
                Assets[i].Reset();
            }
        }
        Assets.Empty(Slack);
    }

    static void ReplaceAssets(TArray<TSharedPtr<FStreamableHandle>>& OldAssets, TArray<TSharedPtr<FStreamableHandle>> const& NewAssets)
    {
        ReleaseAssets(OldAssets, NewAssets.Num());
        OldAssets.Append(NewAssets);
    }

    // Public so MakeShared can access it, but hidden from regular API flow.
public:
    FChainedAssetLoader(TArray<FSoftObjectPath> const& InitialPaths, FOnAllAssetsLoaded OnComplete, const int InMaxRecursionDepth)
        : FinalCallback(OnComplete), MaxRecursionDepth(InMaxRecursionDepth)
    {
#if WITH_EDITORONLY_DATA
        StartTime = FDateTime::UtcNow();
#endif
        for (const FSoftObjectPath& Path : InitialPaths)
        {
            if (Path.IsNull() == false) DataAssetQueue.Add({ Path, 0 });
        }
    }

private:

    struct FQueuedDataAsset
    {
        FSoftObjectPath Path;
        int Depth = 0;
    };

    void Start()
    {
        if (DataAssetQueue.IsEmpty())
        {
            FinalCallback.ExecuteIfBound({});
            return;
        }
        ProcessDataAssetQueue();
    }

    void ProcessDataAssetQueue()
    {
        int32 ItemsToPop = 0;
        bool bFoundNewAsset = false;
        FQueuedDataAsset NextItem;

        while (ItemsToPop < DataAssetQueue.Num())
        {
            bool bAlreadyProcessed = false;
            ProcessedDataAssets.Add(DataAssetQueue[ItemsToPop].Path, &bAlreadyProcessed);

            if (bAlreadyProcessed)
            {
                ItemsToPop++;
            }
            else
            {
                NextItem = MoveTemp(DataAssetQueue[ItemsToPop]);
                ItemsToPop += 1;
                bFoundNewAsset = true;
                break;
            }
        }

        if (ItemsToPop > 0)
        {
            DataAssetQueue.RemoveAt(0, ItemsToPop, EAllowShrinking::No);
        }

        if (bFoundNewAsset)
        {
            TSharedPtr<FChainedAssetLoader> StrongThis = AsShared();
            ActiveHandles.Add(UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NextItem.Path, [StrongThis, NextItem]()
                {
                    StrongThis->OnDataAssetInspected(NextItem);
                }));
        }
        else
        {
            DataAssetQueue.Reset();
            LoadFinalAssetList();
        }
    }

    void OnDataAssetInspected(FQueuedDataAsset const& InspectedItem)
    {
        UObject* ObjectToInspect = nullptr;

        if (UClass* LoadedClass = Cast<UClass>(InspectedItem.Path.ResolveObject()))
            ObjectToInspect = LoadedClass->GetDefaultObject();
        else
            ObjectToInspect = InspectedItem.Path.ResolveObject();

        if (IAssetProviderInterface* AssetProvider = Cast<IAssetProviderInterface>(ObjectToInspect))
        {
            TArray<FSoftObjectPath> Dependencies;
            AssetProvider->CollectAssetsToPreload(Dependencies);

            if (MaxRecursionDepth < 0 || InspectedItem.Depth < MaxRecursionDepth)
            {
                for (const FSoftObjectPath& DepPath : Dependencies)
                {
                    FAssetData AssetData = UAssetManager::Get().GetAssetRegistry().GetAssetByObjectPath(DepPath);
                    bool bShouldInspect = false;

                    if (AssetData.IsValid())
                    {
                        UClass* AssetClass = AssetData.GetClass();
                        if (AssetClass->IsChildOf(UDataAsset::StaticClass()) ||
                            AssetClass->IsChildOf(UBlueprint::StaticClass()) ||
                            AssetClass->IsChildOf(UBlueprintGeneratedClass::StaticClass()))
                        {
                            bShouldInspect = true;
                        }
                    }
                    else if (DepPath.ToString().EndsWith(TEXT("_C")))
                    {
                        bShouldInspect = true;
                    }

                    if (bShouldInspect)
                        DataAssetQueue.Add({ DepPath, InspectedItem.Depth + 1 });
                    else
                        FinalLeafAssets.Add(DepPath);
                }
            }
        }

        ProcessDataAssetQueue();
    }

    void LoadFinalAssetList()
    {
        if (FinalLeafAssets.Num() > 0)
        {
            TSharedPtr<FChainedAssetLoader> StrongThis = AsShared();
            TSharedPtr<FStreamableHandle> FinalHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
                FinalLeafAssets.Array(), [StrongThis]()
                {
                    StrongThis->OnAllAssetsLoaded();
                });

            ActiveHandles.Add(FinalHandle);
        }
        else
        {
            OnAllAssetsLoaded();
        }
    }

    void OnAllAssetsLoaded()
    {
        FinalCallback.ExecuteIfBound(ActiveHandles);
#if WITH_EDITORONLY_DATA
        LogElapsedTime();
#endif
    }

    FOnAllAssetsLoaded FinalCallback;
    int MaxRecursionDepth;
    TArray<FQueuedDataAsset> DataAssetQueue;
    TSet<FSoftObjectPath> ProcessedDataAssets;
    TSet<FSoftObjectPath> FinalLeafAssets;
    TArray<TSharedPtr<FStreamableHandle>> ActiveHandles;

#if WITH_EDITORONLY_DATA
    FDateTime StartTime;
    void LogElapsedTime()
    {
        const float ElapsedTimeSeconds = (FDateTime::UtcNow() - StartTime).GetTotalSeconds();
        L_DBG(ChainedAssetLoader, "took {0} seconds loading {1} assets", ElapsedTimeSeconds, ProcessedDataAssets.Num());
    }
#endif
};