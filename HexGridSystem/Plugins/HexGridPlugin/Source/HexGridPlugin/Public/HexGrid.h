// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridTypes.h"
#include "HexGrid.generated.h"


class UHexGridDataAsset;
class UHexGridTileTypeAsset;
class UHierarchicalInstancedStaticMeshComponent;
class UHexGridFogOfWarComponent;
struct FStreamableHandle;


DECLARE_MULTICAST_DELEGATE(FGridInitializationEvent);


// class that creates and manages a grid of hex tiles
UCLASS(Blueprintable, hideCategories = (
	"Actor Tick",
	Replication,
	Rendering,
	Collision,
	Actor,
	Input,
	HLOD,
	Physics,
	WorldPartition,
	Cooking,
	Events,
	DataLayers
	)
)
class HEXGRIDPLUGIN_API AHexGrid : public AActor
{
	GENERATED_BODY()

public:

	AHexGrid();

protected:

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

#if WITH_EDITORONLY_DATA

protected:

	// properties used to create the grid
	UPROPERTY(EditAnywhere)
	FHexGridSetupProperties GenerateGridProperties;

	// button used to create a grid within the editor
	UPROPERTY(EditInstanceOnly)
	bool GenerateGrid;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif


	// grid properties

public:

	// get properties used to describe the grid
	FHexGridProperties GetHexGridProperties() const { return _HexGridProperties; }

	// set properties used to describe the grid
	void SetHexGridProperties(FHexGridProperties const& InProperties)
	{
		_HexGridProperties = InProperties;
	}

private:

	// properties used to describe the grid
	UPROPERTY()
	FHexGridProperties _HexGridProperties;

	// 


	// grid tiles

public:

	// create the hex grid using the given grid properties and a given tileset
	void GenerateHexGrid(FHexGridProperties const& GridProperties, TArray<FHexGridTile> const& Tiles);

	// create the hex grid using the given grid setup properties to generate a tileset
	void GenerateHexGrid(FHexGridSetupProperties const& InSetupProperties);

	// create the hex grid using the given grid data properties
	void GenerateHexGrid(FHexGridData const& InGridData);

	// create the hex grid using the given grid data asset
	void GenerateHexGrid(UHexGridDataAsset* InGridDataAsset);

	// remove all coordinates from the given list that do not have valid tile actors
	void FilterForCoordinatesWithValidTiles(FHexGridCoordinateList& Coordinates);

	// register a tile type to the grid
	int RegisterTileType(const UHexGridTileTypeAsset* TileType);

