// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HexGridTypes.h"
#include "HexGridPlayerController.generated.h"


class UExampleHexGridWidget;


/**
 * 
 */
UCLASS()
class HEXGRIDSYSTEM_API AHexGridPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AHexGridPlayerController();

	void SetupMinimap();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;


	UPROPERTY(EditDefaultsOnly)
		float MoveDuration;


	UPROPERTY(EditDefaultsOnly)
		bool bShouldTurnToFaceStepsAlongPath;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UExampleHexGridWidget> ExampleWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	FMinimapIndicatorProperties MinimapIndicatorProperties;

	UPROPERTY(EditDefaultsOnly)
	FVector2D MinimapIndicatorLocation;

private:

	UPROPERTY()
	TArray<UObject*> LoadedAssetObjects;

	static const FName c_SelectedTileGroupName;

	UPROPERTY()
	FHexGridCoordinate _TargetGridCoordinate;

	UPROPERTY()
		float _HighlightAngle;
	
	void SetTargetTiles(FHexGridCoordinateList const& NewTargetTile);

	void ClearTargetTiles();

	UFUNCTION()
		void HandlePhysicalInput_MoveLeft();

	UFUNCTION()
		void HandlePhysicalInput_MoveRight();

	UFUNCTION()
		void HandlePhysicalInput_MoveUp();

	UFUNCTION()
		void HandlePhysicalInput_MoveDown();

	UFUNCTION()
		void HandlePhysicalInput_TurnLeft();

	UFUNCTION()
		void HandlePhysicalInput_TurnRight();

	UFUNCTION()
		void HandlePhysicalInput_Click();


	void TestNearestDirectionFunction(FHexGridCoordinate const& TargetCoordinate);

	FHexGridCoordinateList _CursorTargetCoordinates;

	void HandleMinimapClicked(FHexGridCoordinate const& ClickedCoordinate);

	UPROPERTY()
	UExampleHexGridWidget* _ExampleWidget;
};
