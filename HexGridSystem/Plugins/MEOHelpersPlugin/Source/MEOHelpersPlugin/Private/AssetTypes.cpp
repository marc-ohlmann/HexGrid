// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "AssetTypes.h"

#include "MEOHelpersPlugin.h"

#include "Engine/AssetManager.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Engine/AssetManagerSettings.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "UObject/SavePackage.h"
#endif


bool UAssetTypes::DoesAssetExist(FPrimaryAssetId const& AssetId)
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	FAssetData UnusedData;
	return AssetManager.GetPrimaryAssetData(AssetId, UnusedData);
}


bool UAssetTypes::GetAssetDataFromPrimaryAssetId(FPrimaryAssetId const& AssetId, FAssetData& OutData)
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	if (AssetManager.GetPrimaryAssetData(AssetId, OutData) == false)
	{
		L_ERR(MEOHelpers, "failed to get asset data for id '{0}'", AssetId);
		return false;
	}

	//L_VERBOSE(MEOHelpers, "found asset data for id '{0}' : '{1}'", *AssetId, OutData.GetFullName());


	return true;
}


UObject* UAssetTypes::GetAssetFromPrimaryAssetIdLoaded(FPrimaryAssetId const& AssetId)
{
	FAssetData AssetData;
	if (GetAssetDataFromPrimaryAssetId(AssetId, AssetData) == false)
	{
		return nullptr;
	}


	return AssetData.GetAsset();
}


void UAssetTypes::GetAllAssetIdsForType(FPrimaryAssetType const& AssetType, TArray<FPrimaryAssetId>& OutAssetIds)
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	if (AssetManager.GetPrimaryAssetIdList(AssetType, OutAssetIds) == false)
	{
		L_WARN(MEOHelpers, "no assets found for type '{0}'. Make sure it is listed in 'primary asset types to scan' in asset manager project settings", AssetType.GetName());
	}


	return;
}


void UAssetTypes::GetAllAssetDataForType(FPrimaryAssetType const& AssetType, TArray<FAssetData>& OutAssetData)
{
	TArray<FPrimaryAssetId> AssetIdList;
	GetAllAssetIdsForType(AssetType, AssetIdList);

	FOR_FORWARD(i, AssetIdList.Num())
	{
		FAssetData NewAssetData;
		if (GetAssetDataFromPrimaryAssetId(AssetIdList[i], NewAssetData) == false)
		{
			continue;
		}

		OutAssetData.Add(NewAssetData);
	}


	return;
}


void UAssetTypes::GetAllAssetsForTypeLoaded(FPrimaryAssetType const& AssetType, TArray<UObject*>& OutAssets)
{
	TArray<FPrimaryAssetId> AssetIdList;
	GetAllAssetIdsForType(AssetType, AssetIdList);

	FOR_FORWARD(i, AssetIdList.Num())
	{
		FAssetData NewAssetData;
		UObject* NewAsset = GetAssetFromPrimaryAssetIdLoaded(AssetIdList[i]);
		if (UMCommon::IsUObjectPointerSafe(NewAsset) == false)
		{
			continue;
		}

		OutAssets.Add(NewAsset);
	}


	return;
}


void UAssetTypes::GetAllAssetsForTypeSoftly(FPrimaryAssetType const& AssetType, TArray<FSoftObjectPath>& OutAssets)
{
	TArray<FAssetData> AssetDataList;
	GetAllAssetDataForType(AssetType, AssetDataList);

	FOR_FORWARD(i, AssetDataList.Num())
	{
		OutAssets.Add(AssetDataList[i].GetSoftObjectPath());
	}


	return;
}


FSoftObjectPath UAssetTypes::GetAssetFromPrimaryAssetIdSoftly(FPrimaryAssetId const& AssetId)
{
	FAssetData AssetData;
	if (GetAssetDataFromPrimaryAssetId(AssetId, AssetData) == false)
	{
		return FSoftObjectPath();
	}

	
	return AssetData.GetSoftObjectPath();
}