	// register a tile type to the grid
	int RegisterTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType);

	// check if the given tile type index is valid
	bool IsValidTileTypeIndex(const int TileTypeIndex) const;

	// get the index of the given tile type if it is registered
	int GetRegisteredTileTypeIndex(const UHexGridTileTypeAsset* TileType) const;

	// get the index of the given tile type if it is registered
	int GetRegisteredTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const;

	// check if the given tile type is registered
	bool HasRegisteredTileType(const UHexGridTileTypeAsset* TileType) const
	{
		return IsValidTileTypeIndex(GetRegisteredTileTypeIndex(TileType));
	}

	// check if the given tile type is registered
	bool HasRegisteredTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const
	{
		return IsValidTileTypeIndex(GetRegisteredTileTypeIndex(TileType));
	}

	// create a tile at the given coordinate of the given type
	void AddTile(FHexGridTile const& Tile);

	// remove a tile at the given coordinate using current grid properties
	void RemoveTile(FHexGridCoordinate const& TileCoordinate);

	// replace a tile at the given coordinate with the given tile type index
	void ReplaceTile(FHexGridCoordinate const& TileCoordinate, const int NewTileTypeIndex);

	// replace a tile at the given coordinate with the given tile type
	void ReplaceTile(FHexGridCoordinate const& TileCoordinate, const UHexGridTileTypeAsset* NewTileType);

	// replace a tile at the given coordinate with the given tile type
	void ReplaceTile(FHexGridCoordinate const& TileCoordinate, TSoftObjectPtr<const UHexGridTileTypeAsset> const& NewTileType);

	// check if a tile exists at the given coordinate
	bool HasTile(FHexGridCoordinate const& TileCoordinate) const;

	// check if a tile exists in the map
	bool HasTiles(FHexGridCoordinateList const& InTiles) const;

	// get all coordinates for existing tiles
	void GetTileCoordinates(FHexGridCoordinateList& OutCoordinates) const
	{
		_TileMap.GetTileCoordinates(OutCoordinates);
	}

	// get the cached 2d bounds for the grid
	FBox2D GetGridBounds() const { return _CachedGridBounds; }

	// update the cached grid bounds volume data using the current set of existing tiles
	void UpdateGridBounds(bool bUpdateFogOfWar);

	// get the world transform for the tile at the given coordinate
	FTransform GetTileLocalTransform(FHexGridCoordinate const& TileCoordinate) const;

	// get the tile type asset associated with the given index
	const UHexGridTileTypeAsset* GetTileTypeFromIndex(const int TileTypeIndex) const;

	// get the tile type asset associated with the given index
	template <class T>
	const T* GetTileTypeFromIndex(const int TileTypeIndex) const
	{
		return Cast<T>(GetTileTypeFromIndex(TileTypeIndex));
	}

	// getter for the tile type object associated with the given tile coordinate
	const UHexGridTileTypeAsset* GetTileTypeAtCoordinate(FHexGridCoordinate const& TileCoordinate) const;

	// templated getter for the tile type object associated with the given tile coordinate
	template <class T>
	const T* GetTileTypeAtCoordinate(FHexGridCoordinate const& TileCoordinate) const
	{
		return Cast<T>(GetTileTypeAtCoordinate(TileCoordinate));
	}

	// get all tile coordinates at the given length from the grid center
	void GetAllTileCoordinatesAtLength(const int Length, FHexGridCoordinateList& OutCoordinates) const;

	// get largest tile length from the grid center
	int GetFurthestTileLength() const;

	// get total number of tiles in the grid
	int GetTotalNumberOfTiles() const
	{
		return _TileMap.GetNumberOfTiles();
	}

	// check if the grid has no tiles
	bool IsEmptyGrid() const { return GetTotalNumberOfTiles() <= 0; }

	// get the first existing tile relative to the given facing direction that is above the given coordinate
	// returns false if none are found
	bool GetFirstRelativeNeighborTile_Up(
		FHexGridCoordinate const& FromCoordinate,
		const EHexGridDirection FacingDirection,
		FHexGridCoordinate& OutNeighbor
	) const;

	// get the first existing tile relative to the given facing direction that is below the given coordinate
	// returns false if none are found
	bool GetFirstRelativeNeighborTile_Down(
		FHexGridCoordinate const& FromCoordinate,
		const EHexGridDirection FacingDirection,
		FHexGridCoordinate& OutNeighbor
	) const;

	// get the first existing tile relative to the given facing direction that is left of the given coordinate
	// returns false if none are found
	bool GetFirstRelativeNeighborTile_Left(
		FHexGridCoordinate const& FromCoordinate,
		const EHexGridDirection FacingDirection,
		FHexGridCoordinate& OutNeighbor
	) const;

	// get the first existing tile relative to the given facing direction that is right of the given coordinate
	// returns false if none are found
	bool GetFirstRelativeNeighborTile_Right(
		FHexGridCoordinate const& FromCoordinate,
		const EHexGridDirection FacingDirection,
		FHexGridCoordinate& OutNeighbor
	) const;

	// get a direct reference to the grid's tile map
	FHexGridTileMap& GetTileMapReference() { return _TileMap; }

	// generate a list of tile types
	void GenerateTileTypeList(TArray<const UHexGridTileTypeAsset*>& OutTileTypes) const
	{
		_TileMap.GenerateTileTypeList(OutTileTypes);
	}

	// generate a map of tile type indices
	void GenerateTileTypeIndexMap(TMap<FHexGridCoordinate, int>& OutGridTileTypeMap) const
	{
		_TileMap.GenerateTileTypeIndexMap(OutGridTileTypeMap);
	}

