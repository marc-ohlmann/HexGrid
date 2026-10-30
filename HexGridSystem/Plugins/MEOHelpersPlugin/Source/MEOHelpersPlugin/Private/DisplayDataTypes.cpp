// Copyright 2026 Marc Ohlmann, All Rights Reserved.


#include "DisplayDataTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "AssetProviderInterface.h"


void FDisplayData::CollectDisplayPreloadAssets(TArray<FSoftObjectPath>& OutAssets)
{
	IAssetProviderInterface::CollectPreloadAsset(Icon, OutAssets);


	return;
}