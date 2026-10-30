// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexGrid.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "HexGridDataAsset.h"
#include "HexGridTileTypeAsset.h"
#include "MathTypes.h"
#include "HexGridFogOfWarComponent.h"
#include "ChainedAssetLoader.h"

#include "Components/SceneComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#if WITH_EDITOR

#include "DrawDebugHelpers.h"

#endif


const int AHexGrid::c_TileMeshMaterialSlot = 0;


AHexGrid::AHexGrid()
	: Super()
#if WITH_EDITORONLY_DATA
	, GenerateGridProperties(_HexGridProperties, EHexGridShape::hexagon, 3)
	, GenerateGrid(false)
#endif
	, _HexGridProperties()
	, _CachedGridBounds()
	, _LoadingTilesHandles()
	, _HismComponents()
	, _TileMap()
#if WITH_EDITORONLY_DATA
	, _GenerateGridStartTime()
#endif
	, OnHexGridGenerateFinished()
	, _TileGeneratorState(EHexGridTileGenerationState::inactive)
	, _GeneratingTiles()
	, _TileGroupMap()
	, OnFogOfWarInitialized()
	, _FogOfWar(nullptr)
	, _LoadingFogClassHandle()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent->SetMobility(EComponentMobility::Movable);
}


void AHexGrid::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	return;
}


void AHexGrid::BeginPlay()
{
	Super::BeginPlay();


	return;
}


void AHexGrid::BeginDestroy()
{
	FOR_FORWARD(i, _LoadingTilesHandles.Num())
	{
		if (_LoadingTilesHandles[i].IsValid())
		{
			_LoadingTilesHandles[i].Reset();
		}
	}
	_LoadingTilesHandles.Empty();

	if (_LoadingFogClassHandle.IsValid())
	{
		_LoadingFogClassHandle.Reset();
	}


	Super::BeginDestroy();
	return;
}


void AHexGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsGeneratingTiles())
	{
		OnTick_GenerateTiles(DeltaTime);
	}


	return;
}


#if WITH_EDITORONLY_DATA

void AHexGrid::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (GenerateGrid == true)
	{
		GenerateGrid = false;
		GenerateHexGrid(GenerateGridProperties);
	}


	return;
}

#endif


void AHexGrid::OnTick_GenerateTiles(float DeltaTime)
{
	if (_TileGeneratorState == EHexGridTileGenerationState::degeneration)
	{
		// degenerate the grid this frame
		TArray<FHexGridTile> ExistingTilesThisFrame;
		_TileMap.GetTiles(ExistingTilesThisFrame);
		const int TilesDegeneratedThisFrame = 
			(ExistingTilesThisFrame.Num() > _HexGridProperties.MaxTilesGeneratedPerFrame) 
				? _HexGridProperties.MaxTilesGeneratedPerFrame 
				: ExistingTilesThisFrame.Num();

		TArray<FHexGridTile> DegeneratingTilesThisFrame;
		FOR_FORWARD(i, TilesDegeneratedThisFrame)
		{
			DegeneratingTilesThisFrame.Add(ExistingTilesThisFrame[i]);
		}

		DegenerateTilesThisFrame(DegeneratingTilesThisFrame);

		if (_TileMap.GetNumberOfTiles() <= 0)
		{
			// transition to the generation state
			_TileGeneratorState = EHexGridTileGenerationState::generation;
		}
	}
	
	if (_TileGeneratorState == EHexGridTileGenerationState::generation)
	{
		// generate the grid this frame
		const int TotalTilesToGenerate = _GeneratingTiles.Num();
		const int TilesGeneratedThisFrame = 
			(TotalTilesToGenerate > _HexGridProperties.MaxTilesGeneratedPerFrame) 
				? _HexGridProperties.MaxTilesGeneratedPerFrame 
				: TotalTilesToGenerate;
		TArray<FHexGridTile> GeneratingTilesThisFrame;
		FOR_FORWARD(i, TilesGeneratedThisFrame)
		{
			GeneratingTilesThisFrame.Add(_GeneratingTiles.Pop());
		}

		GenerateTilesThisFrame(GeneratingTilesThisFrame);

		if (_GeneratingTiles.Num() <= 0)
		{
			FinishGeneratingGrid();
		}
	}


	return;
}


void AHexGrid::FinishGeneratingGrid()
{
#if WITH_EDITORONLY_DATA

	const float ElapsedTimeSeconds = GetElapsedGenerateGridTimeSeconds();

	L_VERBOSE(HexGrid, "generated {0} tiles in {1} seconds", _TileMap.GetNumberOfTiles(), ElapsedTimeSeconds);

#endif

	_TileGeneratorState = EHexGridTileGenerationState::inactive;

	UpdateGridBounds(false);

	HandleHexGridGenerateFinished();

	if (OnHexGridGenerateFinished.IsBound())
	{
		OnHexGridGenerateFinished.Broadcast();
	}


	return;
}