protected:

	// called when a tile's type data is changed
	virtual void HandleTileTypeChanged(FHexGridCoordinate const& TileCoordinate, const int OldTileTypeIndex, const int NewTileTypeIndex) {}

	// called when hex grid generation is finished
	virtual void HandleHexGridGenerateFinished() {}

	// called when a tile type is registered to the grid
	virtual void HandleTileTypeRegistered(const UHexGridTileTypeAsset* TileType) {}

private:

	// create a tile at the given coordinate of the given type
	void AddTile_Internal(FHexGridTile const& Tile);

	// remove a tile at the given coordinate using current grid properties
	void RemoveTile_Internal(FHexGridCoordinate const& TileCoordinate);

	// calculate the grid bounds using the corner locations of every existing tile coordinate
	FBox2D CalculateGridBounds() const;

	// cached grid boundary data
	UPROPERTY()
	FBox2D _CachedGridBounds;

	// handle to tile assets being streamed in
	TArray<TSharedPtr<FStreamableHandle>> _LoadingTilesHandles;

	// called when the grid tile assets used for generating are finished loading
	void HandleGeneratingTileAssetsLoaded(TArray<TSharedPtr<FStreamableHandle>> const& LoadedAssetHandles);

	// list of hierarchical instanced static mesh components used to render tiles
	UPROPERTY(Transient)
	TArray<UHierarchicalInstancedStaticMeshComponent*> _HismComponents;

	// map of hex grid cells and their corresponding component
	UPROPERTY(Transient)
	FHexGridTileMap _TileMap;

	// get the HISM instance id for the given coordinate
	int GetHismInstanceIdForCoordinate(FHexGridCoordinate const& Coordinate) const;

	// get the HISM instance ids for the given coordinates
	int GetHismInstanceIdsForCoordinates(FHexGridCoordinateList const& Coordinates, TArray<int>& OutHismIds) const;

#if WITH_EDITORONLY_DATA

	// the timestamp for when grid generation started
	UPROPERTY()
	FDateTime _GenerateGridStartTime;

	// get total elapsed time generating the grid
	float GetElapsedGenerateGridTimeSeconds() const
	{
		return (FDateTime::UtcNow() - _GenerateGridStartTime).GetTotalSeconds();
	}

#endif 

	// finishes hex grid generation
	void FinishGeneratingGrid();

	//


	// deferred grid generation

public:

	// delegate broadcasts when grid generation finishes
	FGridInitializationEvent OnHexGridGenerateFinished;

	// check if the grid is currently generating tiles
	bool IsGeneratingTiles() const { return _GeneratingTiles.Num() > 0; }

	// check if degenerating tiles prior to generating them
	// only valid if currently generating tiles
	EHexGridTileGenerationState GetTileGeneratorState() const { return _TileGeneratorState; }

protected:

	// enable ticking in editor
	virtual bool ShouldTickIfViewportsOnly() const override final
	{
		return true;
	}

	// called after the tiles at the given coordinates have been generated
	virtual void HandleTilesGenerated(TArray<FHexGridTile> const& InTiles) { return; }

	// called before the tiles at the given coordinates are removed
	virtual void HandleTilesDegenerated(TArray<FHexGridTile> const& InTiles) { return; }

private:

	// state of the tile generator
	UPROPERTY()
	EHexGridTileGenerationState _TileGeneratorState;

	// list of tiles to be generated
	UPROPERTY(Transient)
	TArray<FHexGridTile> _GeneratingTiles;

	// handles generating tiles on each frame
	void OnTick_GenerateTiles(float DeltaTime);

	// create the given set of tiles this frame
	void GenerateTilesThisFrame(TArray<FHexGridTile> const& InTiles);

	// remove the given set of tiles this frame
	void DegenerateTilesThisFrame(TArray<FHexGridTile> const& InTiles);

	//


	// tiles from/to world location

