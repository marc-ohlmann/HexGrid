// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HexGrid.h"
#include "HexGridTypes.h"
#include "ExampleHexGridTileTypeAsset.h"
#include "ExampleHexGrid.generated.h"


class UMaterialInterface;
class UStaticMesh;


USTRUCT()
struct HEXGRIDSYSTEM_API FExampleHexGridPathfindingProperties final
#if CPP 
	: public THexGridPathfindingProperties<FExampleHexGridTileData>
#endif
{

	GENERATED_BODY();


	FExampleHexGridPathfindingProperties()
		: THexGridPathfindingProperties<FExampleHexGridTileData>()
	{
		Items.Empty();
	}

	FExampleHexGridPathfindingProperties(FHexGridCoordinateList const& InCoordinates)
		: FExampleHexGridPathfindingProperties()
	{
		Items.Append(InCoordinates.List);
	}


	UPROPERTY()
		TArray<FExampleHexGridTileData> Items;


	// THexGridPathfindingProperties Implementation

	// definition for virtual destructor to satisfy compiler warnings
	virtual ~FExampleHexGridPathfindingProperties() final
	{
		Items.Empty();
	}

	virtual FHexGridCoordinate GetCoordinateFromTile(FExampleHexGridTileData const& TileData) const final
	{
		return TileData.Coordinate;
	}

	// get all tile data at the given index
	virtual FExampleHexGridTileData GetTileFromIndex(int const& InIndex) const final
	{
		return Items[InIndex];
	}

	// get index for a tile at the given coordinate
	virtual int GetTileIndexFromCoordinate(FHexGridCoordinate const& InCoordinate) const final
	{
		const int NumberOfTiles = GetTotalNumberOfTiles();
		for (int i = 0; i < NumberOfTiles; i++)
		{
			if (Items[i].Coordinate == InCoordinate)
			{
				return i;
			}
		}

		return -1;
	}

	// get the total number of tiles in the grid
	virtual int GetTotalNumberOfTiles() const final
	{
		return Items.Num();
	}

};


/**
 *
 */
UCLASS()
class HEXGRIDSYSTEM_API AExampleHexGrid : public AHexGrid, public IAssetProviderInterface
{
	GENERATED_BODY()

public:

	AExampleHexGrid();

	FExampleHexGridTileData GetExampleTileData(FHexGridCoordinate const& TileCoordinate) const;


	virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const override;


protected:

	virtual void BeginPlay() override;
	virtual void HandleHexGridGenerateFinished() override;
	virtual void HandleFogOfWarInitialized() override;

	UPROPERTY(EditAnywhere)
	FFogOfWarSetupProperties FogOfWarProperties;

};
