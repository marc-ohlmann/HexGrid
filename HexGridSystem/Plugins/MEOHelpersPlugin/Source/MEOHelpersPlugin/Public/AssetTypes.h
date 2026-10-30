// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MCommon.h"
#include "Misc/Paths.h"
#include "AssetTypes.generated.h"


/**
 * Functions and constants for asset types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UAssetTypes : public UObject
{
	GENERATED_BODY()

public:

	// converts the name of a given class into an FPrimaryAssetType
	static FPrimaryAssetType GetPrimaryAssetTypeForClass(const UClass* AssetClass)
	{
		return FPrimaryAssetType(UMCommon::GetUObjectName(AssetClass));
	}

	// converts the name of a given asset class name and asset name into a fully formed FPrimaryAssetId
	static FPrimaryAssetId GetPrimaryAssetIdForAssetClassAndName(const UClass* AssetClass, FName const& AssetName)
	{
		return FPrimaryAssetId(
			GetPrimaryAssetTypeForClass(AssetClass),
			AssetName
		);
	}

	// creates a primary asset id using the given object's name and class name
	static FPrimaryAssetId GeneratePrimaryAssetIdForObject(const UObject* Object)
	{
		return GetPrimaryAssetIdForAssetClassAndName(
			UMCommon::GetUObjectClass(Object),
			UMCommon::GetUObjectName(Object)
		);
	}

	// check if the given asset id points to an existing asset
	static bool DoesAssetExist(FPrimaryAssetId const& AssetId);

	// check if the given asset class and asset name points to an existing asset
	static bool DoesAssetExist(const UClass* AssetClass, FName const& AssetName)
	{
		return DoesAssetExist(
			FPrimaryAssetId(
				GetPrimaryAssetTypeForClass(AssetClass),
				AssetName
			)
		);
	}

	// check if the given asset id points to an existing asset
	static bool DoesAssetExist(const UClass* AssetClass, FSoftObjectPath const& Asset)
	{
		return DoesAssetExist(
			AssetClass,
			Asset.GetAssetFName()
		);
	}

	// check if the given asset class and asset name points to an existing asset
	template<class TAssetClass>
	static bool DoesAssetExist(FName const& AssetName)
	{
		return DoesAssetExist(
			TAssetClass::StaticClass(),
			AssetName
		);
	}

	// check if the given asset class and asset name points to an existing asset
	template<class TAssetClass>
	static bool DoesAssetExist(TSoftObjectPtr<TAssetClass> const& Asset)
	{
		return DoesAssetExist(
			TAssetClass::StaticClass(),
			Asset.ToSoftObjectPath()
		);
	}

	// fetches asset data for the given asset id from the asset manager
	static bool GetAssetDataFromPrimaryAssetId(FPrimaryAssetId const& AssetId, FAssetData& OutAssetData);

	// fetches and loads an asset reference for the given asset id from the asset manager
	static UObject* GetAssetFromPrimaryAssetIdLoaded(FPrimaryAssetId const& AssetId);

	// fetches and loads an asset reference for the given asset class and name from the asset manager
	static UObject* GetAssetFromClassAndAssetNameLoaded(const UClass* AssetClass, FName const& AssetName)
	{
		return GetAssetFromPrimaryAssetIdLoaded(
			GetPrimaryAssetIdForAssetClassAndName(
				AssetClass,
				AssetName
			)
		);
	}

	// fetches and loads an asset reference for the given asset class and name from the asset manager
	template<class TAssetClass>
	static TAssetClass* GetAssetFromNameLoaded(FName const& AssetName)
	{
		return Cast<TAssetClass>(GetAssetFromClassAndAssetNameLoaded(TAssetClass::StaticClass(), AssetName));
	}

	// fetches an asset reference for the given asset id without loading it
	static FSoftObjectPath GetAssetFromPrimaryAssetIdSoftly(FPrimaryAssetId const& AssetId);

	// fetches an asset reference for the given asset id without loading it
	template <class TAssetClass>
	static TSoftObjectPtr<TAssetClass> GetAssetFromPrimaryAssetIdSoftly(FPrimaryAssetId const& AssetId)
	{
		return TSoftObjectPtr<TAssetClass>(GetAssetFromPrimaryAssetIdSoftly(AssetId));
	}

	// fetches an asset reference for the given asset id without loading it
	static FSoftObjectPath GetAssetFromClassAndAssetNameSoftly(const UClass* AssetClass, FName const& AssetName)
	{
		return GetAssetFromPrimaryAssetIdSoftly(GetPrimaryAssetIdForAssetClassAndName(AssetClass, AssetName));
	}

	// fetches an asset reference for the given asset id without loading it
	template <class TAssetClass>
	static TSoftObjectPtr<TAssetClass> GetAssetFromNameSoftly(FName const& AssetName)
	{
		return TSoftObjectPtr<TAssetClass>(GetAssetFromClassAndAssetNameSoftly(TAssetClass::StaticClass(), AssetName));
	}


	// get a list of all asset ids for a given asset type
	static void GetAllAssetIdsForType(FPrimaryAssetType const& AssetType, TArray<FPrimaryAssetId>& OutAssetIds);

	// get a list of all asset ids for a given asset class
	static void GetAllAssetIdsOfClass(const UClass* AssetClass, TArray<FPrimaryAssetId>& OutAssetIds)
	{
		return GetAllAssetIdsForType(GetPrimaryAssetTypeForClass(AssetClass), OutAssetIds);
	}

	// get a list of all asset ids for a given asset class
	template <class TAssetClass>
	static void GetAllAssetIds(TArray<FPrimaryAssetId>& OutAssetIds)
	{
		GetAllAssetIdsOfClass(TAssetClass::StaticClass(), OutAssetIds);
	}


	// get a list of all asset data for a given asset type
	static void GetAllAssetDataForType(FPrimaryAssetType const& AssetType, TArray<FAssetData>& OutAssetData);

	// get a list of all asset data for a given asset class
	static void GetAllAssetDataOfClass(const UClass* AssetClass, TArray<FAssetData>& OutAssetData)
	{
		return GetAllAssetDataForType(GetPrimaryAssetTypeForClass(AssetClass), OutAssetData);
	}

	// get a list of all asset data for a given asset class
	template <class TAssetClass>
	static void GetAllAssetData(TArray<FAssetData>& OutAssetData)
	{
		GetAllAssetDataOfClass(TAssetClass::StaticClass(), OutAssetData);
	}

	// get a list of soft asset references without loading anything
	static void GetAllAssetsForTypeSoftly(FPrimaryAssetType const& AssetType, TArray<FSoftObjectPath>& OutAssets);

	// get a list of soft asset references without loading anything
	static void GetAllAssetsOfClassSoftly(const UClass* AssetClass, TArray<FSoftObjectPath>& OutAssets)
	{
		return GetAllAssetsForTypeSoftly(GetPrimaryAssetTypeForClass(AssetClass), OutAssets);
	}

	// get a list of soft asset references without loading anything
	template <class TAssetClass>
	static void GetAllAssetsSoftly(TArray<TSoftObjectPtr<TAssetClass>>& OutAssets)
	{
		TArray<FSoftObjectPath> AllAssets;
		GetAllAssetsOfClassSoftly(TAssetClass::StaticClass(), AllAssets);
		UMCommon::CastTArrayOfTypes<TSoftObjectPtr<TAssetClass>>(AllAssets, OutAssets);
	}


	// loads and gets a list of all asset references for a given asset type
	static void GetAllAssetsForTypeLoaded(FPrimaryAssetType const& AssetType, TArray<UObject*>& OutAssets);

	// loads and gets a list of all asset references for a given asset class
	static void GetAllAssetsOfClassLoaded(const UClass* AssetClass, TArray<UObject*>& OutAssets)
	{
		return GetAllAssetsForTypeLoaded(GetPrimaryAssetTypeForClass(AssetClass), OutAssets);
	}

	// loads and gets a list of all asset references for a given asset class
	template <class TAssetClass>
	static void GetAllAssetsLoaded(TArray<TAssetClass*>& OutAssets)
	{
		TArray<UObject*> AllAssets;
		GetAllAssetsOfClassLoaded(TAssetClass::StaticClass(), AllAssets);
		UMCommon::CastTArrayOfPointers<TAssetClass>(AllAssets, OutAssets);
	}


	// get a random asset id from all items of a given type
	static FPrimaryAssetId GetRandomAssetIdOfType(FPrimaryAssetType const& AssetType, FName const& Seed = FName())
	{
		TArray<FPrimaryAssetId> AssetIdList;
		GetAllAssetIdsForType(AssetType, AssetIdList);


		return UMathTypes::GetRandomElement(AssetIdList, Seed);
	}

	// get a random asset id from all items of a given class
	static FPrimaryAssetId GetRandomAssetIdOfClass(const UClass* AssetClass, FName const& Seed = FName())
	{
		return GetRandomAssetIdOfType(GetPrimaryAssetTypeForClass(AssetClass), Seed);
	}

	// get a random asset id from all items of a given class
	template <class TAssetClass>
	static FPrimaryAssetId GetRandomAssetId(FName const& Seed = FName())
	{
		return GetRandomAssetIdOfClass(TAssetClass::StaticClass(), Seed);
	}


	// get a random asset data from all items of a given type
	static FAssetData GetRandomAssetDataOfType(FPrimaryAssetType const& AssetType, FName const& Seed = FName())
	{
		FAssetData Out;
		if (GetAssetDataFromPrimaryAssetId(GetRandomAssetIdOfType(AssetType, Seed), Out) == false)
		{
			return FAssetData();
		}


		return Out;
	}

	// get a random asset data from all items of a given class
	static FAssetData GetRandomAssetDataOfClass(const UClass* AssetClass, FName const& Seed = FName())
	{
		return GetRandomAssetDataOfType(GetPrimaryAssetTypeForClass(AssetClass), Seed);
	}

	// get a random asset data from all items of a given class
	template <class TAssetClass>
	static FAssetData GetRandomAssetData(FName const& Seed = FName())
	{
		return GetRandomAssetDataOfClass(TAssetClass::StaticClass(), Seed);
	}


	// get a random asset from all items of a given type
	static UObject* GetRandomAssetOfTypeLoaded(FPrimaryAssetType const& AssetType, FName const& Seed = FName())
	{
		return GetAssetFromPrimaryAssetIdLoaded(GetRandomAssetIdOfType(AssetType, Seed));
	}

	// get a random asset from all items of a given class
	static UObject* GetRandomAssetOfClassLoaded(const UClass* AssetClass, FName const& Seed = FName())
	{
		return GetRandomAssetOfTypeLoaded(GetPrimaryAssetTypeForClass(AssetClass), Seed);
	}

	// get a random asset from all items of a given class
	template <class TAssetClass>
	static TAssetClass* GetRandomAssetLoaded(FName const& Seed = FName())
	{
		return Cast<TAssetClass>(GetRandomAssetOfClassLoaded(TAssetClass::StaticClass(), Seed));
	}


	// get a random asset from all items of a given type
	static FSoftObjectPath GetRandomAssetOfTypeSoftly(FPrimaryAssetType const& AssetType, FName const& Seed = FName())
	{
		TArray<FSoftObjectPath> AssetList;
		GetAllAssetsForTypeSoftly(AssetType, AssetList);


		return UMathTypes::GetRandomElement(AssetList, Seed);
	}

	// get a random asset from all items of a given class
	static FSoftObjectPath GetRandomAssetOfClassSoftly(const UClass* AssetClass, FName const& Seed = FName())
	{
		return GetRandomAssetOfTypeSoftly(GetPrimaryAssetTypeForClass(AssetClass), Seed);
	}

	// get a random asset from all items of a given class
	template <class TAssetClass>
	static TSoftObjectPtr<TAssetClass> GetRandomAssetSoftly(FName const& Seed = FName())
	{
		return TSoftObjectPtr<TAssetClass>(GetRandomAssetOfClassSoftly(TAssetClass::StaticClass(), Seed));
	}


	// get the total number of assets of the given class
	static int GetTotalNumberOfAssetType(FPrimaryAssetType const& AssetType)
	{
		TArray<FPrimaryAssetId> AllAssetTypeIds;
		GetAllAssetIdsForType(AssetType, AllAssetTypeIds);
		return AllAssetTypeIds.Num();
	}

	// get the total number of assets of the given class
	static int GetTotalNumberOfAssetClass(const UClass* Class)
	{
		return GetTotalNumberOfAssetType(GetPrimaryAssetTypeForClass(Class));
	}

	// get the total number of assets of the given class
	template <class TAssetClass>
	static int GetTotalNumberOfAssets()
	{
		return GetTotalNumberOfAssetClass(TAssetClass::StaticClass());
	}


	// loads and validates assets of a given type by calling the given validation function on every asset
	// returns number of invalid assets
	// outputs names for each invalid asset
	template <class TAssetClass>
	static int LoadAndValidateAllAssets(TValidateObjectFunctionSignature<TAssetClass> ValidationFunction, TArray<FName>& OutInvalidAssetNames)
	{
		TArray<TAssetClass*> AllAssets;
		GetAllAssetsLoaded<TAssetClass>(AllAssets);

		int NumInvalid = 0;
		FOR_FORWARD(i, AllAssets.Num())
		{
			if (!ValidationFunction(AllAssets[i]))
			{
				NumInvalid += 1;
				OutInvalidAssetNames.Add(UMCommon::GetUObjectName(AllAssets[i]));
			}
			// dont break, continue to call IsValid() on every asset
		}


		return NumInvalid;
	}

	// loads and validates assets of a given type by calling the given validation function on every asset
	// returns number of invalid assets
	template <class TAssetClass>
	static int LoadAndValidateAllAssets(TValidateObjectFunctionSignature<TAssetClass> ValidationFunction)
	{
		TArray<FName> UnusedOutput;
		return LoadAndValidateAllAssets<TAssetClass>(ValidationFunction, UnusedOutput);
	}

	// Call to ensure that an asset with the given class / name exists.
	// Creates missing asset at editor runtime and add it to the asset registry.
	// If created by this call, it will throw an error to prevent shipping builds.
	// the editor will need to restart to recognize the new asset in asset manager
	// returns true if the asset exists
	static bool EnsureAssetExists(
		const UClass* NewAssetClass,
		FName const& NewAssetName,
		FString const& NewAssetDirectoryPath = FPaths::ProjectContentDir(),
		SetupObjectFunctionSignature SetupAssetFunctionPtr = nullptr
	);

	// Call to ensure that an asset with the given class / name exists.
	// If created by this call, it will throw an error to prevent shipping builds.
	// Will create a new asset at editor runtime and add it to the asset registry.
	// the editor will need to restart to recognize the new asset in asset manager
	// returns true if the asset exists
	template <class TAssetClass>
	static bool EnsureAssetExists(
		FName const& NewAssetName,
		FString const& NewAssetDirectoryPath = FPaths::ProjectContentDir(),
		SetupObjectFunctionSignature SetupAssetFunctionPtr = nullptr)
	{
		return EnsureAssetExists(
			TAssetClass::StaticClass(),
			NewAssetName,
			NewAssetDirectoryPath,
			SetupAssetFunctionPtr
		);
	}


#if WITH_EDITOR

	// check if the given class is listed as a primary asset in project settings
	// assets need to be listed or asset functions like GetAllAssets or GetRandomAsset will not work
	static bool CheckIfPrimaryAssetIsListedInAssetManagerSettings(const UClass* NewAssetClass);

	// check if the given class is listed as a primary asset in project settings
	// assets need to be listed or asset functions like GetAllAssets or GetRandomAsset will not work
	template <class TAssetClass>
	static bool CheckIfPrimaryAssetIsListedInAssetManagerSettings()
	{
		return CheckIfPrimaryAssetIsListedInAssetManagerSettings(TAssetClass::StaticClass());
	}

	// ensure the given class is listed as a primary asset in project settings
	// assets need to be listed or asset functions like GetAllAssets or GetRandomAsset will not work
	static void EnsureAssetTypeIsListedForScanningInProjectSettings(const UClass* AssetClass);

	// ensure the given class is listed as a primary asset in project settings
	// assets need to be listed or asset functions like GetAllAssets or GetRandomAsset will not work
	template <class TAssetClass>
	static void EnsureAssetTypeIsListedForScanningInProjectSettings()
	{
		EnsureAssetTypeIsListedForScanningInProjectSettings(TAssetClass::StaticClass());
	}

	// Call to create an asset of the given class at editor runtime
	// optionally can provide a name, and a function that modifies the created object
	// the editor will need to restart to recognize the new asset in asset manager
	// returns the created asset
	static UObject* CreateAssetInEditor(
		const UClass* NewAssetClass,
		FName const& NewAssetName = NAME_None,
		FString const& NewAssetDirectoryPath = FPaths::ProjectContentDir(),
		SetupObjectFunctionSignature SetupAssetFunctionPtr = nullptr
	);

	// Call to create an asset of the given class at editor runtime
	// optionally can provide a name, and a function that modifies the created object
	// the editor will need to restart to recognize the new asset in asset manager
	// returns the created asset
	template <class TAssetClass>
	static TAssetClass* CreateAssetInEditor(
		FName const& NewAssetName = NAME_None,
		FString const& NewAssetDirectoryPath = FPaths::ProjectContentDir(),
		SetupObjectFunctionSignature SetupAssetFunctionPtr = nullptr)
	{
		return Cast<TAssetClass>(
			CreateAssetInEditor(
				TAssetClass::StaticClass(),
				NewAssetName,
				NewAssetDirectoryPath,
				SetupAssetFunctionPtr
			)
		);
	}


#endif

};