public:

	// get a hex grid coordinate associated with the given world location
	FHexGridCoordinate GetHexGridCoordinateAtWorldLocation(FVector const& WorldLocation) const;

	// get the tile type associated with the coordinate at the given world location
	const UHexGridTileTypeAsset* GetTileTypeAtWorldLocation(FVector const& WorldLocation) const
	{
		return GetTileTypeAtCoordinate(GetHexGridCoordinateAtWorldLocation(WorldLocation));
	}

	// templated getter for the tile associated with the coordinate at the given world location
	template <class T>
	T* GetTileTypeAtWorldLocation(FVector const& WorldLocation) const
	{
		return Cast<T>(GetTileTypeAtWorldLocation(WorldLocation));
	}

	// get a world location for the given grid coordinate
	FVector GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate const& GridCoordinate) const;

	// get a world rotation for the given grid direction
	FRotator GetWorldRotationFromHexGridDirection(const EHexGridDirection GridDirection) const;

	// get a world transform for the given grid direction
	FTransform GetWorldTransformFromHexGridTransform(FHexGridTransform const& GridTransform) const;

	// get the rotator yaw from a given hex grid direction, relative to the grid's yaw
	float GetYawFromHexGridDirection(const EHexGridDirection Direction) const;

	// get the rotator (yaw only) from a given hex grid direction, relative to the grid's yaw
	FRotator GetRotatorFromHexGridDirection(const EHexGridDirection Direction) const
	{
		return FRotator(0.f, GetYawFromHexGridDirection(Direction), 0.f);
	}

	// get the world location and grid yaw rotation for a given tile and hex grid direction
	// returns false if the tile does not exist
	bool GetWorldLocationAndRotationForTileCoordinateAndDirection(
		FHexGridCoordinate const& TileGridCoordinate,
		const EHexGridDirection Direction,
		FVector& OutLocation,
		FRotator& OutRotation
	) const;

	//


	// pathfinding

public:

	// use A* pathfinding on the grid to get a path between two tiles
	// the provided template grid data class should define heuristic and travel functions used
	// to determine whether tiles are traversable, and whether a shortest path / faster algorithm is desired
	template<typename TileDataClass>
	EGraphAStarResult GetPathBetweenTiles(
		const THexGridPathfindingProperties<TileDataClass>* HexGridData,
		FHexGridCoordinate const& StartTileCoordinate,
		FHexGridCoordinate const& EndTileCoordinate,
		FHexGridCoordinateList& OutPathCoordinates
	)
	{
		if (HexGridData)
		{
			FHexGridCoordinateList GridTiles;
			HexGridData->GenerateCoordinateList(GridTiles);
			if (HasTile(StartTileCoordinate) && HasTile(EndTileCoordinate) && HasTiles(GridTiles))
			{
				return HexGridData->FindPath(StartTileCoordinate, EndTileCoordinate, OutPathCoordinates);
			}
		}

		// grid data is invalid or tiles are missing
		return EGraphAStarResult::GoalUnreachable;
	}

	//


	// present actors