bool UAssetTypes::EnsureAssetExists(
	const UClass* NewAssetClass,
	FName const& NewAssetName,
	FString const& NewAssetDirectoryPath,
	SetupObjectFunctionSignature SetupAssetFunctionPtr
)
{
	IF_NULL_RETURN_VAL_LERR(NewAssetClass, false, MEOHelpers, "asset class is null");

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const FString AssetFilePath = NewAssetDirectoryPath + TEXT("/") + NewAssetName.ToString();
	const FString AssetFilePathWithExtension = AssetFilePath + TEXT(".uasset");

	// check if the asset already exists
	if (DoesAssetExist(NewAssetClass, NewAssetName) || FileManager.FileExists(*AssetFilePathWithExtension))
	{
		// the default asset already exists -- this should be 
		L_VERBOSE(MEOHelpers, "asset of class '{0}' and name '{1}' already exists",
			NewAssetClass,
			NewAssetName
		);
		return true;
	}

	L_ERR(MEOHelpers, "default asset of class '{0}' and name '{1}' not found at path '{2}'",
		NewAssetClass,
		NewAssetName,
		AssetFilePath
	);

#if WITH_EDITOR

	L_ERR(MEOHelpers, "creating asset of class '{0}' and name '{1}' at path '{2}'",
		NewAssetClass,
		NewAssetName,
		AssetFilePath
	);

	return UMCommon::IsUObjectPointerSafe(
		UAssetTypes::CreateAssetInEditor(
			NewAssetClass,
			NewAssetName,
			NewAssetDirectoryPath,
			SetupAssetFunctionPtr
		)
	);
#else
	return false;
#endif
}


#if WITH_EDITOR


bool UAssetTypes::CheckIfPrimaryAssetIsListedInAssetManagerSettings(const UClass* NewAssetClass)
{
	const UAssetManager& AssetMgr = UAssetManager::Get();
	const UAssetManagerSettings& Settings = AssetMgr.GetSettings();

	FOR_FORWARD(i, Settings.PrimaryAssetTypesToScan.Num())
	{
		TSoftClassPtr<UObject> AssetBaseClass = Settings.PrimaryAssetTypesToScan[i].GetAssetBaseClass();
		if (AssetBaseClass == NewAssetClass)
		{
			return true;
		}
	}


	return false;
}


void UAssetTypes::EnsureAssetTypeIsListedForScanningInProjectSettings(const UClass* AssetClass)
{
	if (UAssetTypes::CheckIfPrimaryAssetIsListedInAssetManagerSettings(AssetClass) == true)
	{
		return;
	}

	UAssetManager& AssetMgr = UAssetManager::Get();
	UAssetManagerSettings& Settings = const_cast<UAssetManagerSettings&>(AssetMgr.GetSettings());

	// this doesnt save properly
	/*Settings.PrimaryAssetTypesToScan.Add(
		FPrimaryAssetTypeInfo(
			GetPrimaryAssetTypeForClass(AssetClass).GetName(),
			const_cast<UClass*>(AssetClass),
			true,
			false
		)
	);*/ 

	L_ERR(MEOHelpers, "class '{0}' missing in 'primary asset types to scan' in asset manager project settings", AssetClass);


	return;
}


