// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "HexGridTypes.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "GraphAStar.h"
#include "HexGridFogOfWarComponent.h"
#include "HexGridTileTypeAsset.h"

#include "GameFramework/Actor.h"


const int FHexGridCoordinate::c_HexNumberOfSides = 6;

const FHexGridCoordinate FHexGridCoordinate::c_Zero = FHexGridCoordinate(0, 0);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_N = FHexGridCoordinate(0, -1);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_NE = FHexGridCoordinate(1, -1);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_SE = FHexGridCoordinate(1, 0);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_S = FHexGridCoordinate(0, 1);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_SW = FHexGridCoordinate(-1, 1);
const FHexGridCoordinate FHexGridCoordinate::c_HexGridDirection_NW = FHexGridCoordinate(-1, 0);
const FHexGridCoordinate FHexGridCoordinate::c_Neighbors[] = {
		FHexGridCoordinate::c_HexGridDirection_N,
		FHexGridCoordinate::c_HexGridDirection_NE,
		FHexGridCoordinate::c_HexGridDirection_SE,
		FHexGridCoordinate::c_HexGridDirection_S,
		FHexGridCoordinate::c_HexGridDirection_SW,
		FHexGridCoordinate::c_HexGridDirection_NW,
};

const float FHexGridCoordinate::c_QSizeToHorizontalDistanceRatio = UMathTypes::c_Half * 3.f;
const float FHexGridCoordinate::c_QSizeToVerticalDistanceRatio = UMathTypes::c_Root_Three * UMathTypes::c_Half;
const float FHexGridCoordinate::c_RSizeToVerticalDistanceRatio = UMathTypes::c_Root_Three;

const float FHexGridCoordinate::c_XSizeToQRatio = UMathTypes::c_Third * 2.f;
const float FHexGridCoordinate::c_XSizeToRRatio = UMathTypes::c_Third * -1.f;
const float FHexGridCoordinate::c_YSizeToRRatio = UMathTypes::c_Root_Three * UMathTypes::c_Third;

const double FFractionalHexGridCoordinate::c_FractionalNudgeDistance = 1e-6;

const ECollisionChannel UHexGridTypes::c_HexTileTraceChannel = ECC_GameTraceChannel1;


FHexGridCoordinateList FHexGridCoordinate::GetRelativeNeighbors_Up(
	const EHexGridDirection FacingDirection
) const
{
	FHexGridCoordinateList Out;
	Out.Add(GetNeighbor(FacingDirection));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, 1)));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, -1)));


	return Out;
}


FHexGridCoordinateList FHexGridCoordinate::GetRelativeNeighbors_Down(
	const EHexGridDirection FacingDirection
) const
{
	FHexGridCoordinateList Out;
	Out.Add(GetNeighbor(FHexGridCoordinate::GetReverseDirection(FacingDirection)));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, -2)));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, 2)));


	return Out;
}


FHexGridCoordinateList FHexGridCoordinate::GetRelativeNeighbors_Left(
	const EHexGridDirection FacingDirection
) const
{
	FHexGridCoordinateList Out;
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, -2)));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, -1)));


	return Out;
}


FHexGridCoordinateList FHexGridCoordinate::GetRelativeNeighbors_Right(
	const EHexGridDirection FacingDirection
) const
{
	FHexGridCoordinateList Out;
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, 1)));
	Out.Add(GetNeighbor(FHexGridCoordinate::GetDirectionAfterClockwiseRotations(FacingDirection, 2)));


	return Out;
}


FHexGridCoordinateList FHexGridCoordinate::GetNeighbors() const
{
	FHexGridCoordinateList Out;
	Out.Add(North());
	Out.Add(Northeast());
	Out.Add(Southeast());
	Out.Add(South());
	Out.Add(Southwest());
	Out.Add(Northwest());


	return Out;
}


EHexGridDirection FHexGridCoordinate::GetDirectionFromNeighborsAToB(
	FHexGridCoordinate const& A,
	FHexGridCoordinate const& B
)
{
	if (FHexGridCoordinate::AreNeighbors(A, B) == false)
	{
		L_ERR(HexGrid, "coordinates '{0}' and '{1}' are not neighbors", A, B);
		return EHexGridDirection::north;
	}

	FOR_FORWARD(i, FHexGridCoordinate::c_HexNumberOfSides)
	{
		const EHexGridDirection DirectionIterator = static_cast<EHexGridDirection>(i);
		if (B == A.GetNeighbor(DirectionIterator))
		{
			return DirectionIterator;
		}
	}


	L_ERR(HexGrid, "failed to find direction for coordinates '{0}' and '{1}'", A, B);
	return EHexGridDirection::north;
}

// check if the given neighboring coordinate is in the given forward direction, or adjacent to it
bool FHexGridCoordinate::IsNeighborInFront(
	const EHexGridDirection ForwardDirection,
	FHexGridCoordinate const& NeighborCoordinate
) const
{
	if (IsNeighbor(NeighborCoordinate) == false)
	{
		L_ERR(HexGrid, "coordinates '{0}' and '{1}' are not neighbors", ToString(), NeighborCoordinate);
		return false;
	}


	return FHexGridCoordinate::IsForwardRelativeToDirection(ForwardDirection, GetDirectionTowardsNeighbor(NeighborCoordinate));
}

// check if the given neighboring coordinate is in the reverse of the given forward direction, or adjacent to it
bool FHexGridCoordinate::IsNeighborBehind(
	const EHexGridDirection ForwardDirection,
	FHexGridCoordinate const& NeighborCoordinate
) const
{
	if (IsNeighbor(NeighborCoordinate) == false)
	{
		L_ERR(HexGrid, "coordinates '{0}' and '{1}' are not neighbors", ToString(), NeighborCoordinate);
		return false;
	}


	return FHexGridCoordinate::IsBackwardRelativeToDirection(ForwardDirection, GetDirectionTowardsNeighbor(NeighborCoordinate));
}


void FHexGridCoordinate::Print() const
{
	L_VERBOSE(HexGrid, "{0}", ToString());
}