public:

	// get all actors present on any tile on this grid
	int GetAllPresentActorsOnGrid(TArray<AActor*>& OutActors) const;

	// get the actors present on this grid that match a given class
	int GetAllPresentActorsOfClassOnGrid(UClass* ActorClass, TArray<AActor*>& OutActors) const;

	// templated get the actors present on this grid that match a given class
	template <class TActorClass>
	int GetAllPresentActorsOfClassOnGrid(TArray<AActor*>& OutActors) const
	{
		GetAllPresentActorsOfClassOnGrid(TActorClass::StaticClass(), OutActors);
	}

	// templated get the actors present on this grid that match a given class
	template <class TActorClass>
	int GetAllPresentActorsOfClassOnGrid(TArray<TActorClass*>& OutActors) const
	{
		TArray<AActor*> TempActors;
		GetAllPresentActorsOfClassOnGrid(TActorClass::StaticClass(), TempActors);

		OutActors.Reserve(OutActors.Num() + TempActors.Num());

		int AddedCount = 0;
		for (AActor* Actor : TempActors)
		{
			if (TActorClass* CastedActor = Cast<TActorClass>(Actor))
			{
				OutActors.Add(CastedActor);
				AddedCount += 1;
			}
		}

		return AddedCount;
	}

	// check if an actor is present at the tile with the given coordinate
	bool AreAnyActorsPresentAtTile(FHexGridCoordinate const& Coordinate) const;

	// check if an actor of the given class is present at the tile with the given coordinate
	bool IsAnyActorOfClassPresentAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const;

	// templated check if an actor of the given class is present at the tile with the given coordinate
	template <class TActorClass>
	bool IsAnyActorOfClassPresentAtTile(FHexGridCoordinate const& Coordinate) const
	{
		return IsAnyActorOfClassPresentAtTile(TActorClass::StaticClass(), Coordinate);
	}

	// check if an actor is present on any tile on the grid
	bool IsActorPresentOnGrid(AActor* Actor) const;

	// get the first actor of a given class that is present at the tile with the given coordinate
	AActor* GetFirstPresentActorOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const;

	// templated get the first actor of a given class that is present at the tile with the given coordinate
	template <class TActorClass>
	TActorClass* GetFirstPresentActorOfClassAtTile(FHexGridCoordinate const& Coordinate) const
	{
		return Cast<TActorClass>(GetFirstPresentActorOfClassAtTile(TActorClass::StaticClass(), Coordinate));
	}

	// get actors present at the given coordinate
	int GetAllPresentActorsAtTile(FHexGridCoordinate const& Coordinate, TArray<AActor*>& OutActors) const;

	// get actors present at the given coordinate of a given class
	int GetAllPresentActorsOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate, TArray<AActor*>& OutActors) const;

	// templated get actors present at the given coordinate of a given class
	template <class TActorClass>
	int GetAllPresentActorsOfClassAtTile(FHexGridCoordinate const& Coordinate, TArray<AActor*>& OutActors) const
	{
		return GetAllPresentActorsOfClassAtTile(TActorClass::StaticClass(), Coordinate, OutActors);
	}

	// templated get actors present at the given coordinate of a given class
	template <class TActorClass>
	int GetAllPresentActorsOfClassAtTile(FHexGridCoordinate const& Coordinate, TArray<TActorClass*>& OutActors) const
	{
		TArray<AActor*> TempActors;
		GetAllPresentActorsOfClassAtTile(TActorClass::StaticClass(), Coordinate, TempActors);

		OutActors.Reserve(OutActors.Num() + TempActors.Num());

		int AddedCount = 0;
		for (AActor* Actor : TempActors)
		{
			if (TActorClass* CastedActor = Cast<TActorClass>(Actor))
			{
				OutActors.Add(CastedActor);
				AddedCount += 1;
			}
		}

		return AddedCount;
	}

	// get the number of actors present on the given tile
	int GetNumberOfPresentActorsAtTile(FHexGridCoordinate const& Coordinate) const;

	// get the number of actors present on the given tile of a given class
	int GetNumberOfPresentActorsOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const;

	// templated get the number of actors present on the given tile of a given class
	template <class TActorClass>
	int GetNumberOfPresentActorsOfClassAtTile(FHexGridCoordinate const& Coordinate) const
	{
		return GetNumberOfPresentActorsOfClassAtTile(TActorClass::StaticClass(), Coordinate);
	}

	// get the list of coordinates that the given actor is present at
	bool FindActorCoordinates(AActor* Actor, FHexGridCoordinateList& OutCoordinates) const;

	// registers the given actor as present to the given tile on the grid
	// returns the index to the new present actor
	int AddActorToTile(AActor* Actor, FHexGridCoordinate const& Coordinate);

	// finds any tile that the given actor is present at and removes it
	// searches the tile coordinate list to find the matching actor
	// use version that provides the coordinate for better performance
	// returns the number of removed indices
	int RemoveActorFromGrid(AActor* Actor);

	// remove the given actor present at the given tile
	// returns the number of removed indices
	int RemoveActorFromTile(FHexGridCoordinate const& ActorCoordinate, AActor* Actor);

	// remove all actors that are present from the given tile
	// returns the number of removed indices
	int RemoveAllActorsFromTile(FHexGridCoordinate const& ActorCoordinate);

	//


	// tile materials