int AHexGrid::RegisterTileType(const UHexGridTileTypeAsset* TileType)
{
	IF_NULL_RETURN_VAL_LERR(TileType, -1, HexGrid, "TileType is null");

	const int TileTypeMapIndex = _TileMap.RegisterTileType(TileType);

	// create a new hismc for this tile type
	UHierarchicalInstancedStaticMeshComponent* NewHismc = UMCommon::CreateComponentForActorAtRuntime<UHierarchicalInstancedStaticMeshComponent>(this);
	IF_NULL_RETURN_VAL_LERR(NewHismc, -1, HexGrid, "could not create new hismc for tile type '{0}'", TileType);

	NewHismc->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	UMaterialInstanceDynamic* TileTypeMid = UHexGridTileTypeAsset::SetupTileIsmComponentMaterials(NewHismc, NewHismc, TileType);
	IF_NULL_RETURN_VAL_LERR(TileTypeMid, -1, HexGrid, "could not create new mid for tile type '{0}'", TileType);

	// collision
	NewHismc->SetCollisionResponseToAllChannels(ECR_Ignore); // do nothing on all other channels
	NewHismc->SetCollisionResponseToChannel(UHexGridTypes::c_HexTileTraceChannel, ECR_Block); // block on custom channel
	NewHismc->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // crucial for performance
	//NewHismc->SetCollisionObjectType(UHexGridTypes::c_HexTileTraceChannel); // channel used while moving ? probably not needed  

	const int HismcIndex = _HismComponents.Add(NewHismc);

	if (TileTypeMapIndex == HismcIndex)
	{
		L_VERBOSE(HexGrid, "registered tile type asset '{0}' at index '{1}'", TileType, TileTypeMapIndex);
	}
	else
	{
		L_ERR(HexGrid, "tile type index '{0}' does not match hismc index '{1}'", TileTypeMapIndex, HismcIndex);
	}

	HandleTileTypeRegistered(TileType);


	return TileTypeMapIndex;
}


int AHexGrid::RegisterTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType)
{
	return RegisterTileType(UMCommon::EnsureSoftObjectPtrLoaded(TileType));
}


void AHexGrid::AddTile_Internal(FHexGridTile const& Tile)
{
	if (_TileMap.IsValidTileTypeIndex(Tile.TileTypeIndex) == false)
	{
		L_ERR(HexGrid, "invalid tile type '{0}'", Tile.TileTypeIndex);
		return;
	}

	if (UMCommon::IsValidIndex(_HismComponents, Tile.TileTypeIndex) == false ||
		UMCommon::IsUObjectPointerSafe(_HismComponents[Tile.TileTypeIndex]) == false)
	{
		L_ERR(HexGrid, "no hismc found for tile type '{0}'", Tile.TileTypeIndex);
		return;
	}

	if (HasTile(Tile.Coordinate) == true)
	{
		L_ERR(HexGrid, "tile '{0}' already exists", Tile.Coordinate);
		return;
	}

	//L_VERBOSE(HexGrid, "adding tile '{0}'", TileCoordinate);

	const FTransform TileTransform = GetTileLocalTransform(Tile.Coordinate);

	// add new tile to hismc
	const int TileHismInstanceId = _HismComponents[Tile.TileTypeIndex]->AddInstance(TileTransform, false);

	_TileMap.RegisterTile(Tile, TileHismInstanceId);


	return;
}


void AHexGrid::AddTile(FHexGridTile const& Tile)
{
	AddTile_Internal(Tile);

	HandleTileTypeChanged(Tile.Coordinate, -1, Tile.TileTypeIndex);


	return;
}


void AHexGrid::RemoveTile_Internal(FHexGridCoordinate const& TileCoordinate)
{
	if (HasTile(TileCoordinate) == false)
	{
		return;
	}

	const int TileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);
	if (UMCommon::IsValidIndex(_HismComponents, TileTypeIndex) == false ||
		UMCommon::IsUObjectPointerSafe(_HismComponents[TileTypeIndex]) == false)
	{
		L_ERR(HexGrid, "no hismc found for tile type '{0}'", TileTypeIndex);
		return;
	}

	//L_VERBOSE(HexGrid, "removing tile '{0}'", TileCoordinate);

	// remove tile instance from hismc
	_HismComponents[TileTypeIndex]->RemoveInstance(
		_TileMap.GetRegisterContainingTile(TileCoordinate).HismInstanceId
	);

	_TileMap.RemoveTile(TileCoordinate);


	return;
}


