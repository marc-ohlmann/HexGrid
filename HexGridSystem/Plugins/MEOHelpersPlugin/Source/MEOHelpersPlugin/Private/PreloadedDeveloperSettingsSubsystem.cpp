// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "PreloadedDeveloperSettingsSubsystem.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "AssetProviderInterface.h"
#include "ChainedAssetLoader.h"

#include "Engine/DeveloperSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UObject/UObjectIterator.h"
#include "Engine/GameInstance.h"


UPreloadedDeveloperSettingsSubsystem::UPreloadedDeveloperSettingsSubsystem()
    : Super()
    , OnPreloadStarted()
    , OnPreloadFinished()
    , _bHasPreloadStarted(false)
    , _bHasPreloadFinished(false)
    , _PreloadStartTime()
    , _PreloadedAssetHandles()
    , _Participants()
{
}


void UPreloadedDeveloperSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    StartPreload();


    return;
}

void UPreloadedDeveloperSettingsSubsystem::Deinitialize()
{
    FOR_FORWARD(i, _PreloadedAssetHandles.Num())
    {
        if (_PreloadedAssetHandles[i].IsValid())
        {
            _PreloadedAssetHandles[i]->ReleaseHandle();
            _PreloadedAssetHandles[i].Reset();
        }
    }


    Super::Deinitialize();
    return;
}


UPreloadedDeveloperSettingsSubsystem* UPreloadedDeveloperSettingsSubsystem::Get(const UGameInstance* GameInstance)
{
    if (UMCommon::IsUObjectPointerSafe(GameInstance) == false)
    {
        L_ERR(MLoadingScreen, "game instance is null");
        return nullptr;
    }


    return GameInstance->GetSubsystem<UPreloadedDeveloperSettingsSubsystem>();
}


UPreloadedDeveloperSettingsSubsystem* UPreloadedDeveloperSettingsSubsystem::Get(const UWorld* GameWorld)
{
    if (UMCommon::IsUObjectPointerSafe(GameWorld) == false)
    {
        L_ERR(MLoadingScreen, "world is null");
        return nullptr;
    }


    return UPreloadedDeveloperSettingsSubsystem::Get(GameWorld->GetGameInstance());
}


UPreloadedDeveloperSettingsSubsystem* UPreloadedDeveloperSettingsSubsystem::Get(const UObject* GameContextObject)
{
    if (UMCommon::IsUObjectPointerSafe(GameContextObject) == false)
    {
        L_ERR(MLoadingScreen, "game context object is null");
        return nullptr;
    }


    return UPreloadedDeveloperSettingsSubsystem::Get(GameContextObject->GetWorld());
}


void UPreloadedDeveloperSettingsSubsystem::StartPreload()
{
    _Participants.Reset();
    _bHasPreloadFinished = false;
    _bHasPreloadStarted = true;
    _PreloadStartTime = FDateTime::UtcNow();

    TArray<FSoftObjectPath> TopLevelAssetsToLoad;

    for (TObjectIterator<UDeveloperSettings> It(RF_NoFlags); It; ++It)
    {
        UDeveloperSettings* DevSettings = *It;
        IF_NULL_CONTINUE(DevSettings);

        // only add cdo of non-abstract non-deprecated classes
        if (DevSettings->HasAnyFlags(RF_ClassDefaultObject) == false ||
            DevSettings->GetClass()->HasAnyClassFlags(CLASS_Deprecated | CLASS_Abstract))
        {
            continue;
        }

        IAssetProviderInterface* AssetProvider = Cast<IAssetProviderInterface>(*It);
        if (AssetProvider)
        {
            _Participants.Add(DevSettings);
            AssetProvider->CollectAssetsToPreload(TopLevelAssetsToLoad);
        }
    }

    if (TopLevelAssetsToLoad.Num() > 0)
    {
        // delegate the entire loading process to FChainedAssetLoader
        FChainedAssetLoader::LoadAssetsRecursively(
            TopLevelAssetsToLoad,
            FOnAllAssetsLoaded::CreateUObject(this, &UPreloadedDeveloperSettingsSubsystem::FinishPreload)
        );
    }
    else
    {
        FinishPreload(TArray<TSharedPtr<FStreamableHandle>>());
    }

    OnPreloadStarted.Broadcast();

    L_DBG(MEOHelpers, "discovered {0} top-level assets from {1} participants", TopLevelAssetsToLoad.Num(), _Participants.Num());


    return;
}


void UPreloadedDeveloperSettingsSubsystem::FinishPreload(TArray<TSharedPtr<FStreamableHandle>> const& LoadedAssetHandles)
{
    _bHasPreloadFinished = true;
    _PreloadedAssetHandles.Reserve(LoadedAssetHandles.Num());
    _PreloadedAssetHandles.Append(LoadedAssetHandles);

    for (const TWeakObjectPtr<UDeveloperSettings>& WeakParticipant : _Participants)
    {
        if (WeakParticipant.IsValid())
        {
            Cast<IAssetProviderInterface>(WeakParticipant.Get())->HandleAssetsPreloaded();
        }
    }

    OnPreloadFinished.Broadcast();

    L_DBG(MEOHelpers, "preload finished after {0} seconds", GetElapsedPreloadTimeSeconds());


    return;
}


float UPreloadedDeveloperSettingsSubsystem::GetElapsedPreloadTimeSeconds() const
{
    return (FDateTime::UtcNow() - _PreloadStartTime).GetTotalSeconds();
}


bool UPreloadedDeveloperSettingsSubsystem::HasPreloadStarted(const UObject* WorldContextObject)
{
    UPreloadedDeveloperSettingsSubsystem* PreloadDevSettingsSubsystem = UPreloadedDeveloperSettingsSubsystem::Get(WorldContextObject);
    IF_NULL_RETURN_VAL_LERR(PreloadDevSettingsSubsystem, false, MEOHelpers, "PreloadDevSettingsSubsystem is null");


    return PreloadDevSettingsSubsystem->HasPreloadStarted();
}


bool UPreloadedDeveloperSettingsSubsystem::IsPreloading(const UObject* WorldContextObject)
{
    UPreloadedDeveloperSettingsSubsystem* PreloadDevSettingsSubsystem = UPreloadedDeveloperSettingsSubsystem::Get(WorldContextObject);
    IF_NULL_RETURN_VAL_LERR(PreloadDevSettingsSubsystem, false, MEOHelpers, "PreloadDevSettingsSubsystem is null");


    return PreloadDevSettingsSubsystem->IsPreloading();
}


bool UPreloadedDeveloperSettingsSubsystem::HasPreloadFinished(const UObject* WorldContextObject)
{
    UPreloadedDeveloperSettingsSubsystem* PreloadDevSettingsSubsystem = UPreloadedDeveloperSettingsSubsystem::Get(WorldContextObject);
    IF_NULL_RETURN_VAL_LERR(PreloadDevSettingsSubsystem, false, MEOHelpers, "PreloadDevSettingsSubsystem is null");


    return PreloadDevSettingsSubsystem->HasPreloadFinished();
}