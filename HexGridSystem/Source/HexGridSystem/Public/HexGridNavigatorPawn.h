// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HexGridTypes.h"
#include "HexGridNavigatorPawn.generated.h"


class AHexGrid;
class UHexGridNavigationComponent;
class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;


/*
* a pawn class that has a UHexGridNavigationComponent for navigating a hex grid.
* 
* additionally contains collision, spring arm, and camera components
*/
UCLASS()
class HEXGRIDSYSTEM_API AHexGridNavigatorPawn : public APawn
{
	GENERATED_BODY()

public:

	AHexGridNavigatorPawn();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// component for navigating on hex grids
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UHexGridNavigationComponent* GridNavigationComponent;

	// The CapsuleComponent being used for movement collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<UCapsuleComponent> CapsuleComponent;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;

	// normal camera rotation
	static const FRotator c_DefaultCameraRotation;

	// normal camera boom offset
	static const FVector c_DefaultCameraBoomOffset;

	// scalar applied to default camera arm length
	static const float c_DefaultCameraSpringArmLength;

};
