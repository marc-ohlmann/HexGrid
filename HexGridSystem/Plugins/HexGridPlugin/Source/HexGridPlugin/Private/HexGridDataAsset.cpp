// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAsset.h"

#include "HexGridPlugin.h"
#include "MCommon.h"


UHexGridDataAsset::UHexGridDataAsset()
	: Super()
	, GridData()
#if WITH_EDITORONLY_DATA
	, VisualizationGridShape(EHexGridShape::hexagon)
	, VisualizationGridSize(32)
	, VisualizationEmptyTileAsset(nullptr)
	, OnVisualizationPropertyChanged()
	, OnPaintBrushStarted()
	, OnPaintBrushEnded()
	, OnBrushTargetsChanged()
	, OnTileMapChanged()
#endif
{
}


void UHexGridDataAsset::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	FOR_FORWARD(i, GridData.GridProperties.TileTypes.Num())
	{
		IAssetProviderInterface::CollectPreloadAsset(GridData.GridProperties.TileTypes[i], OutAssets);
	}

#if WITH_EDITORONLY_DATA

	IAssetProviderInterface::CollectPreloadAsset(VisualizationEmptyTileAsset, OutAssets);

#endif

	return;
}


#if WITH_EDITORONLY_DATA

void UHexGridDataAsset::GetEmptyTileCoordinatesForVisualization(FHexGridCoordinateList& OutCoordinates) const
{
	FHexGridCoordinateList EmptyCoordinates;
	FHexGridCoordinate::GetCoordinatesInShapeAroundCoordinate(FHexGridCoordinate::c_Zero, VisualizationGridShape, VisualizationGridSize, EmptyCoordinates);

	EmptyCoordinates.List.RemoveAllSwap(
		[this](const FHexGridCoordinate& Coord)
		{
			return GridData.TileMap.Contains(Coord);
		}
	);

	OutCoordinates.Append(EmptyCoordinates);


	return;
}

#endif


#if WITH_EDITOR

void UHexGridDataAsset::PostEditUndo()
{
	Super::PostEditUndo();

	BroadcastVisualizationChange();


	return;
}

void UHexGridDataAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();

	if (ChangedPropertyName.IsEqual(FName(FString(STRINGIFY(TileRadius)))) ||
		ChangedPropertyName.IsEqual(FName(FString(STRINGIFY(VisualizationEmptyTileAsset)))) ||
		ChangedPropertyName.IsEqual(FName(FString(STRINGIFY(VisualizationGridSize)))) ||
		ChangedPropertyName.IsEqual(FName(FString(STRINGIFY(VisualizationGridShape)))))
	{
		BroadcastVisualizationChange();
	}


	return;
}

#endif