bool FHexGridCoordinate::GetCoordinatesWithinRangeOfTile(
	FHexGridCoordinate const& CenterTile,
	const int Range,
	FHexGridCoordinateList& OutCoordinates
)
{
	for (int q = -Range; q <= Range; q++)
	{
		const int r1 = FMath::Max(-Range, -q - Range);
		const int r2 = FMath::Min(Range, -q + Range);

		for (int r = r1; r <= r2; r++)
		{
			OutCoordinates.Add(CenterTile + FHexGridCoordinate(q, r));
		}
	}


	return OutCoordinates.Num() > 0;
}


bool FHexGridCoordinate::GetCoordinatesOnLine(
	FHexGridCoordinate const& A,
	FHexGridCoordinate const& B,
	FHexGridCoordinateList& OutCoordinates
)
{
	// get distance between coordinates
	const int N = FHexGridCoordinate::Distance(A, B);

	const FFractionalHexGridCoordinate a_nudge = FFractionalHexGridCoordinate(
		A.Q + FFractionalHexGridCoordinate::c_FractionalNudgeDistance,
		A.R + FFractionalHexGridCoordinate::c_FractionalNudgeDistance
	);
	const FFractionalHexGridCoordinate b_nudge = FFractionalHexGridCoordinate(
		B.Q + FFractionalHexGridCoordinate::c_FractionalNudgeDistance,
		B.R + FFractionalHexGridCoordinate::c_FractionalNudgeDistance
	);

	const double StepDistance = 1.0 / FMath::Max(N, 1);

	for (int i = 0; i <= N; i++)
	{
		OutCoordinates.Add(
			FFractionalHexGridCoordinate::HexLerp(a_nudge, b_nudge, StepDistance * i).Round()
		);
	}


	return OutCoordinates.Num() > 0;
}


bool FHexGridCoordinate::GetCoordinatesAtDistanceFromCoordinate(
	FHexGridCoordinate const& Target,
	const int Distance,
	FHexGridCoordinateList& OutCoordinates
)
{
	if (Distance < 0) return false;
	else if (Distance == 0)
	{
		OutCoordinates.Add(Target);
		return true;
	}

	FHexGridCoordinate HexCoordinateIterator = Target + (FHexGridCoordinate::c_Zero.GetNeighbor(EHexGridDirection::southwest) * Distance);
	FOR_FORWARD(i, FHexGridCoordinate::c_HexNumberOfSides)
	{
		FOR_FORWARD(j, Distance)
		{
			OutCoordinates.Add(HexCoordinateIterator);
			HexCoordinateIterator = HexCoordinateIterator.GetNeighbor(static_cast<EHexGridDirection>(i));
		}
	}


	return OutCoordinates.Num() > 0;
}


int FHexGridCoordinate::GetClockwiseRotationsFromNorthToDirection(const EHexGridDirection Direction)
{
	return static_cast<int>(Direction);
}


int FHexGridCoordinate::GetCounterClockwiseRotationsFromNorthToDirection(const EHexGridDirection Direction)
{
	if (Direction == EHexGridDirection::north) return 0;


	return UMCommon::GetEnumSize<EHexGridDirection>() - static_cast<int>(Direction);
}


int FHexGridCoordinate::GetMinimumRotationsFromNorthToDirection(const EHexGridDirection Direction)
{
	const int Clockwise = GetClockwiseRotationsFromNorthToDirection(Direction);
	const int CounterClockwise = GetCounterClockwiseRotationsFromNorthToDirection(Direction);
	const int DirectionScalar = CounterClockwise < Clockwise ? -1 : 1;


	return DirectionScalar * FMath::Min(Clockwise, CounterClockwise);
}


FHexGridCoordinate FHexGridCoordinate::RotateCounterClockwise(const int Rotations) const
{
	FHexGridCoordinate Iterator = *this;
	for (int i = 0; i < Rotations; i++)
	{
		Iterator = Iterator.RotateCounterClockwise();
	}


	return Iterator;
}


FHexGridCoordinate FHexGridCoordinate::RotateClockwise(const int Rotations) const
{
	FHexGridCoordinate Iterator = *this;
	for (int i = 0; i < Rotations; i++)
	{
		Iterator = Iterator.RotateClockwise();
	}


	return Iterator;
}


FHexGridCoordinate FHexGridCoordinate::RotateFromDirectionToDirection(
	const EHexGridDirection From,
	const EHexGridDirection To
) const
{
	FHexGridCoordinate Iterator = *this;
	const int RotationsToFacing = FHexGridCoordinate::GetMinimumRotationsBetweenDirections(From, To);
	if (RotationsToFacing > 0)
	{
		Iterator = Iterator.RotateClockwise(RotationsToFacing);
	}
	else if (RotationsToFacing < 0)
	{
		Iterator = Iterator.RotateCounterClockwise(FMath::Abs(RotationsToFacing));
	}


	return Iterator;
}


int FHexGridCoordinate::GetClockwiseRotationsBetweenDirections(
	const EHexGridDirection FromDirection,
	const EHexGridDirection ToDirection
)
{
	const int FromDirection_RotationsFromNorth = FHexGridCoordinate::GetClockwiseRotationsFromNorthToDirection(FromDirection);
	const int ToDirection_RotationsFromNorth = FHexGridCoordinate::GetClockwiseRotationsFromNorthToDirection(ToDirection);
	const int Unwind = FromDirection_RotationsFromNorth > ToDirection_RotationsFromNorth ? UMCommon::GetEnumSize<EHexGridDirection>() : 0;


	return (ToDirection_RotationsFromNorth - FromDirection_RotationsFromNorth) + Unwind;
}


int FHexGridCoordinate::GetCounterClockwiseRotationsBetweenDirections(
	const EHexGridDirection FromDirection,
	const EHexGridDirection ToDirection
)
{
	const int FromDirection_RotationsFromNorth = FHexGridCoordinate::GetCounterClockwiseRotationsFromNorthToDirection(FromDirection);
	const int ToDirection_RotationsFromNorth = FHexGridCoordinate::GetCounterClockwiseRotationsFromNorthToDirection(ToDirection);
	const int Unwind = FromDirection_RotationsFromNorth > ToDirection_RotationsFromNorth ? UMCommon::GetEnumSize<EHexGridDirection>() : 0;


	return (ToDirection_RotationsFromNorth - FromDirection_RotationsFromNorth) + Unwind;
}