public:

	// mesh material slot for the tile material
	static const int c_TileMeshMaterialSlot;

	// set custom data on the hism instance associated with the given tile coordinate
	void SetTileCustomDataValue(FHexGridCoordinate const& TileCoordinate, const int CustomDataIndex, const float CustomDataValue);

	// set custom data on the hism instances associated with the given tile coordinates
	void SetTileCustomDataValues(FHexGridCoordinateList const& TileCoordinates, const int CustomDataIndex, const float CustomDataValue);

	// set custom data on the hism instances associated with the given tile group
	void SetTileGroupCustomDataValues(FName const& TileGroupName, const int CustomDataIndex, const float CustomDataValue);

	// set tile highlight data on the hism instances associated with the given tile coordinate
	void SetTileHighlightData(FHexGridCoordinate const& TileCoordinate, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend);

	// set tile highlight data on the hism instances associated with the given tile coordinate
	void SetTileHighlightData(FHexGridCoordinateList const& TileCoordinates, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend);

	// set tile highlight data on the hism instances associated with the given tile coordinate
	void SetTileGroupHighlightData(FName const& TileGroupName, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend);


	//


	// tile groups
public:

	// check if the given tile group name is valid
	bool IsValidTileGroupName(FName const& TileGroupName) const
	{
		return _TileGroupMap.TileGroups.Contains(TileGroupName);
	}

	// create a new tile group with the given name
	void RegisterTileGroup(FName const& TileGroupName)
	{
		_TileGroupMap.RegisterTileGroup(TileGroupName);
	}

	// get the tiles in the given group name
	int GetTileGroup(FName const& TileGroupName, FHexGridCoordinateList& OutTileCoordinates) const
	{
		return _TileGroupMap.GetTileGroup(TileGroupName, OutTileCoordinates);
	}

	// set the tiles in the given group name
	void SetTileGroup(FName const& TileGroupName, FHexGridCoordinateList const& TileCoordinates)
	{
		_TileGroupMap.SetTileGroup(TileGroupName, TileCoordinates);
	}

	// set the tiles in the given group name
	void ClearTileGroup(FName const& TileGroupName)
	{
		_TileGroupMap.ClearTileGroup(TileGroupName);
	}

private:

	// map of group names and their tile lists
	UPROPERTY(Transient)
	FHexGridTileGroupMap _TileGroupMap;

	//


	// fog of war
public:

	// setup a fog of war component aligned to this grid
	void SetupFogOfWar(FFogOfWarSetupProperties const& SetupProperties);

	// check if the fog of war component exists
	bool HasFogOfWar() const;

	// get the fog of war component on this grid
	UHexGridFogOfWarComponent* GetFogOfWarComponent() const { return _FogOfWar; }

	// get the fog of war component on this grid
	template <class TFogComponentClass>
	TFogComponentClass* GetFogOfWarComponent() const
	{
		return Cast<TFogComponentClass>(GetFogOfWarComponent());
	}

	// delegate broadcasts when the fog of war is initialized
	FGridInitializationEvent OnFogOfWarInitialized;

protected:

	// called when the fog of war component is created
	virtual void HandleFogOfWarInitialized() { return; }

private:

	// reference to the fog of war component
	UPROPERTY()
	UHexGridFogOfWarComponent* _FogOfWar;

	// handle to tile assets being streamed in
	TSharedPtr<FStreamableHandle> _LoadingFogClassHandle;

	//


};
