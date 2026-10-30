// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PreloadedDeveloperSettingsSubsystem.generated.h"


class UDeveloperSettings;
struct FStreamableHandle;

DECLARE_EVENT(UDevSettingsPreloaderSubsystem, FPreloadedDeveloperSubsystemEvent);


/**
 * Subsystem used to preload soft pointers in developer settings that implement the preloaded developer settings interface.
 */
UCLASS()
class MEOHELPERSPLUGIN_API UPreloadedDeveloperSettingsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UPreloadedDeveloperSettingsSubsystem();

    // static getter for the subsystem
    static UPreloadedDeveloperSettingsSubsystem* Get(const UGameInstance* GameInstance);
    static UPreloadedDeveloperSettingsSubsystem* Get(const UWorld* GameWorld);
    static UPreloadedDeveloperSettingsSubsystem* Get(const UObject* GameContextObject);

    // UGameInstanceSubsystem overrides
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // check if this subsystem has called StartPreload
    bool HasPreloadStarted() const { return _bHasPreloadStarted; }

    // check if the subsystem has called StartPreload
    static bool HasPreloadStarted(const UObject* WorldContextObject);

    // check if this subsystem has called FinishPreload
    bool HasPreloadFinished() const { return _bHasPreloadFinished; }

    // check if the subsystem has called FinishPreload
    static bool HasPreloadFinished(const UObject* WorldContextObject);

    // check if preload is in progress
    bool IsPreloading() const { return HasPreloadStarted() == true && HasPreloadFinished() == false; }

    // check if preload is in progress
    static bool IsPreloading(const UObject* WorldContextObject);

    // event broadcasts when preload started
    FPreloadedDeveloperSubsystemEvent OnPreloadStarted;

    // event broadcasts when preload finished
    FPreloadedDeveloperSubsystemEvent OnPreloadFinished;

private:

    // flag indicates if the preloader has finished
    UPROPERTY()
    bool _bHasPreloadStarted;

    // flag indicates if the preloader has finished
    UPROPERTY()
    bool _bHasPreloadFinished;

    // timestamp when preload started
    UPROPERTY()
    FDateTime _PreloadStartTime;

    // get time since preload started in seconds
    float GetElapsedPreloadTimeSeconds() const;

    // kick off preload of assets
    void StartPreload();

    // finish preload of assets
    void FinishPreload(TArray<TSharedPtr<FStreamableHandle>> const& LoadedAssetHandles);

    // handles will keep our assets loaded for the lifetime of this subsystem instance.
    TArray<TSharedPtr<FStreamableHandle>> _PreloadedAssetHandles;

    // list of participating developer settings objects
    UPROPERTY()
    TArray<TWeakObjectPtr<UDeveloperSettings>> _Participants;


};