int FHexGridCoordinate::GetMinimumRotationsBetweenDirections(
	const EHexGridDirection FromDirection,
	const EHexGridDirection ToDirection
)
{
	const int Clockwise = GetClockwiseRotationsBetweenDirections(FromDirection, ToDirection);
	const int CounterClockwise = GetCounterClockwiseRotationsBetweenDirections(FromDirection, ToDirection);
	const int DirectionScalar = CounterClockwise < Clockwise ? -1 : 1;


	return DirectionScalar * FMath::Min(Clockwise, CounterClockwise);
}


bool FHexGridCoordinate::GetCoordinatesInHexagonAroundCoordinate(
	FHexGridCoordinate const& Target,
	const int Size,
	FHexGridCoordinateList& OutCoordinates
)
{
	if (Size <= 0)
	{
		return false;
	}
	else if (Size == 1)
	{
		OutCoordinates.Add(Target);
		return true;
	}


	return FHexGridCoordinate::GetCoordinatesWithinRangeOfTile(Target, Size - 1, OutCoordinates);
}


bool FHexGridCoordinate::GetCoordinatesInRectangleAroundCoordinate(
	FHexGridCoordinate const& Target,
	const int LeftOffset,
	const int RightOffset,
	const int TopOffset,
	const int BottomOffset,
	FHexGridCoordinateList& OutList
)
{
	for (int q = LeftOffset; q <= RightOffset; q++)
	{
		const int q_offset = floor(q / 2.0); // or q>>1

		for (int r = TopOffset - q_offset; r <= BottomOffset - q_offset; r++)
		{
			OutList.Add(Target + FHexGridCoordinate(q, r));
		}
	}


	return OutList.Num() > 0;
}


bool FHexGridCoordinate::GetCoordinatesInTriangleAroundCoordinate(
	FHexGridCoordinate const& Target,
	const int Size,
	const EHexGridShape TriangleOrientation,
	FHexGridCoordinateList& OutList
)
{
	for (int i = -(Size - 1); i <= (Size - 1); i++) {
		for (int j = -(Size - 1); j <= (Size - 1); j++) {

			const int k = -i - j;
			if (FMath::Abs(i) + FMath::Abs(j) + FMath::Abs(k) > (Size - 1) * 2) continue;

			int Q = i, R = j, S = k;

			bool bIsInTriangle = false;
			switch (TriangleOrientation)
			{
			case EHexGridShape::triangle_s_pos: bIsInTriangle = (Q <= 0 && R <= 0); break;
			case EHexGridShape::triangle_s_neg: bIsInTriangle = (Q >= 0 && R >= 0); break;
			case EHexGridShape::triangle_q_pos: bIsInTriangle = (R <= 0 && S <= 0); break;
			case EHexGridShape::triangle_q_neg: bIsInTriangle = (R >= 0 && S >= 0); break;
			case EHexGridShape::triangle_r_pos: bIsInTriangle = (S <= 0 && Q <= 0); break;
			case EHexGridShape::triangle_r_neg: bIsInTriangle = (S >= 0 && Q >= 0); break;
			}

			if (bIsInTriangle)
			{
				OutList.Add(Target + FHexGridCoordinate(Q, R));
			}
		}
	}


	return OutList.Num() > 0;
}


bool FHexGridCoordinate::GetCoordinatesInParallelogramAroundCoordinate(
	FHexGridCoordinate const& Target,
	const int Q1,
	const int R1,
	const int Q2,
	const int R2,
	FHexGridCoordinateList& OutList
)
{
	for (int q = Q1; q <= Q2; q++)
	{
		for (int r = R1; r <= R2; r++)
		{
			OutList.Add(Target + FHexGridCoordinate(q, r));
		}
	}


	return OutList.Num() > 0;
}


bool FHexGridCoordinate::GetCoordinatesInShapeAroundCoordinate(
	FHexGridCoordinate const& Target,
	const EHexGridShape Shape,
	const int ShapeSize,
	FHexGridCoordinateList& OutCoordinates
)
{
	switch (Shape)
	{
	case EHexGridShape::hexagon:
		FHexGridCoordinate::GetCoordinatesInHexagonAroundCoordinate(
			Target,
			ShapeSize,
			OutCoordinates
		);
		break;

	case EHexGridShape::rectangle:
		FHexGridCoordinate::GetCoordinatesInRectangleAroundCoordinate(
			Target,
			-(ShapeSize - 1),
			(ShapeSize - 1),
			-(ShapeSize - 1),
			(ShapeSize - 1),
			OutCoordinates
		);
		break;

	case EHexGridShape::parallelogram:
		FHexGridCoordinate::GetCoordinatesInParallelogramAroundCoordinate(
			Target,
			-(ShapeSize - 1),
			-(ShapeSize - 1),
			(ShapeSize - 1),
			(ShapeSize - 1),
			OutCoordinates
		);
		break;

		// triangles have their own switch handler function
	case EHexGridShape::triangle_s_pos:
	case EHexGridShape::triangle_s_neg:
	case EHexGridShape::triangle_q_pos:
	case EHexGridShape::triangle_q_neg:
	case EHexGridShape::triangle_r_pos:
	case EHexGridShape::triangle_r_neg:
		FHexGridCoordinate::GetCoordinatesInTriangleAroundCoordinate(
			Target,
			ShapeSize,
			Shape,
			OutCoordinates
		);
		break;

	default:
		L_ERR(HexGrid, "default on hex grid shape: '{0}'", Shape);
		break;
	}



	return OutCoordinates.Num() > 0;
}


FVector2D FHexGridCoordinate::GetCenterPlaneOffsetLocation(const float TileRadius) const
{
	return FVector2D(
		TileRadius * FHexGridCoordinate::c_QSizeToHorizontalDistanceRatio * static_cast<float>(Q),
		TileRadius *
		(FHexGridCoordinate::c_QSizeToVerticalDistanceRatio * static_cast<float>(Q)
			+ FHexGridCoordinate::c_RSizeToVerticalDistanceRatio * static_cast<float>(R)
			)
	);
}