void AHexGrid::RemoveTile(FHexGridCoordinate const& TileCoordinate)
{
	const int TileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);

	RemoveTile_Internal(TileCoordinate);

	HandleTileTypeChanged(TileCoordinate, TileTypeIndex, -1);


	return;
}


bool AHexGrid::HasTile(FHexGridCoordinate const& TileCoordinate) const
{
	if (_TileMap.ContainsTile(TileCoordinate) == false)
	{
		return false;
	}

	const int TileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);
	if (UMCommon::IsValidIndex(_HismComponents, TileTypeIndex) == false ||
		UMCommon::IsUObjectPointerSafe(_HismComponents[TileTypeIndex]) == false)
	{
		L_ERR(HexGrid, "no hismc found for tile type '{0}'", TileTypeIndex);
		return false;
	}


	return _HismComponents[TileTypeIndex]->IsValidInstance(_TileMap.GetRegisterContainingTile(TileCoordinate).HismInstanceId);
}


bool AHexGrid::HasTiles(FHexGridCoordinateList const& InTiles) const
{
	for (int i = 0; i < InTiles.Num(); i++)
	{
		if (HasTile(InTiles[i]) == false)
		{
			return false;
		}
	}


	return true;
}


const UHexGridTileTypeAsset* AHexGrid::GetTileTypeFromIndex(const int TileTypeIndex) const
{
	if (IsValidTileTypeIndex(TileTypeIndex) == false)
	{
		L_ERR(CivCraft, "invalid tile type index", TileTypeIndex);
		return nullptr;
	}


	return _TileMap.TileTypeRegistries[TileTypeIndex].TileType;
}


const UHexGridTileTypeAsset* AHexGrid::GetTileTypeAtCoordinate(FHexGridCoordinate const& TileCoordinate) const
{
	if (_TileMap.ContainsTile(TileCoordinate) == false)
	{
		L_ERR(HexGrid, "tile '{0}' does not exist", TileCoordinate);
		return nullptr;
	}


	return _TileMap.GetTypeRegistryContainingTile(TileCoordinate).TileType;
}


void AHexGrid::DegenerateTilesThisFrame(TArray<FHexGridTile> const& InTiles)
{
	HandleTilesDegenerated(InTiles);

	FOR_FORWARD(i, InTiles.Num())
	{
		RemoveTile_Internal(InTiles[i].Coordinate);
	}


	return;
}


void AHexGrid::GenerateTilesThisFrame(TArray<FHexGridTile> const& InTiles)
{
	FOR_FORWARD(i, InTiles.Num())
	{
		AddTile_Internal(InTiles[i]);
	}

	HandleTilesGenerated(InTiles);


	return;
}


void AHexGrid::GenerateHexGrid(FHexGridProperties const& GridProperties, TArray<FHexGridTile> const& Tiles)
{
	_HexGridProperties = GridProperties;
	const int TileCount = Tiles.Num();
	_GeneratingTiles.Reset();
	_GeneratingTiles.Reserve(TileCount);
	_GeneratingTiles.Append(Tiles);

	if (_GeneratingTiles.Num() <= 0 && _TileMap.GetNumberOfTiles() <= 0)
	{
		L_ERR(CivCraft, "no grid data to generate from empty grid");
		FinishGeneratingGrid();
		return;
	}

	if (GridProperties.TileTypes.Num() > 0)
	{
		// load in the tile type assets before starting to generate tiles
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		TArray<FSoftObjectPath> AssetsToLoad;
		UMCommon::MakeSoftObjectPathsArray(GridProperties.TileTypes, AssetsToLoad);

		FChainedAssetLoader::LoadAssetsRecursively(
			AssetsToLoad,
			FOnAllAssetsLoaded::CreateUObject(this, &AHexGrid::HandleGeneratingTileAssetsLoaded)
		);
	}
	else
	{
		// start generating immediately
		HandleGeneratingTileAssetsLoaded({});
	}


	return;
}


void AHexGrid::HandleGeneratingTileAssetsLoaded(TArray<TSharedPtr<FStreamableHandle>> const& LoadedAssetHandles)
{
	// setup valid tile types
	FOR_FORWARD(i, _HexGridProperties.TileTypes.Num())
	{
		RegisterTileType(_HexGridProperties.TileTypes[i]);
	}

	// release the previously loaded tile assets reference
	FOR_FORWARD(i, _LoadingTilesHandles.Num())
	{
		if (_LoadingTilesHandles[i].IsValid())
		{
			_LoadingTilesHandles[i].Reset();
		}
	}
	_LoadingTilesHandles.Reset();
	_LoadingTilesHandles.Reserve(LoadedAssetHandles.Num());
	_LoadingTilesHandles.Append(LoadedAssetHandles);

	// start the degeneration phase to kickoff the generation process
	_TileGeneratorState = EHexGridTileGenerationState::degeneration;

	L_DBG(HexGrid, "");

#if WITH_EDITORONLY_DATA

	_GenerateGridStartTime = FDateTime::UtcNow();

#endif

#if false
	L_DBG(HexGrid, "{0} generating {1} tiles with {2} tile types at a rate of {3} tiles per frame",
		this,
		TileCount,
		_HexGridProperties.TileTypes.Num(),
		_HexGridProperties.MaxTilesGeneratedPerFrame
	);
#endif


	return;
}



