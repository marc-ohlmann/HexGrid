// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexGridNavigationComponent.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "HexGrid.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"


UHexGridNavigationComponent::UHexGridNavigationComponent()
	: OnGridTransformChanged()
	, OnGridTransformChangeOverDurationFinished()
	, _bIsChangingTransform(false)
	, _CurrentGridTransform()
	, _PreviousGridTransform()
	, _CurrentGrid(nullptr)
	, _ChangeTransformDuration(0.f)
	, _ChangeTransformDurationBuffer(0.f)
	, _GridLocationWorldOffset(FVector::ZeroVector)
	, OnPathTraversalStarted()
	, OnPathTraversalFinished()
	, _PathCoordinates()
	, _bShouldTurnToFaceEachStepAlongPath(false)
	, _PathTraversalIndex(-1)
	, _bShouldTurnAndMoveOnEachStep(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHexGridNavigationComponent::BeginPlay()
{
	Super::BeginPlay();


	return;
}


void UHexGridNavigationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OnTick_ChangeTransformOverDuration(DeltaTime);


	return;
}


void UHexGridNavigationComponent::SetGrid(AHexGrid* TargetGrid)
{
	_CurrentGrid = TargetGrid;


	return;
}


AHexGrid* UHexGridNavigationComponent::GetGrid() const
{
	return UMCommon::SafelyDereferenceWeakObjectPtr<AHexGrid>(_CurrentGrid);
}


void UHexGridNavigationComponent::SetGridTransform(FHexGridTransform const& TargetGridTransform, bool bSnapToGrid)
{
	_PreviousGridTransform = GetGridTransform();
	_CurrentGridTransform = TargetGridTransform;

	if (bSnapToGrid)
	{
		SnapToGridTransform();
	}

	if (OnGridTransformChanged.IsBound())
	{
		OnGridTransformChanged.Broadcast(_PreviousGridTransform, _CurrentGridTransform);
	}


	return;
}


void UHexGridNavigationComponent::SetGridCoordinate(FHexGridCoordinate const& TargetGridLocation, bool bSnapToGrid)
{
	SetGridTransform(FHexGridTransform(TargetGridLocation, GetGridFacingDirection()), bSnapToGrid);


	return;
}


FVector UHexGridNavigationComponent::GetGridCoordinateWorldLocation(bool bIncludeWorldOffset) const
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_VAL_LERR(Grid, FVector::ZeroVector, HexGrid, "grid is null");

	const FVector Offset = bIncludeWorldOffset ? GetGridLocationWorldOffset() : FVector::ZeroVector;


	return Grid->GetWorldLocationFromHexGridCoordinate(GetGridCoordinate()) + Offset;
}


FRotator UHexGridNavigationComponent::GetGridFacingDirectionWorldRotation() const
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_VAL_LERR(Grid, FRotator::ZeroRotator, HexGrid, "could not get grid");


	return Grid->GetRotatorFromHexGridDirection(GetGridFacingDirection());
}


void UHexGridNavigationComponent::SnapToGridLocation()
{
	SetOwnerLocation(GetGridCoordinateWorldLocation(true));


	return;
}


void UHexGridNavigationComponent::SnapToGridTransform()
{
	SnapToGridLocation();
	SnapToGridDirection();


	return;
}


void UHexGridNavigationComponent::SetGridLocationWorldOffset(FVector const& InOffset)
{
	_GridLocationWorldOffset = InOffset;


	return;
}


void UHexGridNavigationComponent::SetGridFacingDirection(const EHexGridDirection Direction, bool bSnapToGrid)
{
	SetGridTransform(FHexGridTransform(GetGridCoordinate(), Direction), bSnapToGrid);


	return;
}


void UHexGridNavigationComponent::SetOwnerLocation(const FVector& NewLocation)
{
	AActor* OwnerActor = GetOwner();
	IF_NULL_RETURN_LERR(OwnerActor, HexGrid, "OwnerActor is null");

	OwnerActor->SetActorLocation(NewLocation);


	return;
}


