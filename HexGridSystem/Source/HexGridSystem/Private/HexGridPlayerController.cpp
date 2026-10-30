// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexGridPlayerController.h"

#include "HexGridSystem.h"
#include "HexGrid.h"
#include "HexGridNavigatorPawn.h"
#include "HexGridNavigationComponent.h"
#include "HexGridGameModeBase.h"
#include "MCommon.h"
#include "HexGridFogOfWarComponent.h"
#include "HexGridMinimapWidget.h"
#include "HexGridSubsystem.h"
#include "HexGridFogOfWarComponent.h"
#include "ExampleHexGridWidget.h"

#include "Engine/HitResult.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/InputComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

#include "DrawDebugHelpers.h"


const FName AHexGridPlayerController::c_SelectedTileGroupName = FName("PlayerSelectedTiles");

AHexGridPlayerController::AHexGridPlayerController()
{
	bShowMouseCursor = true;
	_HighlightAngle = 0.f;
	MoveDuration = 0.5f;
	bShouldTurnToFaceStepsAlongPath = false;
	_TargetGridCoordinate = FHexGridCoordinate();
	_CursorTargetCoordinates = FHexGridCoordinateList();
	ExampleWidgetClass = nullptr;
	_ExampleWidget = nullptr;

}


void AHexGridPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FHexGridCoordinate::GetCoordinatesInTriangleAroundCoordinate(FHexGridCoordinate(), 5, EHexGridShape::triangle_s_pos, _CursorTargetCoordinates);
	FHexGridCoordinate::GetCoordinatesInTriangleAroundCoordinate(FHexGridCoordinate(), 5, EHexGridShape::triangle_r_neg, _CursorTargetCoordinates);


	return;
}


void AHexGridPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	IF_NULL_RETURN_LERR(InputComponent, HexGridTest, "player input component is null");

	// bind ui navigation physical inputs
	FInputActionBinding& InputBinding_MoveLeft = InputComponent->BindAction("MoveLeft", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_MoveLeft);
	FInputActionBinding& InputBinding_MoveRight = InputComponent->BindAction("MoveRight", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_MoveRight);
	FInputActionBinding& InputBinding_MoveUp = InputComponent->BindAction("MoveUp", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_MoveUp);
	FInputActionBinding& InputBinding_MoveDown = InputComponent->BindAction("MoveDown", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_MoveDown);
	FInputActionBinding& InputBinding_TurnRight = InputComponent->BindAction("TurnRight", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_TurnRight);
	FInputActionBinding& InputBinding_TurnLeft = InputComponent->BindAction("TurnLeft", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_TurnLeft);
	FInputActionBinding& InputBinding_Click = InputComponent->BindAction("Click", IE_Pressed, this, &AHexGridPlayerController::HandlePhysicalInput_Click);


	return;
}



void AHexGridPlayerController::HandlePhysicalInput_MoveLeft()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	const FHexGridTransform CurrentTransform = NavComp->GetGridTransform();
	FHexGridCoordinate MoveCoordinate;
	if (HexGrid->GetFirstRelativeNeighborTile_Left(CurrentTransform.Coordinate, CurrentTransform.FacingDirection, MoveCoordinate))
	{
		
		if (HexGrid->HasTile(MoveCoordinate))
		{
			NavComp->MoveLeftOverDuration(MoveDuration);
		}
	}


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_MoveRight()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	const FHexGridTransform CurrentTransform = NavComp->GetGridTransform();
	FHexGridCoordinate MoveCoordinate;
	if (HexGrid->GetFirstRelativeNeighborTile_Right(CurrentTransform.Coordinate, CurrentTransform.FacingDirection, MoveCoordinate))
	{
		if (HexGrid->HasTile(MoveCoordinate))
		{
			NavComp->MoveRightOverDuration(MoveDuration);
		}
	}


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_MoveUp()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	const FHexGridTransform CurrentTransform = NavComp->GetGridTransform();
	FHexGridCoordinate MoveCoordinate;
	if (HexGrid->GetFirstRelativeNeighborTile_Up(CurrentTransform.Coordinate, CurrentTransform.FacingDirection, MoveCoordinate))
	{
		if (HexGrid->HasTile(MoveCoordinate))
		{
			NavComp->MoveUpOverDuration(MoveDuration);
		}
	}


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_MoveDown()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	const FHexGridTransform CurrentTransform = NavComp->GetGridTransform();
	FHexGridCoordinate MoveCoordinate;
	if (HexGrid->GetFirstRelativeNeighborTile_Down(CurrentTransform.Coordinate, CurrentTransform.FacingDirection, MoveCoordinate))
	{
		if (HexGrid->HasTile(MoveCoordinate))
		{
			NavComp->MoveDownOverDuration(MoveDuration);
		}
	}


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_TurnLeft()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");
	
	NavComp->TurnLeftOverDuration(MoveDuration);

	IF_NULL_RETURN_LERR(_ExampleWidget, HexGridTest, "_ExampleWidget is null");

	UHexGridMinimapWidget* Minimap = _ExampleWidget->GetMinimap();
	IF_NULL_RETURN_LERR(Minimap, HexGridTest, "Minimap is null");

	Minimap->PanRotateCounterClockwise();


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_TurnRight()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	NavComp->TurnRightOverDuration(MoveDuration);

	IF_NULL_RETURN_LERR(_ExampleWidget, HexGridTest, "_ExampleWidget is null");

	UHexGridMinimapWidget* Minimap = _ExampleWidget->GetMinimap();
	IF_NULL_RETURN_LERR(Minimap, HexGridTest, "Minimap is null");

	Minimap->PanRotateClockwise();


	return;
}


