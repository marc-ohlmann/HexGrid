// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ExampleHexGrid.h"
#include "HexGridGameModeBase.generated.h"


class AExampleHexGrid;
class AHexGridNavigatorPawn;
class UHexGridMinimapWidget;


/**
 * 
 */
UCLASS()
class HEXGRIDSYSTEM_API AHexGridGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AHexGridGameModeBase();

	EGraphAStarResult FindPath(FHexGridCoordinate const& StartLocation, FHexGridCoordinate const& EndLocation, FHexGridCoordinateList& OutPath) const;
	EGraphAStarResult FindNodesInTraversalRange(FHexGridCoordinate const& StartLocation, double MaxRange, FHexGridCoordinateList& OutNodes) const;

	AHexGrid* GetGrid() const { return _HexGrid; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AExampleHexGrid> HexGridClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AHexGridNavigatorPawn> HexGridPawnClass;


	UPROPERTY(EditAnywhere)
	float CameraTransitionDuration;

	UPROPERTY(EditAnywhere)
	EHexGridDirection StartDirection;

	UPROPERTY(EditAnywhere)
	FHexGridSetupProperties GridSetupProperties;

	UPROPERTY(EditAnywhere)
	UHexGridDataAsset* GridDataAsset;

private:

	void HandleGenerateGridFinished();
	void HandleFogOfWarInitialized();
	void HandleCameraTransitionFinished();

	UPROPERTY()
	FExampleHexGridPathfindingProperties _ExampleGridData;

	UPROPERTY(Transient)
	AExampleHexGrid* _HexGrid;
	
};
