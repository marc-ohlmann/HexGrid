// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HexGridTypes.h"
#include "HexGridNavigationComponent.generated.h"


class AHexGrid;


/*
* A component for moving actors along hex tiles on a hex grid
*
*/
UCLASS(Blueprintable,
	ClassGroup = (Custom),
	meta =
	(BlueprintSpawnableComponent),
	HideCategories = (
		ComponentTick,
		Tags,
		ComponentReplication,
		Activation,
		Cooking,
		Variable,
		Replication,
		AssetUserData,
		Navigation,
		Collision,
		Physics,
		Rendering,
		LOD
		)
)
class HEXGRIDPLUGIN_API UHexGridNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHexGridNavigationComponent();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:


	// relative grid navigation

public:

	// change position up on the grid over a duration 
	// relative to the navigator's current facing direction
	void MoveUpOverDuration(const float Duration);

	// change position down on the grid over a duration 
	// relative to the navigator's current facing direction
	void MoveDownOverDuration(const float Duration);

	// change position to the left on the grid over a duration 
	// relative to the navigator's current facing direction
	void MoveLeftOverDuration(const float Duration);

	// change position to the right on the grid over a duration 
	// relative to the navigator's current facing direction
	void MoveRightOverDuration(const float Duration);

	// change facing direction to the left on the grid over a duration
	// relative to the navigator's current facing direction
	void TurnLeftOverDuration(const float Duration);

	// change facing direction to the right on the grid over a duration
	// relative to the navigator's current facing direction
	void TurnRightOverDuration(const float Duration);

	//


	// grid transform

public:

	// set the target grid to use grid coordinates within
	void SetGrid(AHexGrid* TargetGrid);

	// get the grid that this pawn is locked to
	AHexGrid* GetGrid() const;

	// templated get the grid that this pawn is locked to
	template <typename TGridClass>
	TGridClass* GetGrid() const
	{
		return Cast<TGridClass>(GetGrid());
	}

	// set the coordinate that this pawn is located at within the current grid
	void SetGridCoordinate(FHexGridCoordinate const& TargetGridLocation, bool bSnapToGrid);

	// get the current tile coordinate and facing direction that this pawn is located at within the current grid
	FHexGridTransform GetGridTransform() const { return _CurrentGridTransform; }

	// set the current tile coordinate and facing direction that this pawn is located at within the current grid
	void SetGridTransform(FHexGridTransform const& TargetGridTransform, bool bSnapToGrid);

	// get the current tile coordinate that this pawn is located at within the current grid
	FHexGridCoordinate GetGridCoordinate() const { return _CurrentGridTransform.Coordinate; }

	// get the world location for the current tile coordinate on the grid
	FVector GetGridCoordinateWorldLocation(bool bIncludeWorldOffset) const;

	// get the world rotation for the current facing direction on the grid
	FRotator GetGridFacingDirectionWorldRotation() const;

	// set the direction to face within the current grid
	void SetGridFacingDirection(const EHexGridDirection Direction, bool bSnapToGrid);

	// get current facing direction within the current grid
	EHexGridDirection GetGridFacingDirection() const { return _CurrentGridTransform.FacingDirection; }

	// change facing direction within the current grid over a duration of time
	void SetGridFacingDirectionOverDuration(const EHexGridDirection Direction, const float Duration);

	// change location to a given grid coordinate within the current grid over a duration of time
	void SetGridCoordinateOverDuration(FHexGridCoordinate const& TargetGridLocation, const float Duration);

	// change location and rotation to a given grid transform on the current grid over a duration of time
	void SetGridTransformOverDuration(FHexGridTransform const& TargetGridTransform, const float Duration);

	// check if this component is changing it's transform over a duration
	bool IsChangingTransform() const { return _bIsChangingTransform; }

	// delegate broadcasts after the current transform is changed
	FOnGridTransformChanged OnGridTransformChanged;

	// delegate broadcasts after completing a change in transform over a duration
	FOnGridTransformChanged OnGridTransformChangeOverDurationFinished;

	// set the world offset used when snapping to grid locations
	void SetGridLocationWorldOffset(FVector const& InOffset);
	
	// get the world offset used when snapping to grid locations
	FVector GetGridLocationWorldOffset() const { return _GridLocationWorldOffset; }