TArray<FVector2D> FHexGridCoordinate::GetCornerPlaneOffsetLocations(const float TileRadius) const
{
	TArray<FVector2D> Out;
	const FVector2D CenterWorldOffsetLocation = GetCenterPlaneOffsetLocation(TileRadius);

	// right corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			TileRadius,
			0.f
		)
	);

	// lower right corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			UMathTypes::c_Half * TileRadius,
			-1.f * UMathTypes::c_Root_Three * UMathTypes::c_Half * TileRadius
		)
	);

	// lower left corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			-1.f * UMathTypes::c_Half * TileRadius,
			-1.f * UMathTypes::c_Root_Three * UMathTypes::c_Half * TileRadius
		)
	);

	// left corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			-1.f * TileRadius,
			0.f
		)
	);

	// upper left corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			-1.f * UMathTypes::c_Half * TileRadius,
			UMathTypes::c_Root_Three * UMathTypes::c_Half * TileRadius
		)
	);

	// upper right corner
	Out.Add(CenterWorldOffsetLocation +
		FVector2D(
			UMathTypes::c_Half * TileRadius,
			UMathTypes::c_Root_Three * UMathTypes::c_Half * TileRadius
		)
	);


	return Out;
}


FVector FHexGridCoordinate::GetCenterWorldOffsetLocation(const float TileRadius) const
{
	return FVector(
		GetCenterPlaneOffsetLocation(TileRadius)
		, 0.f // Z-coordinate is always zero
	);
}


TArray<FVector> FHexGridCoordinate::GetCornerWorldOffsetLocations(const float TileRadius) const
{
	TArray<FVector> Out;
	const TArray<FVector2D> CornerPlaneLocations = GetCornerPlaneOffsetLocations(TileRadius);

	FOR_FORWARD(i, CornerPlaneLocations.Num())
	{
		Out.Add(
			FVector(
				CornerPlaneLocations[i]
				, 0.f // Z-coordinate is always zero
			)
		);
	}

	return Out;
}


FFractionalHexGridCoordinate FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(
	const FVector2D& PixelPosition,
	const float TileRadius
)
{
	if (FMath::IsNearlyZero(TileRadius))
	{
		return FFractionalHexGridCoordinate(0.0, 0.0);
	}


	return FFractionalHexGridCoordinate(
		(FHexGridCoordinate::c_XSizeToQRatio * PixelPosition.X) / TileRadius,
		(FHexGridCoordinate::c_XSizeToRRatio * PixelPosition.X
			+ FHexGridCoordinate::c_YSizeToRRatio * PixelPosition.Y
			) / TileRadius
	);
}


FHexGridCoordinate FHexGridCoordinate::GetHexGridCoordinateFromPlaneOffsetLocation(
	const FVector2D& PixelPosition,
	const float TileRadius
)
{
	if (FMath::IsNearlyZero(TileRadius))
	{
		return FHexGridCoordinate::c_Zero;
	}

	return FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(
		PixelPosition, 
		TileRadius
	).Round();
}


FHexGridCoordinate FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(
	FVector const& WorldOffsetLocation,
	const float TileRadius
)
{
	return GetHexGridCoordinateFromPlaneOffsetLocation(
		FVector2D(WorldOffsetLocation), 
		TileRadius
	);
}


bool FHexGridCoordinateList::GenerateHexCoordinatesInBounds(
	const FBox2D& PixelBounds,
	const float TileRadius,
	FHexGridCoordinateList& OutCoordinates
)
{
	if (TileRadius <= 0.f || !PixelBounds.bIsValid)
	{
		return false;
	}

	const int OriginalNum = OutCoordinates.Num();

	// Convert the four corners of the pixel bounds to fractional hex coordinates
	const FFractionalHexGridCoordinate Corner1 = FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(FVector2D(PixelBounds.Min.X, PixelBounds.Min.Y), TileRadius);
	const FFractionalHexGridCoordinate Corner2 = FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(FVector2D(PixelBounds.Max.X, PixelBounds.Min.Y), TileRadius);
	const FFractionalHexGridCoordinate Corner3 = FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(FVector2D(PixelBounds.Min.X, PixelBounds.Max.Y), TileRadius);
	const FFractionalHexGridCoordinate Corner4 = FFractionalHexGridCoordinate::GetFractionalHexGridCoordinateFromPlaneOffsetLocation(FVector2D(PixelBounds.Max.X, PixelBounds.Max.Y), TileRadius);

	// Find the min/max Q and R values from these corners to create a broad-phase search box.
	const double MinQ = FMath::Min(Corner1.Q, Corner2.Q, Corner3.Q, Corner4.Q);
	const double MaxQ = FMath::Max(Corner1.Q, Corner2.Q, Corner3.Q, Corner4.Q);
	const double MinR = FMath::Min(Corner1.R, Corner2.R, Corner3.R, Corner4.R);
	const double MaxR = FMath::Max(Corner1.R, Corner2.R, Corner3.R, Corner4.R);

	// Determine the integer search range
	const int Q_Min = FMath::FloorToInt(MinQ);
	const int Q_Max = FMath::CeilToInt(MaxQ);
	const int R_Min = FMath::FloorToInt(MinR);
	const int R_Max = FMath::CeilToInt(MaxR);

	// Loop over this rectangular (parallelogram in hex-space) area
	for (int q = Q_Min; q <= Q_Max; ++q)
	{
		for (int r = R_Min; r <= R_Max; ++r)
		{
			const FHexGridCoordinate CurrentCoord(q, r);

			// Narrow-phase: Convert the integer hex coord back to its pixel center
			const FVector2D TileCenter = CurrentCoord.GetCenterPlaneOffsetLocation(TileRadius);

			// Check if the tile's center is inside the bounds.
			//    For minimaps, a center-point check is fast and generally sufficient.
			if (PixelBounds.IsInside(TileCenter))
			{
				OutCoordinates.AddUnique(CurrentCoord);
			}
		}
	}

	return OutCoordinates.Num() > OriginalNum;
}


bool FHexGridCoordinateList::GenerateHexCoordinatesInRegion(FHexGridCoordinateList& OutCoordinates) const
{
	if (List.Num() == 0)
	{
		return false;
	}

	const int OriginalNum = OutCoordinates.Num();

	// Find the min/max Q and R values from the input list.
	int MinQ = TNumericLimits<int>::Max();
	int MaxQ = TNumericLimits<int>::Min();
	int MinR = TNumericLimits<int>::Max();
	int MaxR = TNumericLimits<int>::Min();

	for (const FHexGridCoordinate& Coord : List)
	{
		MinQ = FMath::Min(MinQ, Coord.Q);
		MaxQ = FMath::Max(MaxQ, Coord.Q);
		MinR = FMath::Min(MinR, Coord.R);
		MaxR = FMath::Max(MaxR, Coord.R);
	}

	// Loop over this rectangular (parallelogram in hex-space) area
	for (int q = MinQ; q <= MaxQ; ++q)
	{
		for (int r = MinR; r <= MaxR; ++r)
		{
			// Add the coordinate to the output list.
			// AddUnique to prevent duplicates if OutCoordinates already had items.
			OutCoordinates.AddUnique(FHexGridCoordinate(q, r));
		}
	}

	return OutCoordinates.Num() > OriginalNum;
}


