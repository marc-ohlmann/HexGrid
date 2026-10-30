// Copyright Epic Games, Inc. All Rights Reserved.


#include "HexGridGameModeBase.h"

#include "HexGridSystem.h"
#include "MCommon.h"
#include "HexGridTypes.h"
#include "HexGridPlayerController.h"
#include "HexGridNavigatorPawn.h"
#include "HexGridSubsystem.h"
#include "CameraPawn.h"
#include "HexGridNavigationComponent.h"
#include "ExampleHexGrid.h"

#include "DrawDebugHelpers.h"


AHexGridGameModeBase::AHexGridGameModeBase()
{
	PlayerControllerClass = AHexGridPlayerController::StaticClass();
	DefaultPawnClass = ACameraPawn::StaticClass();

	HexGridClass = AExampleHexGrid::StaticClass();
	_HexGrid = nullptr;
	StartDirection = EHexGridDirection::north;
	HexGridPawnClass = AHexGridNavigatorPawn::StaticClass();
	CameraTransitionDuration = 2.f;
	_ExampleGridData = FExampleHexGridPathfindingProperties();
	GridSetupProperties = FHexGridSetupProperties();
}


void AHexGridGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	L_WARN(HexgridTest, "");

	UWorld* World = GetWorld();
	IF_NULL_RETURN_LERR(World, HexGridTest, "could not get world");

	_HexGrid = UHexGridSubsystem::GetHexGrid(this);
	if (UMCommon::IsUObjectPointerSafe(_HexGrid) == false)
	{
		L_WARN(HexGridTest, "no grid in world, spawning one now...");

		UClass* GridClass = HexGridClass.LoadSynchronous();
		IF_NULL_RETURN_LERR(GridClass, HexGridTest, "could not get world");

		FActorSpawnParameters ActorSpawnParams;
		AExampleHexGrid* NewHexGrid = World->SpawnActor<AExampleHexGrid>(GridClass, FTransform(FVector::ZeroVector), ActorSpawnParams);
		IF_NULL_RETURN_LERR(NewHexGrid, HexGridTest, "failed to spawn hex grid generator of class '{0}'", GridClass);

		_HexGrid = NewHexGrid;
	}
	
	if (_HexGrid->IsEmptyGrid())
	{
		if (GridDataAsset)
		{
			_HexGrid->GenerateHexGrid(GridDataAsset);
		}
		else
		{
			_HexGrid->GenerateHexGrid(GridSetupProperties);
		}
		_HexGrid->OnHexGridGenerateFinished.AddUObject(this, &AHexGridGameModeBase::HandleGenerateGridFinished);
		_HexGrid->OnFogOfWarInitialized.AddUObject(this, &AHexGridGameModeBase::HandleFogOfWarInitialized);
	}
	else
	{
		HandleGenerateGridFinished();
	}


	return;
}


EGraphAStarResult AHexGridGameModeBase::FindPath(FHexGridCoordinate const& StartLocation, FHexGridCoordinate const& EndLocation, FHexGridCoordinateList& OutPath) const
{
	IF_NULL_RETURN_VAL_LERR(_HexGrid, EGraphAStarResult::SearchFail, HexGridTest, "could not get hex grid");

	EGraphAStarResult Result = _ExampleGridData.FindPath<false>(StartLocation, EndLocation, OutPath);

#if false
	L_WARN(HexGridTest, "StartLocation: {0}, EndLocation: {1}, Result: '{2}', PathTraversalCost: '{3}', PathHeuristicCost: '{4}'", 
		StartLocation,
		EndLocation,
		UHexGridTypes::GraphAStarResultToString(Result),
		_ExampleGridData.GetPathTraversalCost(OutPath), 
		_ExampleGridData.GetPathHeuristicCost(OutPath)
	);
#endif

	return Result;
}


EGraphAStarResult AHexGridGameModeBase::FindNodesInTraversalRange(FHexGridCoordinate const& StartLocation, double MaxRange, FHexGridCoordinateList& OutNodes) const
{
	IF_NULL_RETURN_VAL_LERR(_HexGrid, EGraphAStarResult::SearchFail, HexGridTest, "could not get hex grid");


	EGraphAStarResult Result = _ExampleGridData.FindNodesWithinTileDistanceAndWithMaxTraversalCost(StartLocation, FMath::CeilToInt(MaxRange), MaxRange, OutNodes);


	return Result;
}