void AHexGridPlayerController::HandlePhysicalInput_Click()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	UWorld* World = GetWorld();
	IF_NULL_RETURN_LERR(World, HexGridTest, "could not get World");
	AHexGridGameModeBase* HexGameMode = UMCommon::GetGameMode<AHexGridGameModeBase>(World);
	IF_NULL_RETURN_LERR(HexGameMode, HexGridTest, "could not get HexGameMode");

	UHexGridFogOfWarComponent* FogOfWar = HexGrid->GetFogOfWarComponent();
#if false
	if (UMCommon::IsUObjectPointerSafe(FogOfWar))
	{
		
		FHitResult HitResult;
		const bool TraceResult = GetHitResultUnderCursor(UHexGridTypes::c_HexTileTraceChannel, false, HitResult);
		if (TraceResult)
		{
			FogOfWar->ApplyRevealBrushAtWorldLocation(HitResult.Location, 250.f);
		}
	}
#endif

	const FHexGridCoordinate PawnGridLocation = NavComp->GetGridCoordinate();

	FHexGridCoordinateList TargetNodes;

#if true

	ClearTargetTiles();

	FOR_FORWARD(i, _CursorTargetCoordinates.Num())
	{
		_CursorTargetCoordinates[i] = _CursorTargetCoordinates[i].RotateCounterClockwise();
	}

	SetTargetTiles(_CursorTargetCoordinates);

	// update highlight material parameter on targeted tiles
	HexGrid->SetTileGroupHighlightData(c_SelectedTileGroupName, FLinearColor::White, 1.f, 1.f);

	if (UMCommon::IsUObjectPointerSafe(FogOfWar))
	{
		FHexGridCoordinateList PartialShapeCoordinates;
		FHexGridCoordinateList PartialCoordinates;
		FHexGridCoordinateList VisibleCoordinates;
		_TargetGridCoordinate.GetSurroundingCoordinatesInShape(EHexGridShape::hexagon, 4, VisibleCoordinates);
		_TargetGridCoordinate.GetSurroundingCoordinatesInShape(EHexGridShape::hexagon, 7, PartialShapeCoordinates);
		PartialShapeCoordinates.Difference(VisibleCoordinates, PartialShapeCoordinates, PartialCoordinates);
		
		FHexGridTileVisibilityGroupMap TileVisibilityGroups;
		TileVisibilityGroups.VisibilityGroupMap.Add(EHexGridTileVisibility::partial, PartialCoordinates);
		TileVisibilityGroups.VisibilityGroupMap.Add(EHexGridTileVisibility::visible, VisibleCoordinates);

		FogOfWar->ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(TileVisibilityGroups);
	}

#endif
	
#if false
	EGraphAStarResult PathSearchResult = HexGameMode->FindPath(PawnGridLocation, _TargetGridCoordinate, TargetNodes);

	if (PathSearchResult == EGraphAStarResult::SearchSuccess)
	{
		FHexGridCoordinateList Path;
		FOR_FORWARD(i, TargetNodes.Num())
		{
			Path.Add(TargetNodes[i]);
			DrawDebugPoint(GetWorld(), HexGrid->GetWorldLocationFromHexGridCoordinate(Path[i]) + (FVector::UpVector * 10.f), 10.f, FColor::Blue, true, 10.f, 10);
		}

		if (UMCommon::IsUObjectPointerSafe(FogOfWar))
		{
			FogOfWar->ApplyRevealBrushAtCoordinates(Path);
		}

		NavComp->StartTraversingPath(Path, MoveDuration, bShouldTurnToFaceStepsAlongPath);



	}
	else
	{
		L_WARN(HexGridTest, "failed to find path, target coordinate: '{0}', result: '{1}'", _TargetGridCoordinate, UHexGridTypes::GraphAStarResultToString(PathSearchResult));
	}
