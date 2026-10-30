// Copyright 2023 Marc Ohlmann. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"


class UCameraComponent;
class USpringArmComponent;


DECLARE_MULTICAST_DELEGATE(FOnCameraPawnTransitionFinished);


/**
 * A simple pawn with a camera component
 */
UCLASS()
class MEOHELPERSPLUGIN_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:

	ACameraPawn();

	// create a camera pawn at the given actor's location
	// will use the actor's camera component location if it has one
	static ACameraPawn* CreateCameraPawnAtActor(UClass* CameraPawnClass, AActor* TargetActor);

	// create a camera pawn at the given actor's location
	// will use the actor's camera component location if it has one
	static ACameraPawn* CreateCameraPawnAtActor(AActor* TargetActor)
	{
		return CreateCameraPawnAtActor(ACameraPawn::StaticClass(), TargetActor);
	}

	// create a camera pawn at the given actor's location
	// will use the actor's camera component location if it has one
	template <typename TCameraPawnClass>
	static TCameraPawnClass* CreateCameraPawnAtActor(AActor* TargetActor)
	{
		return Cast<TCameraPawnClass>(CreateCameraPawnAtActor(TCameraPawnClass::StaticClass(), TargetActor));
	}

	// create a camera pawn at the given pawn's location and then capture it's owning player controller
	// will use the actor's camera component location if it has one
	static ACameraPawn* ReplacePlayerPawnWithCamera(UClass* CameraPawnClass, APawn* TargetPawn);

	// create a camera pawn at the given pawn's location and then capture it's owning player controller
	// will use the actor's camera component location if it has one
	static ACameraPawn* ReplacePlayerPawnWithCamera(APawn* TargetPawn)
	{
		return ReplacePlayerPawnWithCamera(ACameraPawn::StaticClass(), TargetPawn);
	}

	// create a camera pawn at the given actor's location
	// will use the actor's camera component location if it has one
	template <typename TCameraPawnClass>
	static TCameraPawnClass* ReplacePlayerPawnWithCamera(APawn* TargetPawn)
	{
		return Cast<TCameraPawnClass>(ReplacePlayerPawnWithCamera(TCameraPawnClass::StaticClass(), TargetPawn));
	}

	// make the player controller that is possessing the target pawn possess this camera pawn instead
	// returns false if no player controller is found
	bool CapturePlayerControllerFromPawn(APawn* TargetPawn);

	// move the owning player controller's possession ownership of this pawn to the target pawn
	void TransitionPlayerControllerToPawn(APawn* TargetPawn, const float DurationSeconds);

	// delegate broadcasts when the any transition finishes
	FOnCameraPawnTransitionFinished OnCameraPawnTransitionFinished;

	// transfer possession of a player to the new pawn
	// returns false if either are null
	static bool TransferPlayerPawn(APlayerController* Player, APawn* NewPawn);

protected:

	// called every frame
	virtual void Tick(float DeltaTime) override;

	// called on begin play
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Camera boom positioning the camera behind the pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;

	static FTransform GetCameraTransformFromActor(AActor* Actor);

private:

	// duration, in seconds, for the transition
	UPROPERTY()
	float _TransitionDurationSeconds;

	// buffer for the transition
	UPROPERTY()
	float _TransitionBuffer;

	// target pawn to transition the owning player into
	UPROPERTY()
	TWeakObjectPtr<APawn> _TransitionTargetPawn;

	// get the transition target's transform
	FTransform GetTransitionTargetTransform() const;

	// handle transitions toward a target pawn on every frame
	void OnTick_TransitionPlayerControllerToPawn(float DeltaTime);

	// called when the transition is finished
	void FinishTransitionPlayerControllerToPawn();


};