void AHexGrid::GenerateHexGrid(FHexGridSetupProperties const& SetupProperties)
{
	FHexGridCoordinateList TileCoordinates;
	SetupProperties.CreateGridCoordinates(TileCoordinates);

	TArray<FHexGridTile> Tiles;
	FOR_FORWARD(i, TileCoordinates.Num())
	{
		// default to random tile type
		const int TileTypeIndex = UMathTypes::GetRandomInt(0, SetupProperties.TileTypes.Num() - 1);

		Tiles.Add(FHexGridTile(TileCoordinates[i], TileTypeIndex));
	}

	GenerateHexGrid(SetupProperties, Tiles);


	return;
}


void AHexGrid::GenerateHexGrid(FHexGridData const& InGridData)
{
	TArray<FHexGridTile> GridTiles;
	InGridData.GetTiles(GridTiles);
	GenerateHexGrid(InGridData.GridProperties, GridTiles);


	return;
}


void AHexGrid::GenerateHexGrid(UHexGridDataAsset* InGridDataAsset)
{
	IF_NULL_RETURN_LERR(InGridDataAsset, HexGrid, "InGridDataAsset is null");

	GenerateHexGrid(InGridDataAsset->GridData);


	return;
}


FHexGridCoordinate AHexGrid::GetHexGridCoordinateAtWorldLocation(FVector const& WorldLocation) const
{
	const FVector GridCenterLocation = GetActorLocation();
	const FRotator GridRotation = GetActorRotation();
	const FVector WorldOffsetLocation = (WorldLocation - GridCenterLocation).RotateAngleAxis(-1.f * GridRotation.Yaw, FVector::UpVector);


	return FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(WorldOffsetLocation, _HexGridProperties.TileRadius);
}


FTransform AHexGrid::GetTileLocalTransform(FHexGridCoordinate const& TileCoordinate) const
{
	const FVector TileOffsetLocation = _HexGridProperties.GetCoordinateWorldOffsetLocation(TileCoordinate);
	return FTransform(
		FRotator::ZeroRotator,
		TileOffsetLocation,
		FVector::OneVector
	);
}


FVector AHexGrid::GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate const& GridCoordinate) const
{
	const FVector TileOffsetLocation = _HexGridProperties.GetCoordinateWorldOffsetLocation(GridCoordinate);
	const FVector GridCenterLocation = GetActorLocation();
	const float GridYaw = GetActorRotation().Yaw;


	return TileOffsetLocation.RotateAngleAxis(GridYaw, FVector::UpVector) + GridCenterLocation;
}


FRotator AHexGrid::GetWorldRotationFromHexGridDirection(const EHexGridDirection GridDirection) const
{
	const FRotator GridRotation = GetActorRotation();
	const FRotator DirectionRotation = FHexGridCoordinate::GetAbsoluteRotationFromDirection(GridDirection);


	return GridRotation + DirectionRotation;
}


FTransform AHexGrid::GetWorldTransformFromHexGridTransform(FHexGridTransform const& GridTransform) const
{
	const FVector CoordinateLocation = GetWorldLocationFromHexGridCoordinate(GridTransform.Coordinate);
	const FRotator DirectionRotation = GetWorldRotationFromHexGridDirection(GridTransform.FacingDirection);



	return FTransform(DirectionRotation, CoordinateLocation, FVector::OneVector);
}


float AHexGrid::GetYawFromHexGridDirection(const EHexGridDirection Direction) const
{
	return FMath::UnwindDegrees(FHexGridCoordinate::GetAbsoluteYawFromDirection(Direction) + GetActorRotation().Yaw);
}


bool AHexGrid::GetWorldLocationAndRotationForTileCoordinateAndDirection(
	FHexGridCoordinate const& TileGridCoordinate,
	const EHexGridDirection Direction,
	FVector& OutLocation,
	FRotator& OutRotation
) const
{
	if (HasTile(TileGridCoordinate) == false)
	{
		return false;
	}

	OutLocation = GetWorldLocationFromHexGridCoordinate(TileGridCoordinate);
	OutRotation = GetRotatorFromHexGridDirection(Direction);


	return true;
}