void UHexGridNavigationComponent::SetOwnerRotation(const FRotator NewRotation)
{
	AActor* OwnerActor = GetOwner();
	IF_NULL_RETURN_LERR(OwnerActor, HexGrid, "OwnerActor is null");
	
	bool bWasHandled = false;

	// possessed pawns may use a separate control rotation
	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (UMCommon::IsUObjectPointerSafe(OwnerPawn) &&
		OwnerPawn->bUseControllerRotationYaw)
	{
		AController* OwnerController = OwnerPawn->GetController();
		if (UMCommon::IsUObjectPointerSafe(OwnerController))
		{
			OwnerController->SetControlRotation(NewRotation);
			bWasHandled = true;
		}
	}

	// fallback to generic actor method
	if (bWasHandled == false)
	{
		OwnerActor->SetActorRotation(NewRotation);
	}



	return;
}


FVector UHexGridNavigationComponent::GetOwnerLocation() const
{
	AActor* OwnerActor = GetOwner();
	IF_NULL_RETURN_VAL_LERR(OwnerActor, FVector::ZeroVector, HexGrid, "could not get owning actor for component '{0}'", GetName());


	return OwnerActor->GetActorLocation();
}


FRotator UHexGridNavigationComponent::GetOwnerRotation() const
{
	AActor* OwnerActor = GetOwner();
	IF_NULL_RETURN_VAL_LERR(OwnerActor, FRotator::ZeroRotator, HexGrid, "could not get owning actor for component '{0}'", GetName());


	return OwnerActor->GetActorRotation();
}


void UHexGridNavigationComponent::SnapToGridDirection()
{
	const FRotator GridFacingRotator = GetGridFacingDirectionWorldRotation();

	SetOwnerRotation(GridFacingRotator);


	return;
}


void UHexGridNavigationComponent::StartChangingTransformOverDuration(const float Duration)
{
	_bIsChangingTransform = true;
	_ChangeTransformDuration = Duration;
	_ChangeTransformDurationBuffer = 0.f;


	return;
}


void UHexGridNavigationComponent::SetGridFacingDirectionOverDuration(const EHexGridDirection Direction, const float Duration)
{
	const FHexGridCoordinate CurrentGridCoordinate = GetGridCoordinate();
	SetGridTransformOverDuration(FHexGridTransform(CurrentGridCoordinate, Direction), Duration);


	return;
}


void UHexGridNavigationComponent::SetGridCoordinateOverDuration(FHexGridCoordinate const& TargetGridLocation, const float Duration)
{
	const EHexGridDirection CurrentGridDirection = GetGridFacingDirection();
	SetGridTransformOverDuration(FHexGridTransform(TargetGridLocation, CurrentGridDirection), Duration);


	return;
}


void UHexGridNavigationComponent::SetGridTransformOverDuration(FHexGridTransform const& TargetGridTransform, const float Duration)
{
	if (UMathTypes::IsRealNegativeOrNearlyZero(Duration))
	{
		L_WARN(HexGrid, "invalid duration -- snapping to location instead");
		SnapToGridTransform();
		return;
	}

	SetGridTransform(TargetGridTransform, false);
	StartChangingTransformOverDuration(Duration);


	return;
}



void UHexGridNavigationComponent::OnTick_ChangeTransformOverDuration(float DeltaTime)
{
	if (IsChangingTransform())
	{
		_ChangeTransformDurationBuffer += DeltaTime;
		float alpha = _ChangeTransformDurationBuffer / _ChangeTransformDuration;
		bool bFinishedChangingTransform = false;

		if (UMathTypes::IsRealGreaterThan(alpha, 1.f))
		{
			alpha = 1.f;
			bFinishedChangingTransform = true;
		}

		// location
		{
			const FVector CurrentWorldLocation = GetOwnerLocation();
			const FVector TargetWorldLocation = GetGridCoordinateWorldLocation(true);
			if (UMathTypes::AreVectorsNearlyEqual(CurrentWorldLocation, TargetWorldLocation) == false)
			{
				const FVector LerpVector = FMath::Lerp(CurrentWorldLocation, TargetWorldLocation, alpha);
				SetOwnerLocation(LerpVector);
			}
		}

		// rotation
		{
			const FRotator TargetFacingRotator = GetGridFacingDirectionWorldRotation();
			const FRotator CurrentWorldRotation = GetOwnerRotation();
			if (UMathTypes::AreRotationsNearlyEqual(CurrentWorldRotation, TargetFacingRotator) == false)
			{
				const FRotator LerpRotator = FMath::Lerp(CurrentWorldRotation, TargetFacingRotator, alpha);
				SetOwnerRotation(LerpRotator);
			}
		}

		if (bFinishedChangingTransform)
		{
			FinishChangingTransform();
		}
	}


	return;
}