#endif

#if false
	EGraphAStarResult PathSearchResult = HexGameMode->FindNodesInTraversalRange(_TargetGridCoordinate, 3.0, TargetNodes);

	if (PathSearchResult == EGraphAStarResult::SearchSuccess)
	{
		TArray<AActor*> TargetTiles;
		FOR_FORWARD(i, TargetNodes.Num())
		{
			AActor* TargetTile = HexGrid->GetTile<AActor>(TargetNodes[i]);
			if (UMCommon::IsUObjectPointerSafe(TargetTile))
			{
				TargetTiles.Add(TargetTile);
			}
		}
		SetTargetTiles(TargetTiles);
	}
	else
	{
		L_WARN(HexGridTest, "failed to find path, result: '{0}'", UHexGridTypes::GraphAStarResultToString(PathSearchResult));
	}

#endif

#if false

	FHexGridCoordinateList Coordinates;
	_TargetGridCoordinate.GetSurroundingCoordinatesInShape(EHexGridShape::rectangle, 1, Coordinates);

	TArray<AActor*> TargetTiles;
	FOR_FORWARD(i, Coordinates.Num())
	{
		AActor* TargetTile = HexGrid->GetTile<AActor>(Coordinates[i]);
		if (UMCommon::IsUObjectPointerSafe(TargetTile))
		{
			TargetTiles.Add(TargetTile);
		}
	}
	SetTargetTiles(TargetTiles);

#endif


	return;
}


void AHexGridPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World))
	{

		AHexGridGameModeBase* HexGameMode = UMCommon::GetGameMode<AHexGridGameModeBase>(World);
		if (UMCommon::IsUObjectPointerSafe(HexGameMode))
		{

			AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
			if (UMCommon::IsUObjectPointerSafe(HexPawn))
			{

				UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
				AHexGrid* HexGrid = HexGameMode->GetGrid();
				if (UMCommon::IsUObjectPointerSafe(HexGrid) &&
					UMCommon::IsUObjectPointerSafe(NavComp))
				{
					FHitResult HitResult;
					const bool TraceResult = GetHitResultUnderCursor(UHexGridTypes::c_HexTileTraceChannel, false, HitResult);
					if (TraceResult)
					{
						UInstancedStaticMeshComponent* HitIsmComponent = Cast<UInstancedStaticMeshComponent>(HitResult.GetComponent());
						if (UMCommon::IsUObjectPointerSafe(HitIsmComponent))
						{
							_TargetGridCoordinate = HexGrid->GetHexGridCoordinateAtWorldLocation(HitResult.Location);
#if false

							const FHexGridCoordinate PawnCoordinate = NavComp->GetGridCoordinate();
							FHexGridCoordinateList Path;
							EGraphAStarResult PathResult = HexGameMode->FindPath(PawnCoordinate, _TargetGridCoordinate, Path);

							/*L_WARN(HexGridTest, "PathResult: '{0}', StartCoordinate: '{1}', EndCoordinate: '{2}', PathSize: '{3}'",
								UHexGridTypes::GraphAStarResultToString(PathResult),
								PawnCoordinate,
								_TargetGridCoordinate,
								Path.Num()
							);*/

							if (PathResult == EGraphAStarResult::SearchSuccess)
							{
								SetTargetTiles(Path);
							}
							else
							{
								ClearTargetTiles();
							}

#endif

						}
						else
						{
							//L_WARN(HexGridTest, "no hit ismc");

							//ClearTargetTiles();

						}
					}
					else
					{
						//L_WARN(HexGridTest, "no trace result");

						//ClearTargetTiles();

					}
				}
				else
				{
					//L_WARN(HexGridTest, "no grid or nav comp");

					//ClearTargetTiles();

				}
			}
			else
			{
				//L_WARN(HexGridTest, "invalid pawn");

				//ClearTargetTiles();

			}
		}
		else
		{
			//L_WARN(HexGridTest, "no hex game mode");

			//ClearTargetTiles();

		}
	}
	else
	{
		//L_WARN(HexGridTest, "could not get world");

		//ClearTargetTiles();

	}


	return;
}


void AHexGridPlayerController::SetTargetTiles(FHexGridCoordinateList const& NewTargetTiles)
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	// reset material highlight on last targeted tiles
	HexGrid->SetTileGroupHighlightData(c_SelectedTileGroupName, FLinearColor::White, 0.f, 0.f);

	HexGrid->SetTileGroup(c_SelectedTileGroupName, NewTargetTiles);

	L_DBG(HexGridTest, "{0}", NewTargetTiles.Num());


	return;
}

