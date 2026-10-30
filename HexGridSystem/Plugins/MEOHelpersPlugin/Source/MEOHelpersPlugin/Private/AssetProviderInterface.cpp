// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "AssetProviderInterface.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"


void IAssetProviderInterface::SyncLoadAssetProviderRecursively(
    const IAssetProviderInterface* AssetProvider,
    TArray<UObject*>& OutLoadedObjects,
    const int MaxRecursionDepth)
{
    // A "visited" set is crucial to prevent getting stuck in an infinite loop
    // if two data assets reference each other.
    TSet<const IAssetProviderInterface*> VisitedProviders;
    SyncLoadAssetProviderRecursively_Internal(AssetProvider, OutLoadedObjects, VisitedProviders, 0, MaxRecursionDepth);
}


void IAssetProviderInterface::SyncLoadAssetProviderRecursively_Internal(
    const IAssetProviderInterface* AssetProvider,
    TArray<UObject*>& OutLoadedObjects,
    TSet<const IAssetProviderInterface*>& VisitedProviders,
    const int CurrentDepth,
    const int MaxRecursionDepth)
{
    // Base Cases for Recursion
    if (AssetProvider == nullptr ||
        VisitedProviders.Contains(AssetProvider))
    {
        return; // Stop if provider is null or we've already processed it.
    }
    // Stop if we've reached the depth limit (-1 means infinite).
    if (MaxRecursionDepth >= 0 && 
        CurrentDepth > MaxRecursionDepth)
    {
        return;
    }

    VisitedProviders.Add(AssetProvider);

    // Get the list of child assets from the current provider.
    TArray<FSoftObjectPath> ChildAssetPaths;
    AssetProvider->CollectAssetsToPreload(ChildAssetPaths);

    for (const FSoftObjectPath& AssetPath : ChildAssetPaths)
    {
        // Load the asset synchronously.
        UObject* LoadedObject = AssetPath.TryLoad();

        if (UMCommon::IsUObjectPointerSafe(LoadedObject))
        {
            // Add the object to our output list, ensuring no duplicates.
            OutLoadedObjects.AddUnique(LoadedObject);

            // if the object we loaded is a uclass, than we should inspect the CDO
            UObject* ObjectToInspect = nullptr;
            if (UClass* LoadedClass = Cast<UClass>(LoadedObject))
                ObjectToInspect = LoadedClass->GetDefaultObject();
            else
                ObjectToInspect = LoadedObject;

            // The Recursive Step
            // Check if this newly loaded object is ALSO an asset provider.
            IAssetProviderInterface* ChildProvider = Cast<IAssetProviderInterface>(ObjectToInspect);
            if (ChildProvider != nullptr)
            {
                // If it is, call this function again on the child, incrementing the depth.
                SyncLoadAssetProviderRecursively_Internal(
                    ChildProvider,
                    OutLoadedObjects,
                    VisitedProviders,
                    CurrentDepth + 1,
                    MaxRecursionDepth);
            }
        }
    }
}


void IAssetProviderInterface::SyncLoadAssetRecursively(
    FSoftObjectPath const& Asset,
    TArray<UObject*>& OutLoadedObjects,
    const int MaxRecursionDepth
)
{
    UObject* AssetObject = Asset.TryLoad();
    if (UMCommon::IsUObjectPointerSafe(AssetObject) == false)
    {
        return;
    }

    OutLoadedObjects.Add(AssetObject);

    if (MaxRecursionDepth != 0 && 
        AssetObject->Implements<UAssetProviderInterface>())
    {
        IAssetProviderInterface* AssetProvider = Cast<IAssetProviderInterface>(AssetObject);
        if (AssetProvider != nullptr)
        {
            SyncLoadAssetProviderRecursively(AssetProvider, OutLoadedObjects, MaxRecursionDepth > 0 ? MaxRecursionDepth - 1 : -1);
        }
    }


    return;
}


void IAssetProviderInterface::SyncLoadAssetsRecursively(
    TArray<FSoftObjectPath> const& Assets,
    TArray<UObject*>& OutObjects,
    const int MaxRecursionDepth
)
{
    FOR_FORWARD(i, Assets.Num())
    {
        SyncLoadAssetRecursively(
            Assets[i],
            OutObjects,
            MaxRecursionDepth
        );
    }


    return;
}