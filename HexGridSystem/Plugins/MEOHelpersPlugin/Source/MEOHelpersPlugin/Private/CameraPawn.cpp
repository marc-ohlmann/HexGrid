// Copyright 2023 Marc Ohlmann, All Rights Reserved.


#include "CameraPawn.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"


ACameraPawn::ACameraPawn()
{
	bool bShouldEnableTick = true;
	PrimaryActorTick.bCanEverTick = bShouldEnableTick;
	PrimaryActorTick.bStartWithTickEnabled = bShouldEnableTick;
	bAllowTickBeforeBeginPlay = false;
	SetActorTickEnabled(bShouldEnableTick);

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	if (UMCommon::IsUObjectPointerSafe(CameraBoom) == true)
	{
		// Create a camera boom (pulls in towards the player if there is a collision)
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 0.f;
		CameraBoom->TargetOffset = FVector(0.f, 0.f, 0.f);
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);
		CameraBoom->bUsePawnControlRotation = false;
	}

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	if (UMCommon::IsUObjectPointerSafe(FollowCamera) == true)
	{
		// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		FollowCamera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		FollowCamera->bUsePawnControlRotation = false;
	}


	_TransitionTargetPawn = nullptr;
	_TransitionDurationSeconds = 0.f;
	_TransitionBuffer = 0.f;

}


void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();


	return;
}


void ACameraPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	return;
}


void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnTick_TransitionPlayerControllerToPawn(DeltaTime);


	return;
}


void ACameraPawn::TransitionPlayerControllerToPawn(APawn* TargetPawn, const float DurationSeconds)
{
	IF_NULL_RETURN_LERR(TargetPawn, MEOHelpers, "TargetPawn is null");

	APlayerController* OwnerPlayer = GetController<APlayerController>();
	IF_NULL_RETURN_LERR(TargetPawn, MEOHelpers, "OwnerPlayer is null");

	_TransitionTargetPawn = TargetPawn;
	_TransitionDurationSeconds = DurationSeconds;
	_TransitionBuffer = 0.f;


	return;
}



void ACameraPawn::FinishTransitionPlayerControllerToPawn()
{
	APawn* TargetPawn = UMCommon::SafelyDereferenceWeakObjectPtr(_TransitionTargetPawn);
	IF_NULL_RETURN_LERR(TargetPawn, MEOHelpers, "TargetPawn is null");

	APlayerController* OwnerPlayer = GetController<APlayerController>();
	IF_NULL_RETURN_LERR(TargetPawn, MEOHelpers, "OwnerPlayer is null");

	if (TransferPlayerPawn(OwnerPlayer, TargetPawn) == false)
	{
		L_ERR(MEOHelpers, "failed to transfer player '{0}' to pawn '{1}'", OwnerPlayer, TargetPawn);
		return;
	}

	_TransitionTargetPawn = nullptr;
	_TransitionDurationSeconds = 0.f;
	_TransitionBuffer = 0.f;

	if (OnCameraPawnTransitionFinished.IsBound())
	{
		OnCameraPawnTransitionFinished.Broadcast();
	}

	// cleanup this camera pawn
	Destroy();


	return;
}


void ACameraPawn::OnTick_TransitionPlayerControllerToPawn(float DeltaTime)
{
	APawn* TargetPawn = UMCommon::SafelyDereferenceWeakObjectPtr(_TransitionTargetPawn);
	IF_NULL_RETURN(TargetPawn);

	if (_TransitionDurationSeconds == 0.f ||
		FMath::IsNearlyZero(_TransitionDurationSeconds))
	{
		return;
	}

	_TransitionBuffer += DeltaTime;
	if (_TransitionBuffer < _TransitionDurationSeconds)
	{
		const FTransform CurrentTransform = GetActorTransform();
		const FTransform TargetTransform = GetTransitionTargetTransform();

		const float TransitionAlpha = _TransitionBuffer / _TransitionDurationSeconds;

		//const float TransitionFunc = FMath::Sqrt(TransitionAlpha); // sqrt(n)
		//const float TransitionFunc = TransitionAlpha; // n
		//const float TransitionFunc = TransitionAlpha * TransitionAlpha; // n^2
		const float TransitionFunc = TransitionAlpha * TransitionAlpha * TransitionAlpha * TransitionAlpha; // n^4

		const FQuat TransitionRotation = FMath::Lerp(CurrentTransform.Rotator().Quaternion(), TargetTransform.Rotator().Quaternion(), TransitionFunc);
		const FVector TransitionLocation = FMath::Lerp(CurrentTransform.GetLocation(), TargetTransform.GetLocation(), TransitionFunc);
		const FTransform TransitionTransform = FTransform(TransitionRotation, TransitionLocation, FVector::OneVector);

		SetActorTransform(TransitionTransform);
	}
	else
	{
		FinishTransitionPlayerControllerToPawn();
	}


	return;
}