void UHexGridNavigationComponent::MoveUpOverDuration(const float Duration)
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_LERR(Grid, HexGrid, "grid is null");

	FHexGridCoordinate NewCoordinate;
	if (Grid->GetFirstRelativeNeighborTile_Up(GetGridCoordinate(), GetGridFacingDirection(), NewCoordinate))
	{
		SetGridCoordinateOverDuration(NewCoordinate, Duration);
	}
	else
	{
		L_WARN(HexGrid, "no tile found");
	}


	return;
}


void UHexGridNavigationComponent::MoveDownOverDuration(const float Duration)
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_LERR(Grid, HexGrid, "grid is null");

	FHexGridCoordinate NewCoordinate;
	if (Grid->GetFirstRelativeNeighborTile_Down(GetGridCoordinate(), GetGridFacingDirection(), NewCoordinate))
	{
		SetGridCoordinateOverDuration(NewCoordinate, Duration);
	}
	else
	{
		L_WARN(HexGrid, "no tile found");
	}


	return;
}


void UHexGridNavigationComponent::MoveLeftOverDuration(const float Duration)
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_LERR(Grid, HexGrid, "grid is null");

	FHexGridCoordinate NewCoordinate;
	if (Grid->GetFirstRelativeNeighborTile_Left(GetGridCoordinate(), GetGridFacingDirection(), NewCoordinate))
	{
		SetGridCoordinateOverDuration(NewCoordinate, Duration);
	}
	else
	{
		L_WARN(HexGrid, "no tile found");
	}


	return;
}


void UHexGridNavigationComponent::MoveRightOverDuration(const float Duration)
{
	AHexGrid* Grid = GetGrid();
	IF_NULL_RETURN_LERR(Grid, HexGrid, "grid is null");

	FHexGridCoordinate NewCoordinate;
	if (Grid->GetFirstRelativeNeighborTile_Right(GetGridCoordinate(), GetGridFacingDirection(), NewCoordinate))
	{
		SetGridCoordinateOverDuration(NewCoordinate, Duration);
	}
	else
	{
		L_WARN(HexGrid, "no tile found");
	}


	return;
}


void UHexGridNavigationComponent::TurnRightOverDuration(const float Duration)
{
	SetGridFacingDirectionOverDuration(
		FHexGridCoordinate::GetDirectionAfterClockwiseRotations(
			GetGridFacingDirection(),
			1
		),
		Duration
	);


	return;
}


void UHexGridNavigationComponent::TurnLeftOverDuration(const float Duration)
{
	SetGridFacingDirectionOverDuration(
		FHexGridCoordinate::GetDirectionAfterClockwiseRotations(
			GetGridFacingDirection(),
			-1
		),
		Duration
	);


	return;
}


void UHexGridNavigationComponent::StartTraversingPath(FHexGridCoordinateList const& Path, const float StepDuration, bool bShouldTurnToFaceEachStep)
{
	if (IsTraversingPath())
	{
		StopTraversingPath(false);
	}

	_PathCoordinates.Reset();
	if (Path.Num() <= 0)
	{
		return;
	}

	_PathCoordinates.Reserve(Path.Num());
	_PathCoordinates.Append(Path);
	_ChangeTransformDuration = StepDuration;
	SetTurnToFaceEachStepAlongPathEnabled(bShouldTurnToFaceEachStep);
	_PathTraversalIndex = 0;

	HandlePathTraversalStarted();

	if (OnPathTraversalStarted.IsBound())
	{
		OnPathTraversalStarted.Broadcast();
	}

	StartNextStepAlongPath();


	return;
}


