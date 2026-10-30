// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexGridNavigatorPawn.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "HexGrid.h"
#include "HexGridNavigationComponent.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"


const FVector AHexGridNavigatorPawn::c_DefaultCameraBoomOffset = FVector(0.f, 0.f, 500.0);
const float AHexGridNavigatorPawn::c_DefaultCameraSpringArmLength = 650.f;
const FRotator AHexGridNavigatorPawn::c_DefaultCameraRotation = FRotator(-40.f, 0.f, 0.f);


AHexGridNavigatorPawn::AHexGridNavigatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CapsuleComponent"));
	if (UMCommon::IsUObjectPointerSafe(CapsuleComponent) == true)
	{
		CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
		CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CapsuleComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		CapsuleComponent->SetShouldUpdatePhysicsVolume(false);
		CapsuleComponent->SetCanEverAffectNavigation(false);
		CapsuleComponent->bDynamicObstacle = false;
		RootComponent = CapsuleComponent;
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	if (UMCommon::IsUObjectPointerSafe(CameraBoom) == true)
	{
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = AHexGridNavigatorPawn::c_DefaultCameraSpringArmLength; // The camera follows at this distance behind the character	
		CameraBoom->TargetOffset = AHexGridNavigatorPawn::c_DefaultCameraBoomOffset; // The camera follows at this distance behind the character	
		CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	}

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	if (UMCommon::IsUObjectPointerSafe(FollowCamera) == true)
	{
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FollowCamera->SetRelativeRotation(AHexGridNavigatorPawn::c_DefaultCameraRotation); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	}

	// Create a grid navigation component
	GridNavigationComponent = CreateDefaultSubobject<UHexGridNavigationComponent>(FName("HexGridNavigation"));
	if (UMCommon::IsUObjectPointerSafe(GridNavigationComponent) == true)
	{

	}

}


void AHexGridNavigatorPawn::BeginPlay()
{
	Super::BeginPlay();


	return;
}


void AHexGridNavigatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	return;
}


void AHexGridNavigatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	return;
}