FTransform ACameraPawn::GetCameraTransformFromActor(AActor* TargetActor)
{
	IF_NULL_RETURN_TYPE_LERR(TargetActor, FTransform, MEOHelpers, "TargetActor is null");

	// check if it has a camera component and use that if possible
	const UCameraComponent* TargetCameraComponent = TargetActor->GetComponentByClass<UCameraComponent>();
	if (UMCommon::IsUObjectPointerSafe(TargetCameraComponent))
	{
		return TargetCameraComponent->GetComponentTransform();
	}


	// just use the actor transform
	return TargetActor->GetActorTransform();
}


FTransform ACameraPawn::GetTransitionTargetTransform() const
{
	APawn* TargetPawn = UMCommon::SafelyDereferenceWeakObjectPtr(_TransitionTargetPawn);
	IF_NULL_RETURN_TYPE_LERR(TargetPawn, FTransform, MEOHelpers, "TargetPawn is null");

	// just use the pawn transform
	return GetCameraTransformFromActor(TargetPawn);
}


ACameraPawn* ACameraPawn::CreateCameraPawnAtActor(UClass* CameraPawnClass, AActor* TargetActor)
{
	IF_NULL_RETURN_NULL_LERR(CameraPawnClass, MEOHelpers, "CameraPawnClass is null");
	IF_NULL_RETURN_NULL_LERR(TargetActor, MEOHelpers, "TargetActor is null");

	UWorld* World = UMCommon::GetWorldFromContext(TargetActor);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "World is null");

	FActorSpawnParameters SpawnParams;

	const FTransform TargetTransform = GetCameraTransformFromActor(TargetActor);

	ACameraPawn* NewCameraPawn = World->SpawnActor<ACameraPawn>(CameraPawnClass, TargetTransform, SpawnParams);
	IF_NULL_RETURN_NULL_LERR(NewCameraPawn, MEOHelpers, "NewCameraPawn is null");


	return NewCameraPawn;
}


ACameraPawn* ACameraPawn::ReplacePlayerPawnWithCamera(UClass* CameraPawnClass, APawn* TargetPawn)
{
	IF_NULL_RETURN_NULL_LERR(CameraPawnClass, MEOHelpers, "CameraPawnClass is null");
	IF_NULL_RETURN_NULL_LERR(TargetPawn, MEOHelpers, "TargetPawn is null");

	ACameraPawn* NewCameraPawn = CreateCameraPawnAtActor(CameraPawnClass, TargetPawn);
	IF_NULL_RETURN_NULL_LERR(NewCameraPawn, MEOHelpers, "NewCameraPawn is null");

	if (NewCameraPawn->CapturePlayerControllerFromPawn(TargetPawn) == false)
	{
		NewCameraPawn->Destroy();
		return nullptr;
	}


	return NewCameraPawn;
}


bool ACameraPawn::CapturePlayerControllerFromPawn(APawn* TargetPawn)
{
	IF_NULL_RETURN_VAL_LERR(TargetPawn, false, MEOHelpers, "TargetPawn is null");

	APlayerController* TargetPlayerController = TargetPawn->GetController<APlayerController>();
	IF_NULL_RETURN_VAL_LERR(TargetPlayerController, false, MEOHelpers, "TargetPlayerController is null");


	return TransferPlayerPawn(TargetPlayerController, this);
}


bool ACameraPawn::TransferPlayerPawn(APlayerController* Player, APawn* NewPawn)
{
	IF_NULL_RETURN_VAL_LERR(Player, false, MEOHelpers, "Player is null");
	IF_NULL_RETURN_VAL_LERR(NewPawn, false, MEOHelpers, "NewPawn is null");

	// unpossess old pawn
	Player->UnPossess();

	// reset state ?
	//Player->ResetCameraMode();

	// possess new pawn
	Player->Possess(NewPawn);


	return true;
}