void UHexGridNavigationComponent::StopTraversingPath(bool bDidReachEndOfPath)
{
	HandlePathTraversalFinished(bDidReachEndOfPath);

	if (OnPathTraversalFinished.IsBound())
	{
		OnPathTraversalFinished.Broadcast(bDidReachEndOfPath);
	}

	_PathCoordinates.Reset();
	_PathTraversalIndex = -1;


	return;
}


bool UHexGridNavigationComponent::IsFacingNextStepAlongPath() const
{
	const int NextPathIndex = _PathTraversalIndex + 1;
	if (UMCommon::IsValidIndex(_PathCoordinates.List, NextPathIndex) == false)
	{
		L_ERR(HexGrid, "invalid next path traversal index '{0}' for path size '{1}'", NextPathIndex, _PathCoordinates.Num());
		return false;
	}

	const FHexGridCoordinate NextCoordinate = _PathCoordinates[_PathTraversalIndex + 1];


	return	_CurrentGridTransform.Coordinate == NextCoordinate ||
		_CurrentGridTransform.FacingDirection == _CurrentGridTransform.Coordinate.GetNearestDirectionTowardsCoordinate(NextCoordinate);
}


void UHexGridNavigationComponent::StartNextStepAlongPath()
{
	const FHexGridCoordinate NextCoordinate = _PathCoordinates[_PathTraversalIndex + 1];

	if (ShouldTurnToFaceEachStepAlongPath() == false || IsFacingNextStepAlongPath())
	{
		SetGridCoordinateOverDuration(NextCoordinate, _ChangeTransformDuration);

		_PathTraversalIndex += 1;
	}
	else if (ShouldTurnAndMoveOnEachStep())
	{
		// Calculate the direction we should be facing when we arrive

		const EHexGridDirection NextCoordinateDirection = _CurrentGridTransform.Coordinate.GetNearestDirectionTowardsCoordinate(NextCoordinate);

		// Apply both the Coordinate and the Direction to the transform change
		SetGridTransformOverDuration(FHexGridTransform(NextCoordinate, NextCoordinateDirection), _ChangeTransformDuration);

		// Increment index immediately because we are physically moving to the tile
		_PathTraversalIndex += 1;
	}
	else
	{
		const EHexGridDirection NextCoordinateDirection = _CurrentGridTransform.Coordinate.GetNearestDirectionTowardsCoordinate(NextCoordinate);

		SetGridFacingDirectionOverDuration(NextCoordinateDirection, _ChangeTransformDuration);
	}


	return;
}


void UHexGridNavigationComponent::FinishChangingTransform()
{
	_bIsChangingTransform = false;

	if (OnGridTransformChangeOverDurationFinished.IsBound())
	{
		OnGridTransformChangeOverDurationFinished.Broadcast(_PreviousGridTransform, _CurrentGridTransform);
	}

	if (IsTraversingPath())
	{
		const bool bFinishedTraversingPath = _PathTraversalIndex == _PathCoordinates.Num() - 1;
		if (bFinishedTraversingPath == false &&
			CanContinuePathTraversal())
		{
			StartNextStepAlongPath();
		}
		else
		{
			StopTraversingPath(bFinishedTraversingPath);
		}
	}


	return;
}


void UHexGridNavigationComponent::SetTurnToFaceEachStepAlongPathEnabled(bool bEnabled)
{ 
	_bShouldTurnToFaceEachStepAlongPath = bEnabled;


	return;
}


void UHexGridNavigationComponent::SetTurnAndMoveOnEachStepEnabled(bool bEnabled)
{ 
	_bShouldTurnAndMoveOnEachStep = bEnabled;


	return;
}