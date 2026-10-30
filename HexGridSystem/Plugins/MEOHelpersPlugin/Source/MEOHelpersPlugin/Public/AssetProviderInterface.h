// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/StreamableManager.h"
#include "AssetProviderInterface.generated.h"

struct FSoftObjectPath;

/**
 *	interface for objects that provide soft references to assets that need to be preloaded
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAssetProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *	interface for objects that provide soft references to assets that need to be preloaded
 */
class MEOHELPERSPLUGIN_API IAssetProviderInterface
{
	GENERATED_BODY()

public:

	// fill an array with all soft paths that need to be preloaded for this object
	virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const = 0;

	// optional callback function that is executed after all collected assets are loaded
	virtual void HandleAssetsPreloaded() {}

	// collect an asset from a soft pointer if it is not null
	// returns true if the asset was collected
	template <typename TSoftPtrType>
	static bool CollectPreloadAsset(TSoftPtrType const& SoftPtr, TArray<FSoftObjectPath>& OutAssets)
	{
		if (SoftPtr.IsNull())
		{
			return false;
		}

		OutAssets.Add(SoftPtr.ToSoftObjectPath());


		return true;
	}

	// synchronously load all assets under the given asset provider
	static void SyncLoadAssetProviderRecursively(
		const IAssetProviderInterface* AssetProvider,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	);

	// synchronously load the given asset and all assets discovered underneath if it implements the asset provider interface
	static void SyncLoadAssetRecursively(
		FSoftObjectPath const& Asset,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	);

	// synchronously load the given asset and all assets discovered underneath if it implements the asset provider interface
	template <typename T>
	static void SyncLoadAssetRecursively(
		TSoftObjectPtr<T> const& Asset,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	)
	{
		SyncLoadAssetRecursively(
			Asset.ToSoftObjectPath(),
			OutObjects,
			MaxRecursionDepth
		);
	}

	// synchronously load the given asset and all assets discovered underneath if it implements the asset provider interface
	template <typename T>
	static void SyncLoadAssetRecursively(
		TSoftClassPtr<T> const& Asset,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	)
	{
		SyncLoadAssetRecursively(
			Asset.ToSoftObjectPath(),
			OutObjects,
			MaxRecursionDepth
		);
	}

	// synchronously load the given assets and all assets discovered underneath if any implement the asset provider interface
	static void SyncLoadAssetsRecursively(
		TArray<FSoftObjectPath> const& Assets,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	);

	// synchronously load the given assets and all assets discovered underneath if any implement the asset provider interface
	template <typename T>
	static void SyncLoadAssetsRecursively(
		TArray<TSoftObjectPtr<T>> const& Assets,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	)
	{
		TArray<FSoftObjectPath> Paths;
		for (int i = 0; i < Assets.Num(); i++)
		{
			Paths.Add(Assets[i].ToSoftObjectPath());
		}
		SyncLoadAssetsRecursively(
			Paths,
			OutObjects,
			MaxRecursionDepth
		);
	}

	// synchronously load the given assets and all assets discovered underneath if any implement the asset provider interface
	template <typename T>
	static void SyncLoadAssetsRecursively(
		TArray<TSoftClassPtr<T>> const& Assets,
		TArray<UObject*>& OutObjects,
		const int MaxRecursionDepth = -1
	)
	{
		TArray<FSoftObjectPath> Paths;
		for (int i = 0; i < Assets.Num(); i++)
		{
			Paths.Add(Assets[i].ToSoftObjectPath());
		}
		SyncLoadAssetsRecursively(
			Paths,
			OutObjects,
			MaxRecursionDepth
		);
	}

private:

	// internal recursive helper function that tracks state
	static void SyncLoadAssetProviderRecursively_Internal(
		const IAssetProviderInterface* AssetProvider,
		TArray<UObject*>& OutLoadedObjects,
		TSet<const IAssetProviderInterface*>& VisitedProviders,
		const int CurrentDepth,
		const int MaxRecursionDepth
	);
};