int AHexGrid::GetFurthestTileLength() const
{
	FHexGridCoordinateList TileCoordinates;
	GetTileCoordinates(TileCoordinates);

	int FurthestLength = 0;
	FOR_FORWARD(i, TileCoordinates.Num())
	{
		const int LengthIterator = TileCoordinates[i].Length();
		if (FurthestLength < LengthIterator)
		{
			FurthestLength = LengthIterator;
		}
	}


	return FurthestLength;
}


void AHexGrid::GetAllTileCoordinatesAtLength(const int Length, FHexGridCoordinateList& OutArray) const
{
	FHexGridCoordinateList TileCoordinates;
	GetTileCoordinates(TileCoordinates);

	FOR_FORWARD(i, TileCoordinates.Num())
	{
		if (TileCoordinates[i].Length() == Length)
		{
			OutArray.Add(TileCoordinates[i]);
		}
	}


	return;
}


bool AHexGrid::GetFirstRelativeNeighborTile_Up(
	FHexGridCoordinate const& FromCoordinate,
	const EHexGridDirection FacingDirection,
	FHexGridCoordinate& OutNeighbor
) const
{
	FHexGridCoordinateList PotentialCoordinates = FromCoordinate.GetRelativeNeighbors_Up(FacingDirection);
	FOR_FORWARD(i, PotentialCoordinates.Num())
	{
		if (HasTile(PotentialCoordinates[i]))
		{
			OutNeighbor = PotentialCoordinates[i];
			return true;
		}
	}


	return false;
}


bool AHexGrid::GetFirstRelativeNeighborTile_Down(
	FHexGridCoordinate const& FromCoordinate,
	const EHexGridDirection FacingDirection,
	FHexGridCoordinate& OutNeighbor
) const
{
	FHexGridCoordinateList PotentialCoordinates = FromCoordinate.GetRelativeNeighbors_Down(FacingDirection);
	FOR_FORWARD(i, PotentialCoordinates.Num())
	{
		if (HasTile(PotentialCoordinates[i]))
		{
			OutNeighbor = PotentialCoordinates[i];
			return true;
		}
	}


	return false;
}


bool AHexGrid::GetFirstRelativeNeighborTile_Left(
	FHexGridCoordinate const& FromCoordinate,
	const EHexGridDirection FacingDirection,
	FHexGridCoordinate& OutNeighbor
) const
{
	FHexGridCoordinateList PotentialCoordinates = FromCoordinate.GetRelativeNeighbors_Left(FacingDirection);
	FOR_FORWARD(i, PotentialCoordinates.Num())
	{
		if (HasTile(PotentialCoordinates[i]))
		{
			OutNeighbor = PotentialCoordinates[i];
			return true;
		}
	}


	return false;
}


bool AHexGrid::GetFirstRelativeNeighborTile_Right(
	FHexGridCoordinate const& FromCoordinate,
	const EHexGridDirection FacingDirection,
	FHexGridCoordinate& OutNeighbor
) const
{
	FHexGridCoordinateList PotentialCoordinates = FromCoordinate.GetRelativeNeighbors_Right(FacingDirection);
	FOR_FORWARD(i, PotentialCoordinates.Num())
	{
		if (HasTile(PotentialCoordinates[i]))
		{
			OutNeighbor = PotentialCoordinates[i];
			return true;
		}
	}


	return false;
}


int AHexGrid::GetAllPresentActorsOnGrid(TArray<AActor*>& OutArray) const
{
	return _TileMap.GetAllPresentActors(OutArray);
}


int AHexGrid::GetAllPresentActorsOfClassOnGrid(UClass* ActorClass, TArray<AActor*>& OutArray) const
{
	return _TileMap.GetAllPresentActors(OutArray);
}


bool AHexGrid::AreAnyActorsPresentAtTile(FHexGridCoordinate const& Coordinate) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		L_ERR(HexGrid, "tile not found at coordinate '{0}'", Coordinate);
		return false;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).AreActorsPresent();
}


bool AHexGrid::IsAnyActorOfClassPresentAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		L_ERR(HexGrid, "tile not found at coordinate '{0}'", Coordinate);
		return false;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).IsAnyActorOfClassPresent(ActorClass);
}


bool AHexGrid::IsActorPresentOnGrid(AActor* Actor) const
{
	return _TileMap.ContainsActor(Actor);
}


AActor* AHexGrid::GetFirstPresentActorOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return nullptr;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).GetFirstPresentActorOfClass(ActorClass);
}


int AHexGrid::GetAllPresentActorsAtTile(FHexGridCoordinate const& Coordinate, TArray<AActor*>& OutActors) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return 0;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).GetAllPresentActors(OutActors);
}