FRotator FHexGridCoordinate::GetAbsoluteRotationFromDirection(const EHexGridDirection Direction)
{
	return FHexGridCoordinate::c_Zero.GetNeighbor(Direction).GetCenterWorldOffsetLocation(UMathTypes::c_One).Rotation();
}


EHexGridDirection FHexGridCoordinate::GetNearestDirectionBetweenCoordinates(
	FHexGridCoordinate const& FromCoordinate,
	FHexGridCoordinate const& ToCoordinate
)
{
	if (FromCoordinate == ToCoordinate)
	{
		L_WARN(HexGrid, "direction from one coordinate to the same coordinate is undefined -- returning north");
		return EHexGridDirection::north;
	}

	const FHexGridCoordinate Differential = ToCoordinate - FromCoordinate;
	const FVector DifferentialOffset = Differential.GetCenterWorldOffsetLocation(UMathTypes::c_One);
	const FRotator DifferentialOffsetRotation = DifferentialOffset.Rotation();

	EHexGridDirection NearestDirection = EHexGridDirection::north;
	float SmallestAngleDifference = TNumericLimits<float>::Max();
	FOR_FORWARD(i, UMCommon::GetEnumSize<EHexGridDirection>())
	{
		const EHexGridDirection DirectionIterator = static_cast<EHexGridDirection>(i);
		const float DirectionYawIterator = FHexGridCoordinate::GetAbsoluteYawFromDirection(DirectionIterator);
		const float DirectionAngleDifference = FMath::Abs(
			FMath::FindDeltaAngleDegrees(
				DirectionYawIterator,
				DifferentialOffsetRotation.Yaw
			)
		);
		if (DirectionAngleDifference < SmallestAngleDifference)
		{
			NearestDirection = DirectionIterator;
			SmallestAngleDifference = DirectionAngleDifference;
		}
	}


	return NearestDirection;
}


int FHexGridTileRegister::GetNumberOfPresentActors() const
{
	return PresentActors.Num();
}


int FHexGridTileRegister::GetNumberOfPresentActorsOfClass(UClass* ActorClass) const
{
	const int NumberOfActors = PresentActors.Num();
	int ValidActorsFound = 0;
	FOR_FORWARD(i, NumberOfActors)
	{
		AActor* PresentActor = UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(PresentActors[i]);
		if (UMCommon::IsUObjectPointerSafe(PresentActor) &&
			PresentActor->IsA(ActorClass))
		{
			ValidActorsFound += 1;
		}
	}


	return ValidActorsFound;
}


int FHexGridTileRegister::GetAllPresentActors(TArray<AActor*>& OutPresentActors) const
{
	const int NumberOfActors = PresentActors.Num();
	OutPresentActors.Reserve(OutPresentActors.Num() + NumberOfActors);

	int ValidActorsFound = 0;
	FOR_FORWARD(i, NumberOfActors)
	{
		AActor* PresentActor = UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(PresentActors[i]);
		if (UMCommon::IsUObjectPointerSafe(PresentActor))
		{
			OutPresentActors.Add(PresentActor);
			ValidActorsFound += 1;
		}
	}


	return ValidActorsFound;
}


int FHexGridTileRegister::GetAllPresentActorsOfClass(UClass* ActorClass, TArray<AActor*>& OutPresentActors) const
{
	const int NumberOfActors = PresentActors.Num();
	OutPresentActors.Reserve(OutPresentActors.Num() + NumberOfActors);

	int ValidActorsFound = 0;

	FOR_FORWARD(i, NumberOfActors)
	{
		AActor* PresentActor = UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(PresentActors[i]);
		if (UMCommon::IsUObjectPointerSafe(PresentActor) &&
			PresentActor->IsA(ActorClass))
		{
			OutPresentActors.Add(PresentActor);

			ValidActorsFound += 1;
		}
	}


	return ValidActorsFound;
}


bool FHexGridTileRegister::AreActorsPresent() const
{
	FOR_FORWARD(i, PresentActors.Num())
	{
		AActor* PresentActor = UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(PresentActors[i]);
		if (UMCommon::IsUObjectPointerSafe(PresentActor))
		{
			return true;
		}
	}


	return false;
}


void FHexGridTileRegister::AddPresentActors(TArray<AActor*> const& InPresentActors)
{
	PresentActors.Append(InPresentActors);


	return;
}


int FHexGridTileRegister::AddPresentActor(AActor* InPresentActor)
{
	return PresentActors.Add(InPresentActor);
}


int FHexGridTileRegister::RemovePresentActor(AActor* InPresentActor)
{
	return PresentActors.Remove(InPresentActor);
}


void FHexGridTileRegister::SetPresentActors(TArray<AActor*> const& NewPresentActors)
{
	PresentActors.Reset(NewPresentActors.Num());
	PresentActors.Append(NewPresentActors);


	return;
}


bool FHexGridTileRegister::IsAnyActorOfClassPresent(UClass* ActorClass) const
{
	return UMCommon::IsUObjectPointerSafe(GetFirstPresentActorOfClass(ActorClass));
}


AActor* FHexGridTileRegister::GetFirstPresentActorOfClass(UClass* ActorClass) const
{
	FOR_FORWARD(i, PresentActors.Num())
	{
		AActor* PresentActor = UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(PresentActors[i]);
		if (UMCommon::IsUObjectPointerSafe(PresentActor) &&
			PresentActor->IsA(ActorClass))
		{
			return PresentActor;
		}
	}


	return nullptr;
}