void AHexGridGameModeBase::HandleGenerateGridFinished()
{
	IF_NULL_RETURN_LERR(_HexGrid, HexGridTest, "could not get HexGrid");

	UWorld* World = GetWorld();
	IF_NULL_RETURN_LERR(World, HexGridTest, "could not get world");

	ACameraPawn* PlayerCameraPawn = UMCommon::GetLocalPlayerPawn<ACameraPawn>(this);
	IF_NULL_RETURN_LERR(PlayerCameraPawn, HexGridTest, "could not get player pawn");

	UClass* HexPawnClass = HexGridPawnClass.LoadSynchronous();
	IF_NULL_RETURN_LERR(HexPawnClass, HexGridTest, "could not get hex grid pawn class");

	AHexGridNavigatorPawn* HexGridPawn = World->SpawnActor<AHexGridNavigatorPawn>(HexPawnClass);
	IF_NULL_RETURN_LERR(HexPawnClass, HexGridTest, "could not spawn hex grid pawn");

	UHexGridNavigationComponent* NavComp = HexGridPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	NavComp->SetGrid(_HexGrid);
	NavComp->SetGridTransform(FHexGridTransform(FHexGridCoordinate::c_Zero, StartDirection), true);

	PlayerCameraPawn->OnCameraPawnTransitionFinished.AddUObject(this, &AHexGridGameModeBase::HandleCameraTransitionFinished);
	PlayerCameraPawn->TransitionPlayerControllerToPawn(HexGridPawn, CameraTransitionDuration);

	FHexGridCoordinateList TileCoordinates;
	_HexGrid->GetTileCoordinates(TileCoordinates);

	L_DBG(HexGridText, "{0} coordinates in grid", TileCoordinates.Num());

	FOR_FORWARD(i, TileCoordinates.Num())
	{
		const FExampleHexGridTileData TileData = _HexGrid->GetExampleTileData(TileCoordinates[i]);
		_ExampleGridData.Items.Add(TileData);
	}

#if false
	FOR_FORWARD(i, 12)
	{
		const EHexGridDirection Clockwise = FHexGridCoordinate::GetDirectionAfterClockwiseRotations(EHexGridDirection::north, i);
		const EHexGridDirection CounterClockwise = FHexGridCoordinate::GetDirectionAfterClockwiseRotations(EHexGridDirection::north, -i);

		L_WARN(HexGridTest, "[Start@North] {0} rotations: {CW: '{1}', CCW: '{2}'}",
			i,
			Clockwise,
			CounterClockwise
		);
	}

	FOR_FORWARD(i, 12)
	{
		const EHexGridDirection Clockwise = FHexGridCoordinate::GetDirectionAfterClockwiseRotations(EHexGridDirection::south, i);
		const EHexGridDirection CounterClockwise = FHexGridCoordinate::GetDirectionAfterClockwiseRotations(EHexGridDirection::south, -i);

		L_WARN(HexGridTest, "[Start@South] {0} rotations: {CW: '{1}', CCW: '{2}'}",
			i,
			Clockwise,
			CounterClockwise
		);
	}
#endif

#if false

	FOR_FORWARD(i, FHexGridCoordinate::c_HexNumberOfSides)
	{
		L_WARN(HexGridTest, "{0}: {Length: '{1}'}",
			FHexGridCoordinate::c_Neighbors[i],
			FHexGridCoordinate::c_Neighbors[i].Length()
		);
	}

	FOR_FORWARD(i, FHexGridCoordinate::c_HexNumberOfSides)
	{
		L_WARN(HexGridTest, "(Scaled by {0}) {1}: {Length: '{2}'}",
			i,
			(FHexGridCoordinate::c_Neighbors[i] * i),
			(FHexGridCoordinate::c_Neighbors[i] * i).Length()
		);
	}

#endif

#if false

	FExampleHexGridPathfindingProperties Grid = FExampleHexGridPathfindingProperties(FHexGridCoordinateList::GetCoordinatesInHexagon(10).List);

	const FHexGridCoordinate PathStartLocation = FHexGridCoordinate::c_Zero;
	const FHexGridCoordinate PathEndLocation = FHexGridCoordinate(3, -9);

	L_WARN(HexGridTest, "GridSize: {0}", Grid.Items.Num());

	FHexGridCoordinateList Path;
	const EGraphAStarResult Result = Grid.FindPath(PathStartLocation, PathEndLocation, Path);

	L_WARN(HexGridTest, "Result: '{0}', PathLength: {1}", UHexGridTypes::GraphAStarResultToString(Result), Path.Num());
	FOR_FORWARD(i, Path.Num())
	{
		L_WARN(HexGridTest, "\t{0}", Path[i]);
	}
#endif

#if false

	FOR_FORWARD(i, UMCommon::GetEnumSize<EHexGridDirection>())
	{
		EHexGridDirection Direction = static_cast<EHexGridDirection>(i);
		L_WARN(HexGridTest, "{0} : {1}", Direction, FHexGridCoordinate::GetAbsoluteRotationFromDirection(Direction));
	}

#endif

#if false

	FOR_FORWARD(tests_iterator, 10)
	{
		const int Distance = tests_iterator + 1;
		const FHexGridCoordinate TestNode = FHexGridCoordinate(UMathTypes::GetRandomInt(-24, 24), UMathTypes::GetRandomInt(-24, 24));
		const FHexGridCoordinateList DistanceCoordinates = FHexGridCoordinate::GetCoordinatesAtDistanceFromCoordinate(TestNode, Distance);

		L_WARN(HexGridTest, "found {0} coordinates at distance {1} from test node '{2}'", DistanceCoordinates.Num(), Distance, TestNode);

		FOR_FORWARD(i, DistanceCoordinates.Num())
		{
			if (FHexGridCoordinate::Distance(DistanceCoordinates[i], TestNode) == Distance)
			{
				L_WARN(HexGridTest, "coordinate '{0}' distance: '{1}'", DistanceCoordinates[i], FHexGridCoordinate::Distance(DistanceCoordinates[i], TestNode));
			}
			else
			{
				L_WARN(HexGridTest, "invalid distance '{0}' for coordinate '{1}'", FHexGridCoordinate::Distance(DistanceCoordinates[i], TestNode), DistanceCoordinates[i]);
			}
		}
	}


#endif

#if false


	const FVector TileWorldLocation1 = _HexGrid->GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate(1, 0));
	const FVector TileWorldLocation2 = _HexGrid->GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate(1, -1));
	const FVector TileWorldLocation3 = _HexGrid->GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate(-4, 2));
	const FVector TileWorldLocation4 = _HexGrid->GetWorldLocationFromHexGridCoordinate(FHexGridCoordinate(4, 2));

	DrawDebugPoint(GetWorld(), TileWorldLocation1 + (FVector::UpVector * 10.f), 10.f, FColor::Blue, true, 10.f, 10);
	DrawDebugPoint(GetWorld(), TileWorldLocation2 + (FVector::UpVector * 10.f), 10.f, FColor::Red, true, 10.f, 10);
	DrawDebugPoint(GetWorld(), TileWorldLocation3 + (FVector::UpVector * 10.f), 10.f, FColor::Purple, true, 10.f, 10);
	DrawDebugPoint(GetWorld(), TileWorldLocation4 + (FVector::UpVector * 10.f), 10.f, FColor::Orange, true, 10.f, 10);