int AHexGrid::GetAllPresentActorsOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate, TArray<AActor*>& OutActors) const
{
	IF_NULL_RETURN_VAL_LERR(ActorClass, 0, HexGrid, "ActorClass is null");

	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return 0;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).GetAllPresentActorsOfClass(ActorClass, OutActors);
}


int AHexGrid::GetNumberOfPresentActorsAtTile(FHexGridCoordinate const& Coordinate) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return 0;
	}

	return _TileMap.GetRegisterContainingTile(Coordinate).GetNumberOfPresentActors();
}


int AHexGrid::GetNumberOfPresentActorsOfClassAtTile(UClass* ActorClass, FHexGridCoordinate const& Coordinate) const
{
	IF_NULL_RETURN_VAL_LERR(ActorClass, 0, HexGrid, "ActorClass is null");

	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return 0;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).GetNumberOfPresentActorsOfClass(ActorClass);
}


bool AHexGrid::FindActorCoordinates(AActor* Actor, FHexGridCoordinateList& OutCoordinates) const
{
	IF_NULL_RETURN_VAL_LERR(Actor, false, HexGrid, "actor is null");

	bool bDidFindActor = false;
	if (_TileMap.FindActorCoordinates(Actor, OutCoordinates))
	{
		bDidFindActor = true;
	}
	else
	{
		L_WARN(HexGrid, "could not find actor '{0}' on grid", Actor);
	}


	return bDidFindActor;
}


int AHexGrid::AddActorToTile(AActor* Actor, FHexGridCoordinate const& Coordinate)
{
	IF_NULL_RETURN_VAL_LERR(Actor, -1, HexGrid, "actor is null");

	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		L_ERR(HexGrid, "tile coordinate '{0}' does not exist on the grid", Coordinate);
		return -1;
	}


	FHexGridTileRegister& TileRegister = _TileMap.GetRegisterContainingTile(Coordinate);
	if (TileRegister.IsActorPresent(Actor))
	{
		L_ERR(HexGrid, "actor '{0}' already present at tile coordinate '{1}'", Actor, Coordinate);
		return -1;
	}


	return TileRegister.AddPresentActor(Actor);
}


int AHexGrid::RemoveActorFromGrid(AActor* Actor)
{
	IF_NULL_RETURN_VAL_LERR(Actor, 0, HexGrid, "actor is null");

	int FoundIndices = 0;
	FHexGridCoordinateList ActorCoordinateList; 
	FindActorCoordinates(Actor, ActorCoordinateList);

	FOR_FORWARD(i, ActorCoordinateList.Num())
	{
		FoundIndices += RemoveActorFromTile(ActorCoordinateList[i], Actor);
	}


	return FoundIndices;
}


int AHexGrid::RemoveActorFromTile(FHexGridCoordinate const& ActorCoordinate, AActor* Actor)
{
	if (_TileMap.ContainsTile(ActorCoordinate) == false)
	{
		L_ERR(HexGrid, "tile at coordinate '0}' does not exist on the grid", ActorCoordinate);
		return 0;
	}

	FHexGridTileRegister& TileRegister = _TileMap.GetRegisterContainingTile(ActorCoordinate);


	return TileRegister.RemovePresentActor(Actor);
}


int AHexGrid::RemoveAllActorsFromTile(FHexGridCoordinate const& ActorCoordinate)
{
	if (_TileMap.ContainsTile(ActorCoordinate) == false)
	{
		L_ERR(HexGrid, "tile at coordinate '0}' does not exist on the grid", ActorCoordinate);
		return 0;
	}

	FHexGridTileRegister& TileRegister = _TileMap.GetRegisterContainingTile(ActorCoordinate);
	const int PresentActorCount = TileRegister.GetNumberOfPresentActors();
	TileRegister.ClearPresentActors();


	return PresentActorCount;
}


void AHexGrid::FilterForCoordinatesWithValidTiles(FHexGridCoordinateList& Coordinates)
{
	Coordinates.List.RemoveAllSwap(
		[this](const FHexGridCoordinate& Coord)
		{
			return HasTile(Coord) == false;
		}
	);


	return;
}


int AHexGrid::GetHismInstanceIdForCoordinate(FHexGridCoordinate const& Coordinate) const
{
	if (_TileMap.ContainsTile(Coordinate) == false)
	{
		return -1;
	}


	return _TileMap.GetRegisterContainingTile(Coordinate).HismInstanceId;
}


