// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "ExampleHexGrid.h"

#include "HexGridSystem.h"
#include "HexGridSubsystem.h"
#include "MCommon.h"
#include "HexGridFogOfWarComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"


AExampleHexGrid::AExampleHexGrid()
{
	SetHexGridProperties(FHexGridSetupProperties(50.f, 36, EHexGridShape::hexagon, 3));
	FogOfWarProperties = FFogOfWarSetupProperties();
}


void AExampleHexGrid::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	FogOfWarProperties.CollectAssetsToPreload(OutAssets);


	return;
}


void AExampleHexGrid::BeginPlay()
{
	Super::BeginPlay();

	UHexGridSubsystem::RegisterHexGrid(this);


	return;
}


void AExampleHexGrid::HandleHexGridGenerateFinished()
{
	Super::HandleHexGridGenerateFinished();

	if (FogOfWarProperties.ComponentClass.IsNull() == false)
	{
		// start setup on the fog of war once the grid is initialized so that it can properly fit the grid
		SetupFogOfWar(FogOfWarProperties);
	}


	return;
}


void AExampleHexGrid::HandleFogOfWarInitialized()
{
	Super::HandleFogOfWarInitialized();

	UHexGridFogOfWarComponent* FogOfWar = GetFogOfWarComponent();
	IF_NULL_RETURN_LERR(FogOfWar, HexGridTest, "FogOfWar is null");

	FogOfWar->RegisterPlayerPerspective(0);
	FogOfWar->SetActivePerspectivePlayer(0);


	return;
}


FExampleHexGridTileData AExampleHexGrid::GetExampleTileData(FHexGridCoordinate const& TileCoordinate) const
{
	if (HasTile(TileCoordinate) == false)
	{
		L_ERR(HexGrid, "tile coordinate {0} doesnt exist", TileCoordinate);
		return FExampleHexGridTileData();
	}

	const UExampleHexGridTileTypeAsset* ExampleTileTypeAsset = GetTileTypeAtCoordinate<UExampleHexGridTileTypeAsset>(TileCoordinate);
	IF_NULL_RETURN_TYPE_LERR(ExampleTileTypeAsset, FExampleHexGridTileData, HexGridTest, "tile type asset is null for coordinate {0}", TileCoordinate);


	return FExampleHexGridTileData(ExampleTileTypeAsset->GetTileData(), TileCoordinate);
}