#endif

#if false

	FOR_FORWARD(i, UMCommon::GetEnumSize<EHexGridDirection>())
	{
		EHexGridDirection FromDirectionIterator = static_cast<EHexGridDirection>(i);
		const int Rotations_FromDirFromNorth = FHexGridCoordinate::GetMinimumRotationsFromNorthToDirection(FromDirectionIterator);
		FOR_FORWARD(j, UMCommon::GetEnumSize<EHexGridDirection>())
		{
			EHexGridDirection ToDirectionIterator = static_cast<EHexGridDirection>(j);
			const int Rotations_ToDirFromNorth = FHexGridCoordinate::GetMinimumRotationsFromNorthToDirection(ToDirectionIterator);
			const int Rotations = FHexGridCoordinate::GetMinimumRotationsBetweenDirections(FromDirectionIterator, ToDirectionIterator);

			L_WARN(HexGridTest, "'{0}' rotations from '{1}' ({2}) to '{3}' ({4})",
				Rotations,
				FromDirectionIterator,
				Rotations_FromDirFromNorth,
				ToDirectionIterator,
				Rotations_ToDirFromNorth
			);
		}
	}

#endif


	return;
}


void AHexGridGameModeBase::HandleFogOfWarInitialized()
{
	return;
}


void AHexGridGameModeBase::HandleCameraTransitionFinished()
{
	AHexGridPlayerController* HexPlayer = UMCommon::GetFirstLocalPlayerController<AHexGridPlayerController>(this);
	IF_NULL_RETURN_LERR(HexPlayer, HexGridTest, "could not get HexPlayer");

	HexPlayer->SetupMinimap();


	return;
}