int AHexGrid::GetHismInstanceIdsForCoordinates(FHexGridCoordinateList const& Coordinates, TArray<int>& OutHismIds) const
{
	int Total = 0;
	FOR_FORWARD(i, Coordinates.Num())
	{
		const int HismId = GetHismInstanceIdForCoordinate(Coordinates[i]);
		if (HismId != -1)
		{
			Total += 1;
			OutHismIds.Add(HismId);
		}
	}


	return Total;
}


void AHexGrid::SetTileCustomDataValue(FHexGridCoordinate const& TileCoordinate, const int CustomDataIndex, const float CustomDataValue)
{
	if (_TileMap.ContainsTile(TileCoordinate) == false)
	{
		L_ERR(HexGrid, "tile '{0}' does not exist", TileCoordinate);
		return;
	}

	const int TileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);
	if (UMCommon::IsValidIndex(_HismComponents, TileTypeIndex) == false ||
		UMCommon::IsUObjectPointerSafe(_HismComponents[TileTypeIndex]) == false)
	{
		L_ERR(HexGrid, "no hismc found for tile type '{0}'", TileTypeIndex);
		return;
	}

	const int HismInstanceId = GetHismInstanceIdForCoordinate(TileCoordinate);

	_HismComponents[TileTypeIndex]->SetCustomDataValue(HismInstanceId, CustomDataIndex, CustomDataValue, true);

#if false
	L_DBG(HexGrid, "updating tile instance at '{0}' with type id '{1}', hism id '{2}', to set custom data index '{3}' to value '{4}'",
		TileCoordinate,
		TileTypeIndex,
		HismInstanceId,
		CustomDataIndex,
		CustomDataValue
	);
#endif


	return;
}


void AHexGrid::SetTileCustomDataValues(FHexGridCoordinateList const& TileCoordinates, const int CustomDataIndex, const float CustomDataValue)
{
	FOR_FORWARD(i, TileCoordinates.Num())
	{
		SetTileCustomDataValue(TileCoordinates[i], CustomDataIndex, CustomDataValue);
	}


	return;
}


void AHexGrid::SetTileGroupCustomDataValues(FName const& TileGroupName, const int CustomDataIndex, const float CustomDataValue)
{
	if (IsValidTileGroupName(TileGroupName) == false)
	{
		RegisterTileGroup(TileGroupName);
	}

	FHexGridCoordinateList TileGroupCoordinates;
	GetTileGroup(TileGroupName, TileGroupCoordinates);
	SetTileCustomDataValues(TileGroupCoordinates, CustomDataIndex, CustomDataValue);


	return;
}


void AHexGrid::SetTileHighlightData(FHexGridCoordinate const& TileCoordinate, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend)
{
	if (_TileMap.ContainsTile(TileCoordinate) == false)
	{
		return;
	}

	const int TileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);
	if (UMCommon::IsValidIndex(_HismComponents, TileTypeIndex) == false ||
		UMCommon::IsUObjectPointerSafe(_HismComponents[TileTypeIndex]) == false)
	{
		L_ERR(HexGrid, "no hismc found for tile type '{0}'", TileTypeIndex);
		return;
	}

	const int IsmInstanceId = _TileMap.GetRegisterContainingTile(TileCoordinate).HismInstanceId;

	if (_HismComponents[TileTypeIndex]->IsValidInstance(IsmInstanceId))
	{
		UHexGridTileTypeAsset::SetTileInstanceMaterialHighlightData(_HismComponents[TileTypeIndex], IsmInstanceId, HighlightColor, HighlightIntensity, HighlightRainbowBlend);

#if false
		L_DBG(HexGrid, "updating tile instance at '{0}' with type '{1}', ism id '{2}', to set custom data index '{3}' to value '{4}'",
			TileCoordinate,
			UMCommon::GetSoftPtrNameString(TileType),
			IsmInstanceId,
			CustomDataIndex,
			CustomDataValue
		);
#endif
	}
	else
	{
		L_WARN(HexGrid, "invalid ism instance id found for coordinate '{0}'", TileCoordinate);

		_TileMap.RemoveTile(TileCoordinate);
	}


	return;
}


void AHexGrid::SetTileHighlightData(FHexGridCoordinateList const& TileCoordinates, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend)
{
	FOR_FORWARD(i, TileCoordinates.Num())
	{
		SetTileHighlightData(TileCoordinates[i], HighlightColor, HighlightIntensity, HighlightRainbowBlend);
	}


	return;
}


void AHexGrid::SetTileGroupHighlightData(FName const& TileGroupName, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend)
{
	FHexGridCoordinateList TileGroupCoordinates;
	GetTileGroup(TileGroupName, TileGroupCoordinates);
	SetTileHighlightData(TileGroupCoordinates, HighlightColor, HighlightIntensity, HighlightRainbowBlend);


	return;
}