private:

	// flag indicates if this component is changing it's transform over a duration
	UPROPERTY()
	bool _bIsChangingTransform;

	// current tile and facing direction on the grid
	UPROPERTY()
	FHexGridTransform _CurrentGridTransform;

	// previous tile and facing direction on the grid during grid transform changes
	UPROPERTY()
	FHexGridTransform _PreviousGridTransform;

	// current grid the pawn is locked too
	UPROPERTY()
	TWeakObjectPtr<AHexGrid> _CurrentGrid;

	// total time for current transform change
	UPROPERTY()
	float _ChangeTransformDuration;

	// elapsed time while changing transform
	UPROPERTY()
	float _ChangeTransformDurationBuffer;

	// world offset applied when snapping to grid locations
	UPROPERTY()
	FVector _GridLocationWorldOffset;

	// moves this pawn instantly to it's transform within the grid
	void SnapToGridTransform();

	// moves this pawn instantly to it's coordinate within the grid
	void SnapToGridLocation();

	// rotates this pawn instantly to it's direction facing within the grid
	void SnapToGridDirection();

	// set the owning actor's rotation 
	// applies to the actor or pawn controller depending on pawn control settings
	void SetOwnerRotation(const FRotator NewRotation);

	// set the owning actor's location
	void SetOwnerLocation(const FVector& NewLocation);

	// get the owning actor's location
	FVector GetOwnerLocation() const;

	// get the owning actor's rotation
	FRotator GetOwnerRotation() const;

	// executes transform changes over durations on every frame
	void OnTick_ChangeTransformOverDuration(float DeltaTime);

	// called when any transform change is finished
	void FinishChangingTransform();

	// begin changing the owner transform to the current grid transform
	void StartChangingTransformOverDuration(float Duration);

	//


	// path traversal

public:

	// start to move along the given set of coordinates in order, using the provided duration for each step
	void StartTraversingPath(FHexGridCoordinateList const& Path, const float StepDuration, bool bShouldTurnToFaceEachStep);

	// check if this component is currently navigating a path
	bool IsTraversingPath() const { return _PathCoordinates.Num() > 0; }

	// delegate broadcasts after starting a path traversal
	FOnPathTraversalStarted OnPathTraversalStarted;

	// delegate broadcasts after completing a path traversal
	FOnPathTraversalFinished OnPathTraversalFinished;

	// call to stop traversing a path
	void StopTraversingPath(bool bDidReachEndOfPath);

	// check if the current path traversal requires facing each step before moving along the path
	bool ShouldTurnToFaceEachStepAlongPath() const { return _bShouldTurnToFaceEachStepAlongPath; }

	// set if the current path traversal requires facing each step before moving along the path
	void SetTurnToFaceEachStepAlongPathEnabled(bool bEnabled);

	// check if currently facing the next step along the current path being traversed
	bool IsFacingNextStepAlongPath() const;

	// check if we can continue moving along the path
	virtual bool CanContinuePathTraversal() const { return true; }

	// get the current path coordinate index while traversing a path
	int GetPathTraversalIndex() const { return _PathTraversalIndex; }

	// get the total number of path coordinates for the current path traversal
	int GetPathTraversalNumberOfCoordinates() const { return _PathCoordinates.Num(); }

	// get the total number of path coordinates for the current path traversal
	void GetPathTraversalCoordinates(FHexGridCoordinateList& OutCoordinates) const
	{
		return OutCoordinates.Append(_PathCoordinates);
	}

	// get the flag that indicates if move and turning can occur at the same time for each step.
	bool ShouldTurnAndMoveOnEachStep() const { return _bShouldTurnAndMoveOnEachStep; }

	// set the flag that indicates if move and turning can occur at the same time for each step.
	void SetTurnAndMoveOnEachStepEnabled(bool bEnabled);

protected:

	// called when path traversal starts
	virtual void HandlePathTraversalStarted() {}

	// called when path traversal stops
	virtual void HandlePathTraversalFinished(bool bDidReachEndOfPath) {}

private:

	// start moving to the next coordinate on the current path traversal
	void StartNextStepAlongPath();

	// set of coordinates that defines the path to be traversed
	UPROPERTY()
	FHexGridCoordinateList _PathCoordinates;

	// flag indicates if rotation should change towards each coordinate being traversed along the path
	UPROPERTY()
	bool _bShouldTurnToFaceEachStepAlongPath;

	// index of the current path coordinate that is being traversed
	UPROPERTY()
	int _PathTraversalIndex;

	// flag indicates if move and turning can occur at the same time for each step.
	UPROPERTY()
	bool _bShouldTurnAndMoveOnEachStep;

	//

};