FString UHexGridTypes::GraphAStarResultToString(const EGraphAStarResult Result)
{
	switch (Result)
	{

	case EGraphAStarResult::GoalUnreachable:
		return FString(TEXT("GoalUnreachable"));

	case EGraphAStarResult::InfiniteLoop:
		return FString(TEXT("InfiniteLoop"));

	case EGraphAStarResult::SearchFail:
		return FString(TEXT("SearchFail"));

	case EGraphAStarResult::SearchSuccess:
		return FString(TEXT("SearchSuccess"));


	default:
		L_ERR(HexGrid, "default on graph a-star result: '{0}'", Result);
		return FString(TEXT(""));
	}
}


bool FHexGridTileMap::ContainsTile(FHexGridCoordinate const& InCoordinate) const
{
	return UMCommon::IsValidIndex(TileTypeRegistries, GetTileTypeIndexFromCoordinate(InCoordinate));
}


int FHexGridTileMap::GetTileTypeIndexFromCoordinate(FHexGridCoordinate const& InCoordinate) const
{
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		if (TileTypeRegistries[i].Tiles.Contains(InCoordinate))
		{
			return i;
		}
	}


	return -1;
}


int FHexGridTileMap::GetTileCoordinates(FHexGridCoordinateList& OutTileCoordinates) const
{
	// Calculate the total number of tiles across all registries first
	int TotalTiles = 0;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		TotalTiles += TileTypeRegistries[i].Tiles.Num();
	}

	// Pre-allocate the exact memory needed in the output array. 
	// This prevents the array from resizing itself while we add elements.
	OutTileCoordinates.List.Reserve(OutTileCoordinates.List.Num() + TotalTiles);

	// Iterate and add the keys directly
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			// Bypass the middleman array and inject the key directly into the output
			OutTileCoordinates.List.Add(TilePair.Key);
		}
	}

	return TotalTiles;
}


int FHexGridTileMap::GetTiles(TArray<FHexGridTile>& OutTiles) const
{
	// Calculate the total number of tiles to pre-allocate
	int Total = 0;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		Total += TileTypeRegistries[i].Tiles.Num();
	}

	// Pre-allocate the exact memory needed
	OutTiles.Reserve(OutTiles.Num() + Total);

	// Iterate and add the tiles directly
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			// Construct the tile in-place using the Key and registry index
			OutTiles.Add(FHexGridTile(TilePair.Key, i));
		}
	}

	return Total;
}


bool FHexGridTileMap::RegisterTile(FHexGridTile const& InTile, const int HismInstanceId, TArray<AActor*> InPresentActors)
{
	if (IsValidTileTypeIndex(InTile.TileTypeIndex) == false)
	{
		L_ERR(HexGrid, "tile type index '{0}' is invalid", InTile.TileTypeIndex);
		return false;
	}

	if (ContainsTile(InTile.Coordinate))
	{
		L_WARN(HexGrid, "tile at coordinate '{0}' already exists in the map", InTile.Coordinate);
		return false;
	}

	TileTypeRegistries[InTile.TileTypeIndex].Tiles.Add(
		InTile.Coordinate,
		FHexGridTileRegister(HismInstanceId, InPresentActors)
	);


	return true;
}


bool FHexGridTileMap::ContainsActor(const AActor* InActor) const
{
	if (UMCommon::IsUObjectPointerSafe(InActor) == false)
	{
		L_ERR(CivCraft, "InActor is null");
		return false;
	}

	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			AActor* FoundActor = nullptr;
			if (TilePair.Value.IsActorPresent(InActor))
			{
				return true;
			}
		}
	}
	

	return false;
}


bool FHexGridTileMap::FindActorCoordinates(const AActor* InActor, FHexGridCoordinateList& OutCoordinateList) const
{
	if (UMCommon::IsUObjectPointerSafe(InActor) == false)
	{
		L_ERR(CivCraft, "InActor is null");
		return false;
	}

	bool bDidFindActor = false;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			AActor* FoundActor = nullptr;
			if (TilePair.Value.IsActorPresent(InActor))
			{
				OutCoordinateList.Add(TilePair.Key);

				bDidFindActor = true;
			}
		}
	}


	return bDidFindActor;
}


int FHexGridTileMap::RemoveTile(FHexGridCoordinate const& TileCoordinate)
{
	const int TileTypeIndex = GetTileTypeIndexFromCoordinate(TileCoordinate);
	if (UMCommon::IsValidIndex(TileTypeRegistries, TileTypeIndex) == false)
	{
		return false;
	}


	return TileTypeRegistries[TileTypeIndex].Tiles.Remove(TileCoordinate);
}


int FHexGridTileMap::GetAllPresentActors(TArray<AActor*>& OutArray) const
{
	int Total = 0;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			Total += TilePair.Value.GetAllPresentActors(OutArray);
		}
	}


	return Total;
}


int FHexGridTileMap::GetAllPresentActorsOfClass(UClass* ActorClass, TArray<AActor*>& OutArray) const
{
	int Total = 0;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			Total += TilePair.Value.GetAllPresentActorsOfClass(ActorClass, OutArray);
		}
	}


	return Total;
}


bool FHexGridTileMap::IsValidTileTypeIndex(const int TileTypeIndex) const
{
	return UMCommon::IsValidIndex(TileTypeRegistries, TileTypeIndex);
}


int FHexGridTileMap::GetTileTypeIndex(const UHexGridTileTypeAsset* TileType) const
{
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		if (TileTypeRegistries[i].TileType == TileType)
		{
			return i;
		}
	}


	return -1;
}


void FHexGridTileMap::GenerateTileTypeList(TArray<const UHexGridTileTypeAsset*>& OutTileTypes) const
{
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		OutTileTypes.Add(TileTypeRegistries[i].TileType);
	}


	return;
}


void FHexGridTileMap::GenerateTileTypeIndexMap(TMap<FHexGridCoordinate, int>& OutGridTileTypeMap) const
{
	// Calculate total tiles to pre-allocate the map
	int TotalTiles = 0;
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		TotalTiles += TileTypeRegistries[i].Tiles.Num();
	}

	// Reserve the memory in the map to prevent dynamic rehashing
	OutGridTileTypeMap.Reserve(TotalTiles);

	// Populate directly
	FOR_FORWARD(i, TileTypeRegistries.Num())
	{
		FOR_MAP_CONST(TilePair, TileTypeRegistries[i].Tiles)
		{
			OutGridTileTypeMap.Add(TilePair.Key, i);
		}
	}


	return;
}