void AHexGrid::SetupFogOfWar(FFogOfWarSetupProperties const& SetupProperties)
{
	if (HasFogOfWar())
	{
		L_ERR(HexGrid, "fog of war already setup");
		return;
	}

	// Define a lambda that handles the actual initialization logic
	auto OnLoadedLambda = [this, SetupProperties]()
		{
			UClass* FogOfWarComponentClass = UMCommon::EnsureSoftClassPtrLoaded(SetupProperties.ComponentClass);
			IF_NULL_RETURN_LERR(FogOfWarComponentClass, HexGrid, "FogOfWarComponentClass is null");

			_FogOfWar = UMCommon::CreateComponentForActorAtRuntime<UHexGridFogOfWarComponent>(this, FogOfWarComponentClass);
			IF_NULL_RETURN_LERR(_FogOfWar, HexGrid, "failed to create fog of war component");

			_FogOfWar->InitializeFogOfWar(SetupProperties);

			// Notify and Broadcast
			HandleFogOfWarInitialized();
			if (OnFogOfWarInitialized.IsBound())
			{
				OnFogOfWarInitialized.Broadcast();
			}
		};

	if (SetupProperties.ComponentClass.IsNull() == false)
	{
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		// Bind the lambda to the delegate
		_LoadingFogClassHandle = StreamableManager.RequestAsyncLoad(
			SetupProperties.ComponentClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(OnLoadedLambda)
		);
	}
	else
	{
		// Execute immediately if no async load is needed
		OnLoadedLambda();
	}


	return;
}


bool AHexGrid::HasFogOfWar() const
{
	return UMCommon::IsUObjectPointerSafe(_FogOfWar);
}


FBox2D AHexGrid::CalculateGridBounds() const
{
	FHexGridCoordinateList GridCoordinates;
	GetTileCoordinates(GridCoordinates);
	if (GridCoordinates.Num() <= 0)
	{
		L_ERR(HexGrid, "no coordinates found");
		return FBox2d();
	}

	const FHexGridProperties GridProperties = GetHexGridProperties();


	return GridCoordinates.GetBounds(GridProperties.TileRadius);
}


void AHexGrid::UpdateGridBounds(bool bUpdateFogOfWar)
{
	_CachedGridBounds = CalculateGridBounds();

	if (bUpdateFogOfWar)
	{
		UHexGridFogOfWarComponent* FogComponent = GetFogOfWarComponent();
		if (UMCommon::IsUObjectPointerSafe(FogComponent))
		{
			FogComponent->TransformToFitBounds(_CachedGridBounds);
		}
	}


	return;
}


bool AHexGrid::IsValidTileTypeIndex(const int TileTypeIndex) const
{
	return _TileMap.IsValidTileTypeIndex(TileTypeIndex);
}


int AHexGrid::GetRegisteredTileTypeIndex(const UHexGridTileTypeAsset* TileType) const
{
	return _TileMap.GetTileTypeIndex(TileType);
}


int AHexGrid::GetRegisteredTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const
{
	return GetRegisteredTileTypeIndex(UMCommon::EnsureSoftObjectPtrLoaded(TileType));
}


void AHexGrid::ReplaceTile(FHexGridCoordinate const& TileCoordinate, const int NewTileTypeIndex)
{
	const int OldTileTypeIndex = _TileMap.GetTileTypeIndexFromCoordinate(TileCoordinate);
	const bool bHasTile = HasTile(TileCoordinate);
	const bool bIsValidTileType = IsValidTileTypeIndex(NewTileTypeIndex);

	if (bHasTile)
	{
		RemoveTile_Internal(TileCoordinate);
	}

	if (bIsValidTileType)
	{
		AddTile_Internal(FHexGridTile(TileCoordinate, NewTileTypeIndex));
	}

	HandleTileTypeChanged(TileCoordinate, OldTileTypeIndex, NewTileTypeIndex);


	return;
}


void AHexGrid::ReplaceTile(FHexGridCoordinate const& TileCoordinate, const UHexGridTileTypeAsset* NewTileType)
{
	if (UMCommon::IsUObjectPointerSafe(NewTileType) &&
		HasRegisteredTileType(NewTileType) == false)
	{
		RegisterTileType(NewTileType);
	}

	const int NewTileTypeIndex = GetRegisteredTileTypeIndex(NewTileType);

	ReplaceTile(TileCoordinate, NewTileTypeIndex);


	return;
}


void AHexGrid::ReplaceTile(FHexGridCoordinate const& TileCoordinate, TSoftObjectPtr<const UHexGridTileTypeAsset> const& NewTileType)
{
	ReplaceTile(TileCoordinate, NewTileType.IsNull() ? nullptr : UMCommon::EnsureSoftObjectPtrLoaded(NewTileType));


	return;
}