UObject* UAssetTypes::CreateAssetInEditor(
const UClass* NewAssetClass, 
FName const& NewAssetName, 
FString const& NewAssetDirectoryPath,
SetupObjectFunctionSignature SetupAssetFunctionPtr)
{
	IF_NULL_RETURN_NULL_LERR(NewAssetClass, MEOHelpers, "asset class is null");

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const FString AssetFilePath = NewAssetDirectoryPath + TEXT("/") + NewAssetName.ToString();
	const FString AssetFilePathWithExtension = AssetFilePath + TEXT(".uasset");

	// check if the asset already exists
	if (DoesAssetExist(NewAssetClass, NewAssetName) || FileManager.FileExists(*AssetFilePathWithExtension))
	{
		// the default asset already exists -- this should be 
		L_WARN(MEOHelpers, "asset of class '{0}' and name '{1}' already exists",
			NewAssetClass,
			NewAssetName
		);
		return nullptr;
	}

	// make sure directory exists to place the default asset
	if (FileManager.DirectoryExists(*NewAssetDirectoryPath) == false)
	{
		if (FileManager.CreateDirectory(*NewAssetDirectoryPath))
		{
			L_ERR(MEOHelpers, "created directory '{0}' for default asset of class '{1}' and name '{2}'",
				NewAssetDirectoryPath,
				NewAssetClass,
				NewAssetName
			);
		}
		else
		{
			L_ERR(MEOHelpers, "failed to create directory '{0}' for default asset of class '{1}' and name '{2}'",
				NewAssetDirectoryPath,
				NewAssetClass,
				NewAssetName
			);
		}
	}

	FString PackageName = FString(TEXT("/Game/")) + NewAssetClass->GetName() + FString(TEXT("/")) + NewAssetName.ToString();
	FString FinalAssetName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, FinalAssetName);

	UPackage* AssetPackage = CreatePackage(*PackageName);
	IF_NULL_RETURN_NULL_LERR(AssetPackage, MEOHelpers, "failed to create package '{0}' for default asset class '{1}' with name: '{2}'",
		PackageName,
		NewAssetClass,
		NewAssetName
	);

	UObject* NewAsset = NewObject<UObject>(AssetPackage, NewAssetClass, NewAssetName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	IF_NULL_RETURN_NULL_LERR(NewAsset, MEOHelpers, "failed to create new default asset of class '{0}' {name: '{1}'}",
		NewAssetClass,
		NewAssetName
	);

	if (SetupAssetFunctionPtr)
	{
		SetupAssetFunctionPtr(NewAsset);
	}

	// save the asset
	AssetPackage->MarkPackageDirty();

	FSavePackageArgs SavePackageArgs = FSavePackageArgs();
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SavePackageArgs.bForceByteSwapping = true;
	SavePackageArgs.bWarnOfLongFilename = true;
	SavePackageArgs.bSlowTask = true;
	SavePackageArgs.FinalTimeStamp = FDateTime::Now();

	FSavePackageResultStruct SaveResult = UPackage::Save(AssetPackage, NewAsset, *AssetFilePathWithExtension, SavePackageArgs);
	FAssetRegistryModule::AssetCreated(AssetPackage);

	if (SaveResult.IsSuccessful())
	{
		L_WARN(MEOHelpers, "created and saved default asset of class '{0}' and name '{1}' wth package '{2}' at path '{3}' {save result: '{4}'}",
			NewAssetClass,
			FinalAssetName,
			PackageName,
			NewAssetDirectoryPath,
			static_cast<int>(SaveResult.Result)
		);
	}
	else
	{
		L_ERR(MEOHelpers, "created but failed to save default asset of class '{0}' and name '{1}' wth package '{2}' at path '{3}' {save result: '{4}'}",
			NewAssetClass,
			FinalAssetName,
			PackageName,
			NewAssetDirectoryPath,
			static_cast<int>(SaveResult.Result)
		);
	}

	L_ERR(MEOHelpers, "=======================================================================");
	L_WARN(MEOHelpers, "=======================================================================");
	L_WARN(MEOHelpers, "");
	L_WARN(MEOHelpers, "RESTART THE EDITOR TO ENSURE THE DEFAULT ASSET IS ADDED TO THE REGISTRY");
	L_WARN(MEOHelpers, "");
	L_WARN(MEOHelpers, "=======================================================================");
	L_ERR(MEOHelpers, "=======================================================================");


	return NewAsset;
}


#endif