void FHexGridTileGroupMap::RegisterTileGroup(FName const& TileGroupName)
{
	TileGroups.Add(TileGroupName, FHexGridCoordinateList());

	return;
}


int FHexGridTileGroupMap::GetTileGroup(FName const& TileGroupName, FHexGridCoordinateList& OutTileCoordinates) const
{
	if (IsValidTileGroupName(TileGroupName) == false)
	{
		L_ERR(HexGrid, "invalid tile group name '{0}'", TileGroupName);
		return 0;
	}

	OutTileCoordinates.Append(TileGroups[TileGroupName].List);


	return TileGroups[TileGroupName].List.Num();
}


void FHexGridTileGroupMap::SetTileGroup(FName const& TileGroupName, FHexGridCoordinateList const& TileCoordinates)
{
	if (IsValidTileGroupName(TileGroupName) == false)
	{
		RegisterTileGroup(TileGroupName);
	}

	TileGroups[TileGroupName].Reset();
	TileGroups[TileGroupName].Reserve(TileCoordinates.Num());
	TileGroups[TileGroupName].Append(TileCoordinates);


	return;
}


void FHexGridTileGroupMap::ClearTileGroup(FName const& TileGroupName)
{
	if (IsValidTileGroupName(TileGroupName) == false)
	{
		RegisterTileGroup(TileGroupName);
	}

	TileGroups[TileGroupName].List.Reset();


	return;
}


void FHexGridData::GetTiles(TArray<FHexGridTile>& OutTiles) const
{
	OutTiles.Reserve(OutTiles.Num() + TileMap.Num());

	FOR_MAP_CONST(TilePair, TileMap)
	{
		// TilePair.Key is the Coordinate, TilePair.Value is the Map Value (int)
		OutTiles.Add(FHexGridTile(TilePair.Key, TilePair.Value));
	}


	return;
}


void FHexGridData::GetTileCoordinatesOfType(const int TypeIndex, FHexGridCoordinateList& OutCoordinates) const
{
	FOR_MAP_CONST(TilePair, TileMap)
	{
		if (TilePair.Value == TypeIndex)
		{
			OutCoordinates.Add(TilePair.Key);
		}
	}


	return;
}


void FHexGridData::GetTileCoordinatesOfType(
	TSoftObjectPtr<const UHexGridTileTypeAsset> const& Type,
	FHexGridCoordinateList& OutCoordinates
) const
{
	const int TileTypeIndex = GetTileTypeIndex(Type);
	if (UMCommon::IsValidIndex(GridProperties.TileTypes, TileTypeIndex) == false)
	{
		L_ERR(HexGridEditor, "tile type '{0}' not found", Type);
		return;
	}

	GetTileCoordinatesOfType(TileTypeIndex, OutCoordinates);


	return;
}


int FHexGridData::GetTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& Type) const
{
	FOR_FORWARD(i, GridProperties.TileTypes.Num())
	{
		if (GridProperties.TileTypes[i] == Type)
		{
			return i;
		}
	}


	return -1;
}


TSoftObjectPtr<const UHexGridTileTypeAsset> FHexGridData::GetTileTypeFromCoordinate(
	FHexGridCoordinate const& Coordinate
) const
{
	if (TileMap.Contains(Coordinate) == false)
	{
		return nullptr;
	}

	const int TileTypeIndex = TileMap[Coordinate];
	if (UMCommon::IsValidIndex(GridProperties.TileTypes, TileTypeIndex) == false)
	{
		return nullptr;
	}


	return GridProperties.TileTypes[TileTypeIndex];
}


FVector FHexGridData::GetTileCoordinateWorldOffsetLocation(FHexGridCoordinate const& Coordinate)
{
	return Coordinate.GetCenterWorldOffsetLocation(GridProperties.TileRadius);
}


EHexGridTileVisibility FFogOfWarPerspectiveMap::GetTileVisibilityForPlayer(
	const int PlayerIndex,
	FHexGridCoordinate const& Coordinate
) const
{
	if (PlayerPerspectives.Contains(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return EHexGridTileVisibility::hidden;
	}


	return PlayerPerspectives[PlayerIndex].GetTileVisibility(Coordinate);
}


void FFogOfWarPerspectiveMap::SetTileVisibilityForPlayer(
	const int PlayerIndex,
	FHexGridCoordinate const& Coordinate,
	const EHexGridTileVisibility VisibilityState
)
{
	if (PlayerPerspectives.Contains(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return;
	}

	PlayerPerspectives[PlayerIndex].SetTileVisibility(Coordinate, VisibilityState);


	return;
}


void FFogOfWarPerspectiveMap::GetListedTilesWithVisibilityForPlayer(
	const int PlayerIndex,
	const EHexGridTileVisibility VisibilityState,
	FHexGridCoordinateList& OutCoordinates
) const
{
	if (PlayerPerspectives.Contains(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return;
	}

	PlayerPerspectives[PlayerIndex].GetListedTilesWithVisibility(VisibilityState, OutCoordinates);


	return;
}


float UHexGridTypes::GetVisibilityScalarFromState(const EHexGridTileVisibility State)
{
	switch (State)
	{

	case EHexGridTileVisibility::hidden:
		return 0.f;

	case EHexGridTileVisibility::partial:
		return 0.5f;

	case EHexGridTileVisibility::visible:
		return 1.f;


	default:
		L_ERR(HexGrid, "default on visibility state '{0}'", State);
		return 0.f;
	}
}


void FHexGridCoordinateList::GetCenterWorldOffsetLocations(const float TileRadius, TArray<FVector>& OutWorldLocations) const
{
	FOR_FORWARD(i, List.Num())
	{
		OutWorldLocations.Add(List[i].GetCenterWorldOffsetLocation(TileRadius));
	}


	return;
}


void FHexGridCoordinateList::GetCornerWorldOffsetLocations(const float TileRadius, TArray<FVector>& OutWorldLocations) const
{
	FOR_FORWARD(i, List.Num())
	{
		OutWorldLocations.Append(List[i].GetCornerWorldOffsetLocations(TileRadius));
	}


	return;
}


int FHexGridProperties::GetTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const
{
	if (TileType.IsNull())
	{
		return -1;
	}

	FOR_FORWARD(i, TileTypes.Num())
	{
		if (TileTypes[i] == TileType)
		{
			return i;
		}
	}


	return -1;
}


void FHexGridProperties::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	FOR_FORWARD(i, TileTypes.Num())
	{
		IAssetProviderInterface::CollectPreloadAsset(TileTypes[i], OutAssets);
	}


	return;
}


void FFogOfWarSetupProperties::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	IAssetProviderInterface::CollectPreloadAsset(ComponentClass, OutAssets);


	return;
}


void FMinimapIndicatorProperties::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	IAssetProviderInterface::CollectPreloadAsset(Texture, OutAssets);


	return;
}