void AHexGridPlayerController::ClearTargetTiles()
{ 
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN(HexPawn);

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN(NavComp);

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN(HexGrid);

	SetTargetTiles(FHexGridCoordinateList()); 


	return;
}


void AHexGridPlayerController::TestNearestDirectionFunction(FHexGridCoordinate const& TargetCoordinate)
{
	AHexGridNavigatorPawn* HexNavPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexNavPawn, HexGridTest, "could not get hex grid nav comp");

	UHexGridNavigationComponent* NavComp = HexNavPawn->GetComponentByClass<UHexGridNavigationComponent>();
	if (UMCommon::IsUObjectPointerSafe(NavComp))
	{
		const FHexGridTransform CurrentTransform = NavComp->GetGridTransform();
		const EHexGridDirection NearestDirection = FHexGridCoordinate::GetNearestDirectionBetweenCoordinates(CurrentTransform.Coordinate, TargetCoordinate);
		L_WARN(HexGridTest, "current direction: '{0}', nearest direction: {1}", CurrentTransform.FacingDirection, NearestDirection);
	}


	return;
}


void AHexGridPlayerController::SetupMinimap()
{
	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	UHexGridFogOfWarComponent* HexGridFog = HexGrid->GetFogOfWarComponent();
	IF_NULL_RETURN_LERR(HexGridFog, HexGridTest, "could not get HexGridFog");

	IF_NULL_RETURN_LERR(ExampleWidgetClass, HexGridTest, "MinimapWidgetClass is null");

	UExampleHexGridWidget* UiWidget = CreateWidget<UExampleHexGridWidget>(
		this,
		ExampleWidgetClass
	);
	if (UMCommon::IsUObjectPointerSafe(UiWidget) == false)
	{
		L_ERR(HexGridTest, "could not create player ui class '{0}'", ExampleWidgetClass);
		return;
	}

	UHexGridMinimapWidget* Minimap = UiWidget->GetMinimap();
	IF_NULL_RETURN_LERR(Minimap, HexGridTest, "Minimap is null");

	// apply widget to player hud
	UiWidget->AddToPlayerScreen();
	UiWidget->SetVisibility(ESlateVisibility::Visible);

	Minimap->SetRenderedGridDataReferences(
		&HexGrid->GetTileMapReference(),
		&HexGridFog->GetPlayerPerspectiveDataReference(0)
	);

	Minimap->OnMinimapTileClicked.AddUObject(this, &AHexGridPlayerController::HandleMinimapClicked);

	_ExampleWidget = UiWidget;

	TArray<UObject*> MinimapAssetObjects;
	IAssetProviderInterface::SyncLoadAssetProviderRecursively(Minimap, MinimapAssetObjects);

	TArray<UObject*> MinimapIndicatorAssetObjects;
	IAssetProviderInterface::SyncLoadAssetRecursively(MinimapIndicatorProperties.Texture, MinimapIndicatorAssetObjects);

	LoadedAssetObjects.Append(MinimapAssetObjects);
	LoadedAssetObjects.Append(MinimapIndicatorAssetObjects);


	Minimap->SetDynamicIndicator(FName("TestIndicator"), MinimapIndicatorLocation, MinimapIndicatorProperties);


	return;
}


void AHexGridPlayerController::HandleMinimapClicked(FHexGridCoordinate const& ClickedCoordinate)
{
	IF_NULL_RETURN_LERR(_ExampleWidget, HexGridTest, "_ExampleWidget is null");

	UHexGridMinimapWidget* Minimap = _ExampleWidget->GetMinimap();
	IF_NULL_RETURN_LERR(Minimap, HexGridTest, "Minimap is null");

	Minimap->PanToViewCenter(ClickedCoordinate);

	AHexGridNavigatorPawn* HexPawn = GetPawn<AHexGridNavigatorPawn>();
	IF_NULL_RETURN_LERR(HexPawn, HexGridTest, "could not get hex grid nav pawn");

	UHexGridNavigationComponent* NavComp = HexPawn->GetComponentByClass<UHexGridNavigationComponent>();
	IF_NULL_RETURN_LERR(NavComp, HexGridTest, "could not get hex grid nav comp");

	AHexGrid* HexGrid = NavComp->GetGrid();
	IF_NULL_RETURN_LERR(HexGrid, HexGridTest, "could not get HexGrid");

	if (HexGrid->HasTile(ClickedCoordinate) == false)
	{
		return;
	}

	NavComp->SetGridCoordinateOverDuration(ClickedCoordinate, 2.f);

	L_DBG(HexGridTest, "{0}", ClickedCoordinate);


	return;
}