void FHexGridCoordinateList::GenerateMirroredCoordinatesAcrossAxis_Q(FHexGridCoordinateList& OutCoordinates) const
{
	FOR_FORWARD(i, List.Num())
	{
		OutCoordinates.Add(List[i].MirrorAcrossAxisQ());
	}


	return;
}


void FHexGridCoordinateList::GenerateMirroredCoordinatesAcrossAxis_R(FHexGridCoordinateList& OutCoordinates) const
{
	FOR_FORWARD(i, List.Num())
	{
		OutCoordinates.Add(List[i].MirrorAcrossAxisR());
	}


	return;
}


void FHexGridCoordinateList::GenerateMirroredCoordinatesAcrossAxis_S(FHexGridCoordinateList& OutCoordinates) const
{
	FOR_FORWARD(i, List.Num())
	{
		OutCoordinates.Add(List[i].MirrorAcrossAxisS());
	}


	return;
}


int FHexGridCoordinateList::GetDistanceOfFurthestCoordinateFromCenter() const
{
	int FurthestDistance = 0;
	FOR_FORWARD(i, List.Num())
	{
		const int CoordinateDistance = FHexGridCoordinate::Distance(FHexGridCoordinate::c_Zero, List[i]);
		if (CoordinateDistance > FurthestDistance)
		{
			FurthestDistance = CoordinateDistance;
		}
	}


	return FurthestDistance;
}


FBox2D FHexGridCoordinateList::GetBounds(const float TileRadius) const
{
	TArray<FVector> CornerLocations;
	GetCornerWorldOffsetLocations(TileRadius, CornerLocations);

	if (CornerLocations.Num() <= 0)
	{
		return FBox2D();
	}

	FVector2D MaximumExtent = FVector2D(TNumericLimits<double>::Lowest());
	FVector2D MinimumExtent = FVector2D(TNumericLimits<double>::Max());
	FOR_FORWARD(i, CornerLocations.Num())
	{
		// maximums
		if (CornerLocations[i].X > MaximumExtent.X)
		{
			MaximumExtent.X = CornerLocations[i].X;
		}
		if (CornerLocations[i].Y > MaximumExtent.Y)
		{
			MaximumExtent.Y = CornerLocations[i].Y;
		}

		// minimums
		if (CornerLocations[i].X < MinimumExtent.X)
		{
			MinimumExtent.X = CornerLocations[i].X;
		}
		if (CornerLocations[i].Y < MinimumExtent.Y)
		{
			MinimumExtent.Y = CornerLocations[i].Y;
		}
	}

	return FBox2D(MinimumExtent, MaximumExtent);
}


FHexGridCoordinate FHexGridCoordinateList::FindApproximateCenterCoordinate() const
{
	const int CoordinateCount = List.Num();
	if (CoordinateCount <= 0)
	{
		// return origin if the list is empty
		return FHexGridCoordinate(0, 0);
	}

	// sum the q and r components
	float TotalQ = 0.f;
	float TotalR = 0.f;

	FOR_FORWARD(i, List.Num())
	{
		TotalQ += List[i].Q;
		TotalR += List[i].R;
	}

	// calculate the floating-point average for q, r, and s
	const float AverageQ = TotalQ / CoordinateCount;
	const float AverageR = TotalR / CoordinateCount;
	// calculate s from the averages, since s = -q - r
	const float AverageS = -AverageQ - AverageR;

	// Round the floating-point averages to the nearest integer hex coordinate
	int RoundedQ = FMath::RoundToInt(AverageQ);
	int RoundedR = FMath::RoundToInt(AverageR);
	int RoundedS = FMath::RoundToInt(AverageS);

	// correct the rounded values to maintain the q + r + s = 0 constraint.
	// this is the most important step for accuracy.
	const float DiffQ = FMath::Abs(static_cast<float>(RoundedQ) - AverageQ);
	const float DiffR = FMath::Abs(static_cast<float>(RoundedR) - AverageR);
	const float DiffS = FMath::Abs(static_cast<float>(RoundedS) - AverageS);

	if (DiffQ > DiffR && DiffQ > DiffS)
	{
		// q had the biggest rounding error, so reconstruct it from the others.
		RoundedQ = -RoundedR - RoundedS;
	}
	else if (DiffR > DiffS)
	{
		// r had the biggest rounding error.
		RoundedR = -RoundedQ - RoundedS;
	}


	return FHexGridCoordinate(RoundedQ, RoundedR);
}


void FFogOfWarPerspectiveData::GetListedTilesWithVisibility(
	const EHexGridTileVisibility TileVisibility,
	FHexGridCoordinateList& OutCoordinates
) const
{
	if (TileVisibility == EHexGridTileVisibility::hidden)
	{
		L_WARN(HexGrid, "called with EHexGridTileVisibility::hidden -- this may not capture all existing tiles that should be hidden");
	}

	FOR_MAP_CONST(Pair, TileVisibilityMap)
	{
		if (Pair.Value == TileVisibility)
		{
			OutCoordinates.Add(Pair.Key);
		}
	}


	return;
}


void FFogOfWarPerspectiveData::SetTileVisibility(FHexGridCoordinate const& Coordinate, const EHexGridTileVisibility TileVisibility)
{
	TileVisibilityMap.Add(Coordinate, TileVisibility);


	return;
}


FHexGridTileVisibilityGroupMap::FHexGridTileVisibilityGroupMap(
	TMap<FHexGridCoordinate, EHexGridTileVisibility> const& InTileVisibilityMap
)
	: FHexGridTileVisibilityGroupMap()
{
	FOR_MAP_CONST(TilePair, InTileVisibilityMap)
	{
		VisibilityGroupMap.FindOrAdd(TilePair.Value).List.Add(TilePair.Key);
	}
}