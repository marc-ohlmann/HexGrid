// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphAStar.h"
#include "HexGridTypes.generated.h"


class AActor;
class UStaticMesh;
class UHexGridTileTypeAsset;
class UMaterialInterface;
class UHexGridFogOfWarComponent;

struct FHexGridCoordinateList;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridTransformChanged, FHexGridTransform const& /*OldTransform*/, FHexGridTransform const& /*NewTransform*/);
DECLARE_MULTICAST_DELEGATE(FOnPathTraversalStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTraversalFinished, bool /*bDidReachEndOfPath*/);


// enumerated directions for a flat-top hexagonal grid
UENUM(BlueprintType)
enum class EHexGridDirection : uint8
{
	north			UMETA(DisplayName = "North"),
	northeast		UMETA(DisplayName = "Northeast"),
	southeast		UMETA(DisplayName = "Southeast"),
	south			UMETA(DisplayName = "South"),
	southwest		UMETA(DisplayName = "Southwest"),
	northwest		UMETA(DisplayName = "Northwest"),
};


// enumerated shapes that can be constructed with a hexagonal grid
UENUM(BlueprintType)
enum class EHexGridShape : uint8
{
	hexagon				UMETA(DisplayName = "Hexagon"),
	rectangle			UMETA(DisplayName = "Rectangle"),
	parallelogram		UMETA(DisplayName = "Parallelogram"),
	triangle_s_pos		UMETA(DisplayName = "Triangle (S+)"),
	triangle_s_neg		UMETA(DisplayName = "Triangle (S-)"),
	triangle_q_pos		UMETA(DisplayName = "Triangle (Q+)"),
	triangle_q_neg		UMETA(DisplayName = "Triangle (Q-)"),
	triangle_r_pos		UMETA(DisplayName = "Triangle (R+)"),
	triangle_r_neg		UMETA(DisplayName = "Triangle (R-)"),
};


// visibility state of a tile
UENUM(BlueprintType)
enum class EHexGridTileVisibility : uint8
{
	hidden				UMETA(DisplayName = "Hidden"),
	partial				UMETA(DisplayName = "Partial"),
	visible				UMETA(DisplayName = "Visible"),
};


// state of a hex grid's tile generator
UENUM(BlueprintType)
enum class EHexGridTileGenerationState : uint8
{
	inactive,
	degeneration,
	generation,
};


// a coordinate for a flat-top hexagonal grid using axial coordinates
// only needs 2 axes (Q: horizontal, R: diagonal) 
// but a third axis (S: orthogonal to R) coordinate can be calculated 
// using the constraint Q + S + R = 0 or S = -Q - R
// 
//		 flat-top
//		  _______
//		 /   q   \
//      /         \
//      \ (s)   r /
//		 \_______/
//
USTRUCT(BlueprintType)
struct HEXGRIDPLUGIN_API FHexGridCoordinate
{
	GENERATED_BODY()

public:

	FHexGridCoordinate()
		: Q(0)
		, R(0)
	{
	}

	FHexGridCoordinate(const int InQ, const int InR)
		: Q(InQ)
		, R(InR)
	{
	}

	static const int c_HexNumberOfSides;

	// Q-Axis runs vertically from the top to the bottom
	// positive direction is vertically to the bottom
	// negative direction is vertically to the top
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Q;

	// R-Axis runs diagonally from bottom left to top right
	// positive direction is diagonally to the upper right
	// negative direction is diagonally to the lower left
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int R;

	// S-Axis runs diagonally from bottom right to top left
	// positive direction is diagonally to the upper left
	// negative direction is diagonally to the lower right
	// this coordinate isnt stored, instead it is calculated using the constraint: Q + R + S = 0
	// therefore S = -Q - R
	int S() const { return -Q - R; }

	// hash function that allows coordinates to be used in a TMap<> or TSet<>
	FORCEINLINE friend uint32 GetTypeHash(const FHexGridCoordinate& data)
	{
		return HashCombine(::GetTypeHash(data.Q), ::GetTypeHash(data.R));
	}

	bool operator==(FHexGridCoordinate const& Other) const
	{
		return (*this).Q == Other.Q && (*this).R == Other.R;
	}

	bool operator!=(FHexGridCoordinate const& Other) const
	{
		return !((*this) == Other);
	}

	// add two hex grid coordinates
	FHexGridCoordinate operator+(FHexGridCoordinate const& Other) const
	{
		return FHexGridCoordinate((*this).Q + Other.Q, (*this).R + Other.R);
	}

	// add two hex grid coordinates
	FHexGridCoordinate& operator+=(FHexGridCoordinate const& Other)
	{
		return (*this) = (*this) + Other;
	}

	// scale a hex grid coordinate
	FHexGridCoordinate operator*(const int Scalar) const
	{
		return FHexGridCoordinate((*this).Q * Scalar, (*this).R * Scalar);
	}

	// scale a hex grid coordinate
	FHexGridCoordinate& operator*=(const int Scalar)
	{
		return (*this) = (*this) * Scalar;
	}

	// inverse scale a hex grid coordinate
	FHexGridCoordinate operator/(const int InverseScalar) const
	{
		return FHexGridCoordinate((*this).Q / InverseScalar, (*this).R / InverseScalar);
	}

	// inverse scale a hex grid coordinate
	FHexGridCoordinate& operator/=(const int Scalar)
	{
		return (*this) = (*this) / Scalar;
	}

	// subtract two hex grid coordinates
	FHexGridCoordinate operator-(FHexGridCoordinate const& Other) const
	{
		return (*this) + (Other * -1);
	}

	// subtract two hex grid coordinates
	FHexGridCoordinate& operator-=(FHexGridCoordinate const& Other)
	{
		return (*this) = (*this) - Other;
	}

	// get distance of this coordinate from the center tile
	int Length() const
	{
		return (FMath::Abs(Q) + FMath::Abs(R) + FMath::Abs(S())) / 2;
	}

	// get distance, in hex tiles, between two coordinates on the grid
	static int Distance(FHexGridCoordinate const& A, FHexGridCoordinate const& B)
	{
		return (A - B).Length();
	}

	// the root tile of a grid with coordinates (0, 0)
	static const FHexGridCoordinate c_Zero;

	// unit length coordinate whose offset from the center is in the north direction
	static const FHexGridCoordinate c_HexGridDirection_N;

	// unit length coordinate whose offset from the center is in the northeast direction
	static const FHexGridCoordinate c_HexGridDirection_NE;

	// unit length coordinate whose offset from the center is in the southeast direction
	static const FHexGridCoordinate c_HexGridDirection_SE;

	// unit length coordinate whose offset from the center is in the south direction
	static const FHexGridCoordinate c_HexGridDirection_S;

	// unit length coordinate whose offset from the center is in the southwest direction
	static const FHexGridCoordinate c_HexGridDirection_SW;

	// unit length coordinate whose offset from the center is in the northwest direction
	static const FHexGridCoordinate c_HexGridDirection_NW;

	// array of neighboring vector coordinates 
	// moves clock-wise starting from north neighbour
	// index 0: North
	// index 1: North East
	// index 2: South East
	// index 3: South
	// index 4: South West
	// index 5: North West
	static const FHexGridCoordinate c_Neighbors[];

	// get a direction on a hex grid from a given starting direction after applying the given number of clockwise rotations
	static EHexGridDirection GetDirectionAfterClockwiseRotations(
		const EHexGridDirection StartingDirection,
		const int ClockwiseRotations
	)
	{
		return static_cast<EHexGridDirection>(
			(FHexGridCoordinate::c_HexNumberOfSides +
				((static_cast<uint8>(StartingDirection) + ClockwiseRotations)
					% FHexGridCoordinate::c_HexNumberOfSides))
			% FHexGridCoordinate::c_HexNumberOfSides
			);
	}

	// get the number of clockwise rotations required to go from north to another direction
	static int GetClockwiseRotationsFromNorthToDirection(const EHexGridDirection Direction);

	// get the number of counter clockwise rotations required to go from north to another direction
	static int GetCounterClockwiseRotationsFromNorthToDirection(const EHexGridDirection Direction);

	// get the minimum number of rotations required to go from north to another direction
	// positive rotations indicates clockwise rotations
	// negative rotations indicates counter-clockwise rotations
	static int GetMinimumRotationsFromNorthToDirection(const EHexGridDirection Direction);

	// get the number of clockwise rotations required to go from one direction to another direction
	static int GetClockwiseRotationsBetweenDirections(
		const EHexGridDirection FromDirection,
		const EHexGridDirection ToDirection
	);

	// get the number of counter clockwise rotations required to go from one direction to another direction
	static int GetCounterClockwiseRotationsBetweenDirections(
		const EHexGridDirection FromDirection,
		const EHexGridDirection ToDirection
	);

	// get the minimum number of clockwise rotations required to go from one direction to another
	// positive rotations indicates clockwise rotations
	// negative rotations indicates counter-clockwise rotations
	static int GetMinimumRotationsBetweenDirections(
		const EHexGridDirection FromDirection,
		const EHexGridDirection ToDirection);

	// get the nearest direction between two given coordinates
	// since two coordinates may not be neighbors, the nearest direction may not face that coordinate directly
	static EHexGridDirection GetNearestDirectionBetweenCoordinates(
		FHexGridCoordinate const& FromCoordinate,
		FHexGridCoordinate const& ToCoordinate
	);

	// get the nearest direction from this coordinate to the given coordinate
	// since two coordinates may not be neighbors, the nearest direction may not face that coordinate directly
	EHexGridDirection GetNearestDirectionTowardsCoordinate(FHexGridCoordinate const& ToCoordinate) const
	{
		return GetNearestDirectionBetweenCoordinates(*this, ToCoordinate);
	}

	// get the reverse direction for a given direction
	static EHexGridDirection GetReverseDirection(const EHexGridDirection Direction)
	{
		return GetDirectionAfterClockwiseRotations(Direction, 3);
	}

	// get the adjacent directions for a given direction
	static TArray<EHexGridDirection> GetForwardAdjacentDirections(const EHexGridDirection Direction)
	{
		TArray<EHexGridDirection> Out;
		Out.Add(GetDirectionAfterClockwiseRotations(Direction, 1));
		Out.Add(GetDirectionAfterClockwiseRotations(Direction, -1));


		return Out;
	}

	// get the adjacent directions for the reverse of a given direction
	static TArray<EHexGridDirection> GetBackwardAdjacentDirections(const EHexGridDirection Direction)
	{
		return GetForwardAdjacentDirections(GetReverseDirection(Direction));
	}

	// check if the given test direction is forward relative to the given forward direction
	static bool IsForwardRelativeToDirection(const EHexGridDirection ForwardDirection, const EHexGridDirection TestDirection)
	{
		TArray<EHexGridDirection> AdjacentDirections = GetForwardAdjacentDirections(ForwardDirection);
		return TestDirection == ForwardDirection || AdjacentDirections.Contains(TestDirection);
	}

	// check if the given test direction is backward relative to the given forward direction
	static bool IsBackwardRelativeToDirection(const EHexGridDirection ForwardDirection, const EHexGridDirection TestDirection)
	{
		TArray<EHexGridDirection> AdjacentDirections = GetBackwardAdjacentDirections(ForwardDirection);
		return TestDirection == GetReverseDirection(ForwardDirection) || AdjacentDirections.Contains(TestDirection);
	}

	// get coordinate of a neighboring tile in the given direction
	FHexGridCoordinate GetNeighbor(const EHexGridDirection Direction) const
	{
		return (*this) + FHexGridCoordinate::c_Neighbors[static_cast<uint8>(Direction)];
	}

	// get coordinate of a neighboring tile to the north
	FHexGridCoordinate North() const
	{
		return GetNeighbor(EHexGridDirection::north);
	}

	// get coordinate of a neighboring tile to the south
	FHexGridCoordinate South() const
	{
		return GetNeighbor(EHexGridDirection::south);
	}

	// get coordinate of a neighboring tile to the northwest
	FHexGridCoordinate Northwest() const
	{
		return GetNeighbor(EHexGridDirection::northwest);
	}

	// get coordinate of a neighboring tile to the southwest
	FHexGridCoordinate Southwest() const
	{
		return GetNeighbor(EHexGridDirection::southwest);
	}

	// get coordinate of a neighboring tile to the northeast
	FHexGridCoordinate Northeast() const
	{
		return GetNeighbor(EHexGridDirection::northeast);
	}

	// get coordinate of a neighboring tile to the southeast
	FHexGridCoordinate Southeast() const
	{
		return GetNeighbor(EHexGridDirection::southeast);
	}

	// get the neighbors that appears above this coordinate relative to the given facing direction
	FHexGridCoordinateList GetRelativeNeighbors_Up(const EHexGridDirection FacingDirection) const;

	// get the neighbors that appears below this coordinate relative to the given facing direction
	FHexGridCoordinateList GetRelativeNeighbors_Down(const EHexGridDirection FacingDirection) const;

	// get the neighbors that appear to the left of this coordinate relative to the given facing direction
	FHexGridCoordinateList GetRelativeNeighbors_Left(const EHexGridDirection FacingDirection) const;

	// get the neighbors that appear to the right of this coordinate relative to the given facing direction
	FHexGridCoordinateList GetRelativeNeighbors_Right(const EHexGridDirection FacingDirection) const;

	// get absolute yaw for a given hex grid direction
	// does not take a grid's yaw into account
	static FRotator GetAbsoluteRotationFromDirection(const EHexGridDirection Direction);

	// get absolute yaw for a given hex grid direction
	// does not take a grid's yaw into account
	static float GetAbsoluteYawFromDirection(const EHexGridDirection Direction)
	{
		return GetAbsoluteRotationFromDirection(Direction).Yaw;
	}

	// get coordinates of every neighboring tile
	FHexGridCoordinateList GetNeighbors() const;

	// check if two hex grid tiles are neighbors to eachother
	static bool AreNeighbors(FHexGridCoordinate const& A, FHexGridCoordinate const& B)
	{
		return FHexGridCoordinate::Distance(A, B) == 1;
	}

	// check if the given hex grid tile is a neighbour to this one
	bool IsNeighbor(FHexGridCoordinate const& Other) const
	{
		return FHexGridCoordinate::AreNeighbors((*this), Other);
	}

	// get the direction between two neighboring hex grid coordinates (from A towards B)
	static EHexGridDirection GetDirectionFromNeighborsAToB(FHexGridCoordinate const& A, FHexGridCoordinate const& B);

	// get the direction between two neighboring hex grid coordinates (from A towards B)
	EHexGridDirection GetDirectionTowardsNeighbor(FHexGridCoordinate const& Other) const
	{
		return GetDirectionFromNeighborsAToB((*this), Other);
	}

	// check if the given neighboring coordinate is in the given forward direction, or adjacent to it
	bool IsNeighborInFront(const EHexGridDirection ForwardDirection, FHexGridCoordinate const& NeighborCoordinate) const;

	// check if the given neighboring coordinate is in the reverse of the given forward direction, or adjacent to it
	bool IsNeighborBehind(const EHexGridDirection ForwardDirection, FHexGridCoordinate const& NeighborCoordinate) const;

	// rotates this coordinate clockwise
	FHexGridCoordinate RotateClockwise() const
	{
		return FHexGridCoordinate(-R, -S());
	}

	// rotates this coordinate clockwise for the given number of rotations
	FHexGridCoordinate RotateClockwise(const int Rotations) const;

	// rotates this coordinate counter-clockwise
	FHexGridCoordinate RotateCounterClockwise() const
	{
		return FHexGridCoordinate(-S(), -Q);
	}

	// rotates this coordinate counter-clockwise for the given number of rotations
	FHexGridCoordinate RotateCounterClockwise(const int Rotations) const;

	// rotates this coordinate from a given direction to another
	FHexGridCoordinate RotateFromDirectionToDirection(const EHexGridDirection From, const EHexGridDirection To) const;

	// rotates this coordinate from an assumed north direction to another
	FHexGridCoordinate RotateToDirection(const EHexGridDirection Direction) const
	{
		return RotateFromDirectionToDirection(EHexGridDirection::north, Direction);
	}

	// multiplier for the tile size and Q coordinate to get the horizontal distance between two tile center points
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_QSizeToHorizontalDistanceRatio;

	// multiplier for the tile size and Q coordinate to get the vertical distance between two tile center points
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_QSizeToVerticalDistanceRatio;

	// multiplier for the tile size and R coordinate to get the vertical distance between two tile center points
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_RSizeToVerticalDistanceRatio;

	// get the 2d plane offset location from this tile and given tile size for the center of this tile
	// where tile size is half the width of a flat-top hexagon (distance from center to any corner)
	FVector2D GetCenterPlaneOffsetLocation(const float TileRadius) const;

	// get the 2d plane offset location, from the this tile and a given tile size, of each corner of this tile
	// where tile size is half the width of a flat-top hexagon (distance from center to any corner)
	// provides corner points in clockwise order starting from the corner on the rightside
	TArray<FVector2D> GetCornerPlaneOffsetLocations(const float TileRadius) const;

	// get the world offset location from this tile and given tile size for the center of this tile
	// where tile size is half the width of a flat-top hexagon (distance from center to any corner)
	FVector GetCenterWorldOffsetLocation(const float TileRadius) const;

	// get the world offset location, from the this tile and a given tile size, of each corner of this tile
	// where tile size is half the width of a flat-top hexagon (distance from center to any corner)
	// provides corner points in clockwise order starting from the corner on the rightside
	TArray<FVector> GetCornerWorldOffsetLocations(const float TileRadius) const; 
	
	static FHexGridCoordinate GetHexGridCoordinateFromPlaneOffsetLocation(
		const FVector2D& PixelPosition,
		const float TileRadius
	);

	// get the hex grid tile at the given world offset location from the center of the grid
	// where tile size is half the width of a flat-top hexagon (distance from center to any corner)
	static FHexGridCoordinate GetHexGridCoordinateFromWorldOffsetLocation(
		FVector const& WorldOffsetLocation,
		const float TileRadius
	);

	// multiplier for the tile size and world offset X coordinate to get the Q coordinate of a hex tile on the grid
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_XSizeToQRatio;

	// multiplier for the tile size and world offset X coordinate to get the R coordinate of a hex tile on the grid
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_XSizeToRRatio;

	// multiplier for the tile size and world offset Y coordinate to get the R coordinate of a hex tile on the grid
	// where the size of a tile is half the entire width of a tile
	// for flat top hexagonal tiles
	static const float c_YSizeToRRatio;

	// convert to printable string
	FString ToString() const
	{
		return FString::Printf(TEXT("{Q: '%d', R: '%d', S: '%d'}"), Q, R, S());
	}

	// convert to a compact string
	FString ToCompactString() const
	{
		return FString::Printf(TEXT("Q%d_R%d_S%d"), Q, R, S());
	}

	// print the coordinates to the log
	void Print() const;

	// get all coordinates within the given range of the given tile
	static bool GetCoordinatesWithinRangeOfTile(
		FHexGridCoordinate const& CenterTile,
		const int Range,
		FHexGridCoordinateList& OutCoordinates
	);

	// get all coordinates within the given range of the current tile
	bool GetCoordinatesWithinRange(const int Range, FHexGridCoordinateList& OutCoordinates) const
	{
		return FHexGridCoordinate::GetCoordinatesWithinRangeOfTile((*this), Range, OutCoordinates);
	}

	// get all coordinates on the line between two given hextiles
	static bool GetCoordinatesOnLine(
		FHexGridCoordinate const& A,
		FHexGridCoordinate const& B,
		FHexGridCoordinateList& OutCoordinates
	);

	// get all coordinates in a line between this tile and another tile
	bool GetCoordinatesBetween(FHexGridCoordinate const& Other, FHexGridCoordinateList& OutCoordinates) const
	{
		return FHexGridCoordinate::GetCoordinatesOnLine((*this), Other, OutCoordinates);
	}

	// get all coordinates that are a given distance away from the target coordinate
	static bool GetCoordinatesAtDistanceFromCoordinate(
		FHexGridCoordinate const& Target,
		const int Distance,
		FHexGridCoordinateList& OutCoordinates
	);

	// get all coordinates that are a given distance away from this coordinate
	bool GetSurroundingCoordinatesAtDistance(const int Distance, FHexGridCoordinateList& OutCoordinates) const
	{
		return FHexGridCoordinate::GetCoordinatesAtDistanceFromCoordinate(*this, Distance, OutCoordinates);
	}

	// get all surrounding coordinates in the shape of a hexagon for the target coordinate
	static bool GetCoordinatesInHexagonAroundCoordinate(
		FHexGridCoordinate const& Target,
		const int Size,
		FHexGridCoordinateList& OutCoordinates
	);

	// get all surrounding coordinates in the shape of a hexagon for this coordinate
	bool GetSurroundingCoordinatesInHexagon(const int Size, FHexGridCoordinateList& OutCoordinates) const
	{
		return FHexGridCoordinate::GetCoordinatesInHexagonAroundCoordinate(*this, Size, OutCoordinates);
	}

	// get all coordinates within a rectangle with dimensions described by offsets around the target coordinate
	// for rectangles containing the zero coordinate, left / top offsets should be negative and right / bottom offsets should be positive
	static bool GetCoordinatesInRectangleAroundCoordinate(
		FHexGridCoordinate const& Target,
		const int LeftOffset,
		const int RightOffset,
		const int TopOffset,
		const int BottomOffset,
		FHexGridCoordinateList& OutList
	);

	// get all surrounding coordinates in the shape of a rectangle for this coordinate
	bool GetSurroundingCoordinatesInRectangle(
		const int LeftOffset,
		const int RightOffset,
		const int TopOffset,
		const int BottomOffset,
		FHexGridCoordinateList& OutList) const
	{
		return FHexGridCoordinate::GetCoordinatesInRectangleAroundCoordinate(
			*this,
			LeftOffset,
			RightOffset,
			TopOffset,
			BottomOffset,
			OutList
		);
	}

	// get all coordinates within a triangle of a given size around the target coordinate
	static bool GetCoordinatesInTriangleAroundCoordinate(
		FHexGridCoordinate const& Target,
		const int Size,
		const EHexGridShape TriangleOrientation,
		FHexGridCoordinateList& OutList
	);

	// get all coordinates within a triangle of a given size around this coordinate
	bool GetSurroundingCoordinatesInTriangle(
		const int Size,
		const EHexGridShape TriangleOrientation,
		FHexGridCoordinateList& OutList
	) const
	{
		return FHexGridCoordinate::GetCoordinatesInTriangleAroundCoordinate(*this, Size, TriangleOrientation, OutList);
	}

	// get all coordinates within a parallelogram around the target coordinate
	static bool GetCoordinatesInParallelogramAroundCoordinate(
		FHexGridCoordinate const& Target,
		const int Q1,
		const int R1,
		const int Q2,
		const int R2,
		FHexGridCoordinateList& OutList
	);

	// get all surrounding coordinates in the shape of a parallelogram for this coordinate
	bool GetSurroundingCoordinatesInParallelogram(
		const int Q1,
		const int R1,
		const int Q2,
		const int R2,
		FHexGridCoordinateList& OutList) const
	{
		return FHexGridCoordinate::GetCoordinatesInParallelogramAroundCoordinate(
			*this,
			Q1,
			R1,
			Q2,
			R2,
			OutList
		);
	}

	// get all coordinates within the given shape and size around the target coordinate
	static bool GetCoordinatesInShapeAroundCoordinate(
		FHexGridCoordinate const& Target,
		const EHexGridShape Shape,
		const int ShapeSize,
		FHexGridCoordinateList& OutCoordinates
	);

	// get all surrounding coordinates in the given shape and size this coordinate
	bool GetSurroundingCoordinatesInShape(
		const EHexGridShape Shape,
		const int ShapeSize,
		FHexGridCoordinateList& OutList) const
	{
		return FHexGridCoordinate::GetCoordinatesInShapeAroundCoordinate(
			*this,
			Shape,
			ShapeSize,
			OutList
		);
	}

	// create a hex grid coordinate that is mirrored across the q-axis from the given coordinate
	static FHexGridCoordinate MirrorCoordinateAcrossAxisQ(FHexGridCoordinate const& InCoordinate)
	{
		return FHexGridCoordinate(InCoordinate.Q * -1, InCoordinate.S());
	}

	// create a hex grid coordinate that is mirrored across the q-axis from this coordinate
	FHexGridCoordinate MirrorAcrossAxisQ() const
	{
		return MirrorCoordinateAcrossAxisQ(*this);
	}

	// create a hex grid coordinate that is mirrored across the r-axis from the given coordinate
	static FHexGridCoordinate MirrorCoordinateAcrossAxisR(FHexGridCoordinate const& InCoordinate)
	{
		return FHexGridCoordinate(InCoordinate.S(), InCoordinate.R * -1);
	}

	// create a hex grid coordinate that is mirrored across the r-axis from this coordinate
	FHexGridCoordinate MirrorAcrossAxisR() const
	{
		return MirrorCoordinateAcrossAxisR(*this);
	}

	// create a hex grid coordinate that is mirrored across the s-axis from the given coordinate
	static FHexGridCoordinate MirrorCoordinateAcrossAxisS(FHexGridCoordinate const& InCoordinate)
	{
		return FHexGridCoordinate(InCoordinate.R * -1, InCoordinate.Q * -1);
	}

	// create a hex grid coordinate that is mirrored across the s-axis from this coordinate
	FHexGridCoordinate MirrorAcrossAxisS() const
	{
		return MirrorCoordinateAcrossAxisS(*this);
	}

};


// a coordinate for a flat-top hexagonal grid using axial coordinates
// that can have fractional coordinate values
// useful for lerping hex coordinates
USTRUCT()
struct HEXGRIDPLUGIN_API FFractionalHexGridCoordinate
{
	GENERATED_BODY()

public:

	FFractionalHexGridCoordinate()
		: Q(0.0)
		, R(0.0)
	{
	}

	FFractionalHexGridCoordinate(const double InQ, const double InR)
		: Q(InQ)
		, R(InR)
	{
	}

	// Q axis coordinate
	UPROPERTY()
	double Q;

	// R axis coordinate
	UPROPERTY()
	double R;

	// getter for S axis coordinate
	double S() const { return -Q - R; }

	// lerp between two hex grid coordinates
	static FFractionalHexGridCoordinate HexLerp(FHexGridCoordinate const& A, FHexGridCoordinate const& B, const double Alpha)
	{
		return FFractionalHexGridCoordinate(
			FMath::Lerp(A.Q, B.Q, Alpha),
			FMath::Lerp(A.R, B.R, Alpha)
		);
	}

	// lerp between two hex grid coordinates
	static FFractionalHexGridCoordinate HexLerp(
		FFractionalHexGridCoordinate const& A,
		FFractionalHexGridCoordinate const& B,
		const double Alpha
	)
	{
		return FFractionalHexGridCoordinate(
			FMath::Lerp(A.Q, B.Q, Alpha),
			FMath::Lerp(A.R, B.R, Alpha)
		);
	}

	// get roundex hex coordinate from this fractional coordinate
	FHexGridCoordinate Round() const
	{
		int q = FMath::RoundToNearestTiesToEven(Q);
		int r = FMath::RoundToNearestTiesToEven(R);
		int s = FMath::RoundToNearestTiesToEven(S());

		const double q_diff = FMath::Abs(q - Q);
		const double r_diff = FMath::Abs(r - R);
		const double s_diff = FMath::Abs(s - S());

		if (q_diff > r_diff && q_diff > s_diff)
		{
			q = -r - s;
		}
		else if (r_diff > s_diff)
		{
			r = -q - s;
		}


		return FHexGridCoordinate(q, r);
	}

	// a scalar applied to the fractional coordinate to nudge it slightly
	static const double c_FractionalNudgeDistance;

	// convert a 2d pixel location (relative to a (0,0) origin) to a fractional hex grid coordinate
	static FFractionalHexGridCoordinate GetFractionalHexGridCoordinateFromPlaneOffsetLocation(
		const FVector2D& PixelPosition,
		const float TileRadius
	);

};


// a list of coordinates
// supports boolean operations for creating unique lists of coordinates with dynamic shapes
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridCoordinateList
{
	GENERATED_BODY()

public:

	FHexGridCoordinateList()
		: List()
	{
	}

	FHexGridCoordinateList(TArray<FHexGridCoordinate> const& InList)
		: FHexGridCoordinateList()
	{
		for (int i = 0; i < InList.Num(); i++)
		{
			List.AddUnique(InList[i]);
		}
	}

	UPROPERTY(EditAnywhere)
	TArray<FHexGridCoordinate> List;

	// returns number of coordinates in the list
	int32 Num() const { return List.Num(); }

	// get the coordinate at the given index
	FHexGridCoordinate& GetCoordinateAtIndex(const int Index) { return List[Index]; }

	// get the coordinate at the given index
	const FHexGridCoordinate& GetCoordinateAtIndex(const int Index) const { return List[Index]; }

	// get the coordinate at the given index
	const FHexGridCoordinate& operator[](const int Index) const { return GetCoordinateAtIndex(Index); }

	// get the coordinate at the given index
	FHexGridCoordinate& operator[](const int Index) { return GetCoordinateAtIndex(Index); }

	// add the given coordinate and return its index
	int32 Add(FHexGridCoordinate const& NewCoordinate) { return List.Add(NewCoordinate); }

	// add the given coordinate, if it isnt already contained by the list, and return its index
	int32 AddUnique(FHexGridCoordinate const& NewCoordinate) { return List.AddUnique(NewCoordinate); }

	// add the given coordinate list to this one
	void Append(TArray<FHexGridCoordinate> const& NewCoordinates) { List.Append(NewCoordinates); }

	// add the given coordinate list to this one
	void Append(FHexGridCoordinateList const& NewCoordinates) { List.Append(NewCoordinates.List); }

	// check if the given coordinate is in the list
	bool Contains(FHexGridCoordinate const& Coordinate) const { return List.Contains(Coordinate); }

	// remove all instances of the given coordinate from the list and return the number removed 
	int32 Remove(FHexGridCoordinate const& Coordinate) { return List.Remove(Coordinate); }

	// empty the array, optionally providing expected usage size
	void Empty(const int Slack = 0) { List.Empty(Slack); }

	// Same as empty, but doesn't change memory allocations, unless the new size is larger than the current array
	void Reset(const int NewSize = 0) { List.Reset(NewSize); }

	// Reserves memory such that the array can contain at least the given number elements.
	void Reserve(const int DesiredSize) { List.Reserve(DesiredSize); }

	// get a list of unique coordinates contained in either A or B
	static void Union(FHexGridCoordinateList const& A, FHexGridCoordinateList const& B, FHexGridCoordinateList& OutList)
	{
		OutList = FHexGridCoordinateList(A.List);
		for (int i = 0; i < B.List.Num(); i++)
		{
			OutList.List.AddUnique(B.List[i]);
		}


		return;
	}

	// boolean union (addition)
	// get a list of unique coordinates contained in either this list or the other list
	FHexGridCoordinateList operator|(FHexGridCoordinateList const& Other) const
	{
		FHexGridCoordinateList OutList;
		Union((*this), Other, OutList);


		return OutList;
	}

	// boolean union (addition)
	// get a list of unique coordinates contained in either this list or the other list
	FHexGridCoordinateList operator+(FHexGridCoordinateList const& Other) const
	{
		FHexGridCoordinateList OutList;
		Union((*this), Other, OutList);


		return OutList;
	}


	// get a list of unique coordinates that are exclusively contained in both A and B
	static void Intersection(FHexGridCoordinateList const& A, FHexGridCoordinateList const& B, FHexGridCoordinateList& OutList)
	{
		for (int i = 0; i < B.List.Num(); i++)
		{
			if (A.List.Contains(B.List[i]))
			{
				OutList.List.AddUnique(B.List[i]);
			}
		}


		return;
	}

	// boolean intersection
	// get a list of unique coordinates that are exclusively contained in both this list and the other
	FHexGridCoordinateList operator&(FHexGridCoordinateList const& Other) const
	{
		FHexGridCoordinateList OutList;
		Intersection((*this), Other, OutList);


		return OutList;
	}


	// get a list of unique coordinates that are contained in A or B, but not both
	static void Difference(FHexGridCoordinateList const& A, FHexGridCoordinateList const& B, FHexGridCoordinateList& OutList)
	{
		for (int i = 0; i < B.List.Num(); i++)
		{
			if (A.List.Contains(B.List[i]) == false)
			{
				OutList.List.AddUnique(B.List[i]);
			}
		}
		for (int i = 0; i < A.List.Num(); i++)
		{
			if (B.List.Contains(A.List[i]) == false)
			{
				OutList.List.AddUnique(A.List[i]);
			}
		}


		return;
	}

	// boolean difference
	// get a list of unique coordinates that are contained in this list or the other, but not both
	FHexGridCoordinateList operator^(FHexGridCoordinateList const& Other) const
	{
		FHexGridCoordinateList OutList;
		Difference((*this), Other, OutList);


		return OutList;
	}


	// get a list of unique coordinates that are contained in A but are not contained B
	static void Exception(FHexGridCoordinateList const& A, FHexGridCoordinateList const& B, FHexGridCoordinateList& OutList)
	{
		OutList = FHexGridCoordinateList(A.List);
		for (int i = A.List.Num() - 1; i >= 0; i--)
		{
			if (B.List.Contains(A.List[i]) == true)
			{
				OutList.List.Remove(A.List[i]);
			}
		}


		return;
	}

	// boolean exception (subtraction)
	// get a list of unique coordinates that are contained in this list but are not contained the other
	FHexGridCoordinateList operator-(FHexGridCoordinateList const& Other) const
	{
		FHexGridCoordinateList OutList;
		Exception((*this), Other, OutList);


		return OutList;
	}


	// get a list of world offset locations that represent the center of each coordinate in the list
	void GetCenterWorldOffsetLocations(const float TileRadius, TArray<FVector>& OutWorldLocations) const;

	// get a list of world offset locations that represent the corner of each coordinate in the list
	// this provides six world locations for every grid coordinate
	void GetCornerWorldOffsetLocations(const float TileRadius, TArray<FVector>& OutWorldLocations) const;

	// generate an equivalent set of coordinates that are mirrored across the Q axis
	void GenerateMirroredCoordinatesAcrossAxis_Q(FHexGridCoordinateList& OutCoordinates) const;

	// generate an equivalent set of coordinates that are mirrored across the R axis
	void GenerateMirroredCoordinatesAcrossAxis_R(FHexGridCoordinateList& OutCoordinates) const;

	// generate an equivalent set of coordinates that are mirrored across the S axis
	void GenerateMirroredCoordinatesAcrossAxis_S(FHexGridCoordinateList& OutCoordinates) const;

	// get the distance of the furthest coordinate in this list from the center coordinate
	int GetDistanceOfFurthestCoordinateFromCenter() const;

	// get the bounds of the contained coordinates for the given tile radius
	FBox2D GetBounds(const float TileRadius) const;

	// generates all hex coordinates whose centers are overlapped by a 2D bounding box.
	// This performs a broad - phase check first, then a narrow - phase check.
	static bool GenerateHexCoordinatesInBounds(
		const FBox2D& PixelBounds,
		const float TileRadius,
		FHexGridCoordinateList& OutCoordinates
	);

	// generates all hex coordinates whose centers are overlapped by the bounds of this list of coordinates.
	bool GenerateHexCoordinatesInRegion(FHexGridCoordinateList& OutCoordinates) const;

	// finds the hex coordinate closest to the geometric center of this list of coordinates.
	FHexGridCoordinate FindApproximateCenterCoordinate() const;

};


// properties that describe the transform of an object on the grid
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTransform
{
	GENERATED_BODY()

public:

	FHexGridTransform()
	{
		Coordinate = FHexGridCoordinate::c_Zero;
		FacingDirection = EHexGridDirection::north;
	}

	FHexGridTransform(FHexGridCoordinate const& InCoordinate, const EHexGridDirection InDirection)
		: FHexGridTransform()
	{
		Coordinate = InCoordinate;
		FacingDirection = InDirection;
	}

	// coordinate - kind of like a location
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FHexGridCoordinate Coordinate;

	// facing direction - kind of like a rotation
	UPROPERTY(EditAnywhere)
	EHexGridDirection FacingDirection;

};


/*
* A grid of hex tiles that list each tile's coordinate with its data packaged as a type TileDataClass
*
* Inheriting types must provide definitions for pure virtual functions:
*
*	// definition for virtual destructor to satisfy compiler warnings
*	virtual ~FExampleHexGrid() final;
*
*	// get coordinate data for a tile at the given index
*	virtual FHexGridCoordinate GetCoordinateFromTile(TileDataClass const& TileData) const final;
*
*	// get all tile data at the given index
*	virtual TileDataClass GetTileFromIndex(int const& InIndex) const final;
*
*	// get index for a tile at the given coordinate
*	virtual int GetTileIndexFromCoordinate(FHexGridCoordinate const& InCoordinate) const final;
*
*	// get the total number of tiles in the grid
*	virtual int GetTotalNumberOfTiles() const final;
*
*
*
* the TileDataClass type must contain coordinate data and declare and implement the following static functions:
*
*	// For A* pathfinding:
*
*
*	// get estimated minimum cost for traveling between two nodes
*	// if zero,									-- A* becomes Dijkstra's Algorithm	guaranteed to find shortest path, may run slower
*	// if always lower or equal to actual cost	-- still guaranteed to find shortest path
*	// if greater than actual cost				-- not guaranteed to find shortest path, but the algorithm may sometimes run faster
*	// if much greater than actual cost			-- A* becomes greedy best-first-search, not guaranteed to find shortest path
*	static FVector::FReal GetHeuristicCost(const TileDataClass& StartNode, const TileDataClass& EndNode);
*
*
*	// scales cost provided by GetHeuristicCost()
*	static FVector::FReal GetHeuristicScale();
*
*
*	// get actual cost for traveling between two nodes
*	static FVector::FReal GetTraversalCost(const TileDataClass& StartNode, const TileDataClass& EndNode);
*
*
*	// check if an edge exists between two nodes, and thus can be traversed
*	static bool IsTraversalAllowed(const TileDataClass& NodeA, const TileDataClass& NodeB);
*
*
*/
template <typename TTileDataType>
struct THexGridPathfindingProperties
{
	// get coordinate data from tile data
	virtual FHexGridCoordinate GetCoordinateFromTile(TTileDataType const& TileData) const = 0;

	// get all tile data at the given index
	virtual TTileDataType GetTileFromIndex(int const& InIndex) const = 0;

	// get index for a tile at the given coordinate
	virtual int GetTileIndexFromCoordinate(FHexGridCoordinate const& InCoordinate) const = 0;

	// get the total number of tiles in the grid
	virtual int GetTotalNumberOfTiles() const = 0;


	FORCEINLINE bool GetTileFromCoordinate(FHexGridCoordinate const& InCoordinate, TTileDataType& OutTile) const
	{
		const int TileIndex = GetTileIndexFromCoordinate(InCoordinate);
		if (IsValidTileIndex(TileIndex))
		{
			OutTile = GetTileFromIndex(TileIndex);
			return true;
		}

		return false;
	}

	// get coordinate data for a tile at the given index
	FORCEINLINE FHexGridCoordinate GetTileCoordinateFromIndex(int const& InIndex) const
	{
		const TTileDataType Tile = GetTileFromIndex(InIndex);
		return GetCoordinateFromTile(Tile);
	}

	// get a list of coordinates from the grid
	FORCEINLINE void GenerateCoordinateList(TArray<FHexGridCoordinate>& OutList) const
	{
		for (int i = 0; i < GetTotalNumberOfTiles(); i++)
		{
			OutList.Add(GetTileCoordinateFromIndex(i));
		}
	}

	// get a list of coordinates from the grid
	FORCEINLINE void GenerateCoordinateList(FHexGridCoordinateList& OutList) const
	{
		for (int i = 0; i < GetTotalNumberOfTiles(); i++)
		{
			OutList.List.AddUnique(GetTileCoordinateFromIndex(i));
		}
	}

	// check if the grid contains the given coordinate 
	FORCEINLINE bool ContainsCoordinate(FHexGridCoordinate const& InCoordinate) const
	{
		return IsValidTileIndex(GetTileIndexFromCoordinate(InCoordinate));
	}

	// check if the grid contains the given coordinate 
	FORCEINLINE bool IsValidTileIndex(const int TileIndex) const
	{
		return TileIndex >= 0 && TileIndex < GetTotalNumberOfTiles();
	}


	// Graph A* Search implementations
	typedef int FNodeRef;
	FORCEINLINE bool IsValidRef(FNodeRef NodeRef) const { return NodeRef >= 0 && NodeRef < GetTotalNumberOfTiles(); }
	FORCEINLINE int32 GetNeighbourCount(FNodeRef NodeRef) const { return FHexGridCoordinate::c_HexNumberOfSides; }
	FORCEINLINE FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighborIndex) const
	{
		return GetTileIndexFromCoordinate(GetTileCoordinateFromIndex(NodeRef).GetNeighbors()[NeighborIndex]);
	}

	// filter class used to find a path
	template <typename T, bool bWantsPartialSolution>
	struct THexGridPathfindingPropertiesQueryFilter
	{
		THexGridPathfindingPropertiesQueryFilter(const THexGridPathfindingProperties<T>* InGrid)
			: Grid(InGrid)
		{
		}

		const THexGridPathfindingProperties<T>* Grid;

		FVector::FReal GetHeuristicScale() const
		{
			return T::GetHeuristicScale();
		}
		FVector::FReal GetHeuristicCost(const FNodeRef& StartNode, const FNodeRef& EndNode) const
		{
			return T::GetHeuristicCost(Grid->GetTileFromIndex(StartNode), Grid->GetTileFromIndex(EndNode));
		}
		FVector::FReal GetTraversalCost(const FNodeRef& StartNode, const FNodeRef& EndNode) const
		{
			return T::GetTraversalCost(Grid->GetTileFromIndex(StartNode), Grid->GetTileFromIndex(EndNode));
		}
		bool IsTraversalAllowed(const FNodeRef NodeA, const FNodeRef NodeB) const
		{
			return T::IsTraversalAllowed(Grid->GetTileFromIndex(NodeA), Grid->GetTileFromIndex(NodeB));
		}
		bool WantsPartialSolution() const { return bWantsPartialSolution; }
		bool ShouldIncludeStartNodeInPath() const { return true; }
	};

	// find path using A* search algorithm on the current grid
	// template parameter bool is used to determine if partial solutions are acceptable
	template <bool bWantsPartialSolution>
	FORCEINLINE EGraphAStarResult FindPath(FNodeRef const& StartNode, FNodeRef const& EndNode, TArray<FNodeRef>& OutPath) const
	{
		FGraphAStar<THexGridPathfindingProperties<TTileDataType>> AStar((*this));
		return AStar.template FindPath<THexGridPathfindingPropertiesQueryFilter<TTileDataType, bWantsPartialSolution>>(
			StartNode,
			EndNode,
			THexGridPathfindingPropertiesQueryFilter<TTileDataType,
			bWantsPartialSolution>((this)),
			OutPath
		);
	}

	// find path using A* search algorithm on the current grid
	// template parameter bool is used to determine if partial solutions are acceptable
	template <bool bWantsPartialSolution>
	FORCEINLINE EGraphAStarResult FindPath(FHexGridCoordinate const& StartNode, FHexGridCoordinate const& EndNode, FHexGridCoordinateList& OutPath) const
	{
		const FNodeRef StartNodeRef = GetTileIndexFromCoordinate(StartNode);
		if (IsValidRef(StartNodeRef) == false)
		{
			return EGraphAStarResult::GoalUnreachable;
		}

		const FNodeRef EndNodeRef = GetTileIndexFromCoordinate(EndNode);
		if (IsValidRef(EndNodeRef) == false)
		{
			return EGraphAStarResult::GoalUnreachable;
		}

		TArray<FNodeRef> PathNodeRefs;
		const EGraphAStarResult Result = FindPath<bWantsPartialSolution>(StartNodeRef, EndNodeRef, PathNodeRefs);

		// convert path node references to coordinates at the output
		for (int i = 0; i < PathNodeRefs.Num(); i++)
		{
			OutPath.Add(GetCoordinateFromTile(GetTileFromIndex(PathNodeRefs[i])));
		}


		return Result;
	}

	// check if a path exists between the two coordinates
	FORCEINLINE bool CanNavigateBetweenCoordinates(FHexGridCoordinate const& TileA, FHexGridCoordinate const& TileB) const
	{
		FHexGridCoordinateList UnusedPath;
		return FindPath<false>(TileA, TileB, UnusedPath) == EGraphAStarResult::SearchSuccess;
	}

	// check if this set of pathfinding data does include a tile for each of the given set of coordinates
	FORCEINLINE bool DoesContainTileForEachCoordinate(FHexGridCoordinateList const& Coordinates) const
	{
		for (int i = 0; i < Coordinates.Num(); i++)
		{
			if (ContainsCoordinate(Coordinates[i]) == false)
			{
				return false;
			}
		}


		return true;
	}

	// remove any coordinate from the given set that does not have matching tile data
	// returns true if the set is modified at all
	FORCEINLINE bool FilterCoordinates(FHexGridCoordinateList& Coordinates) const
	{
		const int RemovedCount = Coordinates.List.RemoveAllSwap(
			[this](const FHexGridCoordinate& Coord)
			{
				return ContainsCoordinate(Coord) == false;
			}
		);


		return RemovedCount > 0;
	}

	// get the total traversal cost of the given set of templated<TTileDataType>
	FORCEINLINE FVector::FReal GetPathTraversalCost(FHexGridCoordinateList const& Path) const
	{
		FVector::FReal OutCost = 0.0;
		for (int i = 0; i < Path.Num() - 1; i++)
		{
			TTileDataType TileA;
			TTileDataType TileB;
			if (GetTileFromCoordinate(Path[i], TileA) == false || GetTileFromCoordinate(Path[i + 1], TileB) == false)
			{
				return 0.0;
			}

			FVector::FReal StepCost = TTileDataType::GetTraversalCost(TileA, TileB);
			OutCost += StepCost;
		}


		return OutCost;
	}

	// get the total heuristic cost of the given set of templated<TTileDataType>
	FORCEINLINE FVector::FReal GetPathHeuristicCost(FHexGridCoordinateList const& Path) const
	{
		FVector::FReal OutCost = 0.0;
		for (int i = 0; i < Path.Num() - 1; i++)
		{
			TTileDataType TileA;
			TTileDataType TileB;
			if (GetTileFromCoordinate(Path[i], TileA) == false || GetTileFromCoordinate(Path[i + 1], TileB) == false)
			{
				return 0.0;
			}

			FVector::FReal StepCost = TTileDataType::GetHeuristicCost(TileA, TileB);
			OutCost += StepCost;
		}


		return OutCost;
	}

	// find all nodes that are valid paths with a provided cost-function that returns a value within the given range
	// only searches tiles that are within the given tile distance from the start node because 
	// searching the whole graph can cause hitches
	FORCEINLINE EGraphAStarResult FindNodesWithinTileDistanceAndWithCostFunctionInRange(
		FHexGridCoordinate const& StartNode,
		const int TileDistance,
		const FVector::FReal MinimumCost,
		const FVector::FReal MaximumCost,
		FVector::FReal(THexGridPathfindingProperties<TTileDataType>::* CostFunction)(FHexGridCoordinateList const&) const,
		FHexGridCoordinateList& OutNodes) const
	{
		EGraphAStarResult FinalResult = EGraphAStarResult::SearchFail;

		// get all coordinates within the given tile distance
		FHexGridCoordinateList GridCoordinates;
		FHexGridCoordinate::GetCoordinatesWithinRangeOfTile(StartNode, TileDistance, GridCoordinates);

		// remove any invalid coordinates that dont have matching tiles
		FilterCoordinates(GridCoordinates);

		// find all nodes with a cost within range
		const int TotalCoordinates = GridCoordinates.Num();
		for (int i = 0; i < TotalCoordinates; i++)
		{
			if (GridCoordinates[i] == StartNode)
			{
				continue;
			}
			FHexGridCoordinateList Path;
			EGraphAStarResult PathResult = FindPath<false>(StartNode, GridCoordinates[i], Path);
			FVector::FReal PathCost = 0.0;
			switch (PathResult)
			{
			case EGraphAStarResult::SearchSuccess:
				PathCost = (this->*CostFunction)(Path);
				if (PathCost >= MinimumCost && PathCost <= MaximumCost)
				{
					FinalResult = EGraphAStarResult::SearchSuccess;
					OutNodes.Add(GridCoordinates[i]);
				}
				break;

			default:
				break;
			}
		}


		return FinalResult;
	}

	// find all nodes that are valid paths with a total traversal cost within the given range
	// only searches tiles that are within the given tile distance from the start node because 
	// searching the whole graph can cause hitches
	FORCEINLINE EGraphAStarResult FindNodesWithinTileDistanceAndWithTraversalCostInRange(
		FHexGridCoordinate const& StartNode,
		const int TileDistance,
		const FVector::FReal MinimumCost,
		const FVector::FReal MaximumCost,
		FHexGridCoordinateList& OutNodes) const
	{
		return FindNodesWithinTileDistanceAndWithCostFunctionInRange(
			StartNode,
			TileDistance,
			0.0,
			MaximumCost,
			&THexGridPathfindingProperties<TTileDataType>::GetPathTraversalCost,
			OutNodes
		);
	}

	// find all nodes that are valid paths with a total traversal cost below the given maximum
	// only searches tiles that are within the given tile distance from the start node because 
	// searching the whole graph can cause hitches
	FORCEINLINE EGraphAStarResult FindNodesWithinTileDistanceAndWithMaxTraversalCost(
		FHexGridCoordinate const& StartNode,
		const int TileDistance,
		const FVector::FReal MaximumCost,
		FHexGridCoordinateList& OutNodes) const
	{
		return FindNodesWithinTileDistanceAndWithTraversalCostInRange(
			StartNode,
			TileDistance,
			0.0,
			MaximumCost,
			OutNodes
		);
	}

	// find all nodes that are valid paths with a total heuristic cost within the given range
	// only searches tiles that are within the given tile distance from the start node because 
	// searching the whole graph can cause hitches
	FORCEINLINE EGraphAStarResult FindNodesWithinTileDistanceAndWithHeuristicCostInRange(
		FHexGridCoordinate const& StartNode,
		const int TileDistance,
		const FVector::FReal MinimumCost,
		const FVector::FReal MaximumCost,
		FHexGridCoordinateList& OutNodes) const
	{
		return FindNodesWithinTileDistanceAndWithCostFunctionInRange(
			StartNode,
			TileDistance,
			0.0,
			MaximumCost,
			&THexGridPathfindingProperties<TTileDataType>::GetPathHeuristicCost,
			OutNodes
		);
	}

	// find all nodes that are valid paths with a total heuristic cost below the given maximum
	// only searches tiles that are within the given tile distance from the start node because 
	// searching the whole graph can cause hitches
	FORCEINLINE EGraphAStarResult FindNodesWithinTileDistanceAndWithMaxHeuristicCost(
		FHexGridCoordinate const& StartNode,
		const int TileDistance,
		const FVector::FReal MaximumCost,
		FHexGridCoordinateList& OutNodes) const
	{
		return FindNodesWithinTileDistanceAndWithHeuristicCostInRange(
			StartNode,
			TileDistance,
			0.0,
			MaximumCost,
			OutNodes
		);
	}

};


// a package that contains a tile coordinate and type
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTile
{
	GENERATED_BODY()

public:

	FHexGridTile()
		: Coordinate()
		, TileTypeIndex(-1)
	{
	}

	FHexGridTile(FHexGridCoordinate const& InCoordinate, const int InTileTypeIndex)
		: Coordinate(InCoordinate)
		, TileTypeIndex(InTileTypeIndex)
	{
	}

	// the coordinate for this tile
	UPROPERTY()
	FHexGridCoordinate Coordinate;

	// the type id for this tile
	UPROPERTY()
	int TileTypeIndex;

};


// a package that contains a tile reference and actors that are present on the tile
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileRegister
{
	GENERATED_BODY()

public:

	FHexGridTileRegister()
		: HismInstanceId(-1)
		, PresentActors()
	{
	}

	FHexGridTileRegister(const int InHismInstanceId, TArray<AActor*> const& InPresentActors)
		: HismInstanceId(InHismInstanceId)
		, PresentActors()
	{
		SetPresentActors(InPresentActors);
	}

	// the instance
	UPROPERTY()
	int HismInstanceId;

	// the actors that are present on this tile
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> PresentActors;

	// get the actors present on this tile
	int GetAllPresentActors(TArray<AActor*>& OutPresentActors) const;

	// get the actors present on this tile that match a given class
	int GetAllPresentActorsOfClass(UClass* ActorClass, TArray<AActor*>& OutPresentActors) const;

	// templated get the actors present on this tile that match a given class
	template <class TActorClass>
	int GetAllPresentActorsOfClass(TArray<AActor*>& OutPresentActors) const
	{
		return GetAllPresentActorsOfClass(TActorClass::StaticClass(), OutPresentActors);
	}

	// get the number of present actors
	int GetNumberOfPresentActors() const;

	// get the number of present actors of a given class
	int GetNumberOfPresentActorsOfClass(UClass* ActorClass) const;

	// templated get the number of present actors of a given class
	template <class TActorClass>
	int GetNumberOfPresentActorsOfClass(UClass* ActorClass) const
	{
		return GetNumberOfPresentActorsOfClass(TActorClass::StaticClass());
	}

	// set the actors present on this tile
	void SetPresentActors(TArray<AActor*> const& InPresentActors);

	// add an the actor present on this tile
	void AddPresentActors(TArray<AActor*> const& InPresentActors);

	// add an the actor present on this tile
	// returns the index to the new present actor
	int AddPresentActor(AActor* InPresentActor);

	// remove an the actor's presence from this tile
	// returns the number of removed indices
	int RemovePresentActor(AActor* InPresentActor);

	// check if the given actor is present on this tile
	bool IsActorPresent(const AActor* SpecificActor) const
	{
		return PresentActors.Contains(SpecificActor);
	}

	// check if the tile contains any present actors
	bool AreActorsPresent() const;

	// get the first present actor of the given class
	AActor* GetFirstPresentActorOfClass(UClass* ActorClass) const;

	// templated get the first present actor of the given class
	template <class TActorClass>
	TActorClass* GetFirstPresentActorOfClass() const
	{
		return GetFirstPresentActorOfClass(TActorClass::StaticClass());
	}

	// check if any actor of the given class is present
	bool IsAnyActorOfClassPresent(UClass* ActorClass) const;

	// templated check if any actor of the given class is present
	template <class TActorClass>
	bool IsAnyActorPresent() const
	{
		return IsAnyActorOfClassPresent(TActorClass::StaticClass());
	}

	// clears all present actors
	void ClearPresentActors()
	{
		PresentActors.Reset();
	}

};


// properties associated with a grid
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridProperties
{
	GENERATED_BODY()

public:

	FHexGridProperties()
		: TileRadius(50.f)
		, MaxTilesGeneratedPerFrame(36)
		, TileTypes()
	{
	}

	FHexGridProperties(
		const float InTileRadius,
		const int InMaxTilesGeneratedPerFrame
	)
		: TileRadius(InTileRadius)
		, MaxTilesGeneratedPerFrame(InMaxTilesGeneratedPerFrame)
		, TileTypes()
	{
	}

	FHexGridProperties(
		const float InTileRadius,
		const int InMaxTilesGeneratedPerFrame,
		TArray<TSoftObjectPtr<const UHexGridTileTypeAsset>> const& InTileTypes
	)
		: TileRadius(InTileRadius)
		, MaxTilesGeneratedPerFrame(InMaxTilesGeneratedPerFrame)
		, TileTypes(InTileTypes)
	{
	}

	// radius of each hexagon tile
	// measured from the center to any corner
	UPROPERTY(EditAnywhere)
	float TileRadius;

	// max number of tiles that can be generated on each frame
	UPROPERTY(EditAnywhere)
	int MaxTilesGeneratedPerFrame;

	// get the world offset location from the center tile (0,0) for the center of the given coordinate
	FVector GetCoordinateWorldOffsetLocation(FHexGridCoordinate const& InCoordinate) const
	{
		return InCoordinate.GetCenterWorldOffsetLocation(TileRadius);
	}

	// the set of valid tile types that can be used in the grid
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<const UHexGridTileTypeAsset>> TileTypes;

	// get the index of the given tile type if it is contained in the list of tile types
	int GetTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const;

	// collect assets for preloading
	void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const;

};


// properties used to create a grid of hex tiles
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridSetupProperties : public FHexGridProperties
{
	GENERATED_BODY()

public:

	FHexGridSetupProperties()
		: FHexGridProperties()
		, Shape(EHexGridShape::hexagon)
		, GridRadius(3)
	{
	}

	FHexGridSetupProperties(
		FHexGridProperties const& InGridProperties,
		const EHexGridShape InShape,
		const int InGridRadius
	)
		: FHexGridProperties(InGridProperties)
		, Shape(InShape)
		, GridRadius(InGridRadius)
	{
	}

	FHexGridSetupProperties(
		const float InTileRadius,
		const int InMaxTilesGeneratedPerFrame,
		const EHexGridShape InShape,
		const int InGridRadius
	)
		: FHexGridProperties(InTileRadius, InMaxTilesGeneratedPerFrame)
		, Shape(InShape)
		, GridRadius(InGridRadius)
	{
	}

	// shape of the grid
	UPROPERTY(EditAnywhere)
	EHexGridShape Shape;

	// approximately the radius of the grid in tiles
	// meaning changes based on shape
	UPROPERTY(EditAnywhere)
	int GridRadius;

	// create a list of coordinates using the properties of the grid
	bool CreateGridCoordinates(FHexGridCoordinateList& OutCoordinates) const
	{
		return FHexGridCoordinate::GetCoordinatesInShapeAroundCoordinate(
			FHexGridCoordinate::c_Zero,
			Shape,
			GridRadius,
			OutCoordinates
		);
	}

};


// a map of tiles that share the same type
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileTypeRegistry
{
	GENERATED_BODY()

public:

	FHexGridTileTypeRegistry()
		: TileType(nullptr)
		, Tiles()
	{
	}

	FHexGridTileTypeRegistry(const UHexGridTileTypeAsset* InType)
		: TileType(InType)
		, Tiles()
	{
	}

	// the tiles in this type
	UPROPERTY()
	const UHexGridTileTypeAsset* TileType;

	// the tiles in this type
	UPROPERTY()
	TMap<FHexGridCoordinate, FHexGridTileRegister> Tiles;

};


// a map of tile types and the tiles that belong to each type
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileMap
{
	GENERATED_BODY()

public:

	FHexGridTileMap()
		: TileTypeRegistries()
	{
	}

	// the tiles in this type
	UPROPERTY()
	TArray<FHexGridTileTypeRegistry> TileTypeRegistries;

	// get the index of the tile type registry that contains the tile at the given coordinate
	int GetTileTypeIndexFromCoordinate(FHexGridCoordinate const& InCoordinate) const;

	// check if the given tile coordinate exists in the map
	bool ContainsTile(FHexGridCoordinate const& InCoordinate) const;

	// get the type registry containing the given tile coordinate
	// assumes the tile exists in the map
	FHexGridTileTypeRegistry& GetTypeRegistryContainingTile(FHexGridCoordinate const& InCoordinate)
	{
		return TileTypeRegistries[GetTileTypeIndexFromCoordinate(InCoordinate)];
	}

	// get the type registry containing the given tile coordinate
	// assumes the tile exists in the map
	const FHexGridTileTypeRegistry& GetTypeRegistryContainingTile(FHexGridCoordinate const& InCoordinate) const
	{
		return TileTypeRegistries[GetTileTypeIndexFromCoordinate(InCoordinate)];
	}

	// get the register containing the given tile coordinate
	// assumes the tile exists in the map
	FHexGridTileRegister& GetRegisterContainingTile(FHexGridCoordinate const& InCoordinate)
	{
		return GetTypeRegistryContainingTile(InCoordinate).Tiles[InCoordinate];
	}

	// get the register containing the given tile coordinate
	// assumes the tile exists in the map
	const FHexGridTileRegister& GetRegisterContainingTile(FHexGridCoordinate const& InCoordinate) const
	{
		return GetTypeRegistryContainingTile(InCoordinate).Tiles[InCoordinate];
	}

	// get list of all tile coordinates in the map
	int GetTileCoordinates(FHexGridCoordinateList& OutCoordinates) const;

	// get list of all tiles in the map
	int GetTiles(TArray<FHexGridTile>& OutTiles) const;

	// get the number of tiles in the map
	int GetNumberOfTiles() const
	{
		FHexGridCoordinateList UnusedCoordinates;
		return GetTileCoordinates(UnusedCoordinates);
	}

	// add a tile type to the map and return its index
	int RegisterTileType(const UHexGridTileTypeAsset* TileType)
	{
		return TileTypeRegistries.Add(FHexGridTileTypeRegistry(TileType));
	}

	// add a tile to the map
	// the tile type must be a valid registered tile index and the tile coordinate must be unique
	// returns true if the tile is successfully added
	bool RegisterTile(FHexGridTile const& InTile, const int HismInstanceId, TArray<AActor*> InPresentActors = TArray<AActor*>());

	// find the tile coordinates containing the given actor
	// returns true if the actor is found present on a tile
	bool FindActorCoordinates(const AActor* InActor, FHexGridCoordinateList& OutCoordinateList) const;

	// check if the actor is present on a tile on the grid
	bool ContainsActor(const AActor* InActor) const;

	// remove the given tile from the map
	// returns number of tiles removed
	int RemoveTile(FHexGridCoordinate const& TileCoordinate);

	// get a list of all actors present on tiles in the map
	int GetAllPresentActors(TArray<AActor*>& OutArray) const;

	// get a list of all actors present on tiles in the map of a given class
	int GetAllPresentActorsOfClass(UClass* ActorClass, TArray<AActor*>& OutArray) const;

	// check if the given tile type index is valid
	bool IsValidTileTypeIndex(const int TileTypeIndex) const;

	// get the index of the given tile type
	int GetTileTypeIndex(const UHexGridTileTypeAsset* TileType) const;

	// generate a list of tile types
	void GenerateTileTypeList(TArray<const UHexGridTileTypeAsset*>& OutTileTypes) const;

	// generate a map of tile type indices
	void GenerateTileTypeIndexMap(TMap<FHexGridCoordinate, int>& OutGridTileTypeMap) const;


};


// a map of groups of tiles
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileGroupMap
{
	GENERATED_BODY()

public:

	FHexGridTileGroupMap()
		: TileGroups()
	{
	}

	UPROPERTY()
	TMap<FName, FHexGridCoordinateList> TileGroups;

	bool IsValidTileGroupName(FName const& TileGroupName) const
	{
		return TileGroups.Contains(TileGroupName);
	}

	// create a new tile group with the given name
	void RegisterTileGroup(FName const& TileGroupName);

	// get the tiles in the given group name
	int GetTileGroup(FName const& TileGroupName, FHexGridCoordinateList& OutTileCoordinates) const;

	// set the tiles in the given group name
	void SetTileGroup(FName const& TileGroupName, FHexGridCoordinateList const& TileCoordinates);

	// set the tiles in the given group name
	void ClearTileGroup(FName const& TileGroupName);

};


// tile instance data that combines tile type and ism instance id
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileInstanceData
{
	GENERATED_BODY()

public:

	FHexGridTileInstanceData()
		: TileTypeId(-1)
		, IsmInstanceId(-1)
	{
	}

	FHexGridTileInstanceData(const int InTileTypeId, const int InIsmInstanceId)
		: TileTypeId(InTileTypeId)
		, IsmInstanceId(InIsmInstanceId)
	{
	}

	// tile type id
	UPROPERTY()
	int TileTypeId;

	// hism instance id
	UPROPERTY()
	int IsmInstanceId;

};


// data that can be used to perfectly reconstruct a grid
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridData
{
	GENERATED_BODY()

public:

	FHexGridData()
		: GridProperties()
		, TileMap()
	{
	}

	// distance between tiles on the grid
	UPROPERTY(EditAnywhere, Category = "Hex Grid Data")
	FHexGridProperties GridProperties;

	// Array of tile coordinates and their associated tile types
	UPROPERTY()
	TMap<FHexGridCoordinate, int> TileMap;

	// get the tile type for the given coordinate
	TSoftObjectPtr<const UHexGridTileTypeAsset> GetTileTypeFromCoordinate(FHexGridCoordinate const& Coordinate) const;

	// get the tile type index for the given coordinate
	int GetTileTypeIndexFromCoordinate(FHexGridCoordinate const& Coordinate) const
	{
		return GetTileTypeIndex(GetTileTypeFromCoordinate(Coordinate));
	}

	// get all tiles in the grid
	void GetTiles(TArray<FHexGridTile>& OutTiles) const;

	// get the index of the given tile type
	int GetTileTypeIndex(TSoftObjectPtr<const UHexGridTileTypeAsset> const& Type) const;

	// get all tile coordinates in the grid of the given type by index
	void GetTileCoordinatesOfType(const int TypeIndex, FHexGridCoordinateList& OutCoordinates) const;

	// get all tile coordinates in the grid of the given type
	void GetTileCoordinatesOfType(
		TSoftObjectPtr<const UHexGridTileTypeAsset> const& Type,
		FHexGridCoordinateList& OutCoordinates
	) const;

	// get the world offset location for the given coordinate if it exists
	FVector GetTileCoordinateWorldOffsetLocation(FHexGridCoordinate const& Coordinate);

};


// visibility data for a set of tiles
USTRUCT()
struct HEXGRIDPLUGIN_API FHexGridTileVisibilityGroupMap
{
	GENERATED_BODY()

public:

	FHexGridTileVisibilityGroupMap()
		: VisibilityGroupMap()
	{
	}

	FHexGridTileVisibilityGroupMap(
		FHexGridCoordinateList const& InCoordinates,
		const EHexGridTileVisibility InVisibility
	)
		: VisibilityGroupMap()
	{
		VisibilityGroupMap.Add(InVisibility, InCoordinates);
	}

	FHexGridTileVisibilityGroupMap(
		TMap<FHexGridCoordinate, EHexGridTileVisibility> const& InTileVisibilityMap
	);

	// tile coordinate
	UPROPERTY()
	TMap<EHexGridTileVisibility, FHexGridCoordinateList> VisibilityGroupMap;

};


// visibility data from the perspective of a particular player
USTRUCT()
struct HEXGRIDPLUGIN_API FFogOfWarPerspectiveData
{
	GENERATED_BODY()

public:

	FFogOfWarPerspectiveData()
		: TileVisibilityMap()
	{
	}

	// map of coordinates and their visibility
	UPROPERTY()
	TMap<FHexGridCoordinate, EHexGridTileVisibility> TileVisibilityMap;

	// get the visibility data of the given tile coordinate
	// unlisted coordinates are assumed to be completely hidden
	EHexGridTileVisibility GetTileVisibility(FHexGridCoordinate const& Coordinate) const
	{
		if (TileVisibilityMap.Contains(Coordinate) == false)
		{
			return EHexGridTileVisibility::hidden;
		}


		return TileVisibilityMap[Coordinate];
	}

	// get all tiles with the given visibility
	// NOTE: since hidden tiles may not exist in the map, this may not capture all tiles that should be hidden
	void GetListedTilesWithVisibility(const EHexGridTileVisibility TileVisibility, FHexGridCoordinateList& OutCoordinates) const;

	// set the given tile to the given visibility
	void SetTileVisibility(FHexGridCoordinate const& Coordinates, const EHexGridTileVisibility TileVisibility);

};


// data used to capture and reload the state of fog of war
// contains maps of tile visibility data for each player
USTRUCT()
struct HEXGRIDPLUGIN_API FFogOfWarPerspectiveMap
{
	GENERATED_BODY()

public:

	FFogOfWarPerspectiveMap()
		: PlayerPerspectives()
	{
	}

	FFogOfWarPerspectiveMap(TMap<int, FFogOfWarPerspectiveData> const& InPlayerPerspectives)
		: PlayerPerspectives()
	{
		PlayerPerspectives.Append(InPlayerPerspectives);
	}

	// map of player indices to their perspective visibility data
	UPROPERTY()
	TMap<int, FFogOfWarPerspectiveData> PlayerPerspectives;

	// get the visibility data of the given tile coordinate for the given player index
	// unlisted coordinates are assumed to be completely hidden
	EHexGridTileVisibility GetTileVisibilityForPlayer(const int PlayerIndex, FHexGridCoordinate const& Coordinate) const;

	// set the visibility data of the given tile coordinate for the given player index
	void SetTileVisibilityForPlayer(
		const int PlayerIndex,
		FHexGridCoordinate const& Coordinate,
		const EHexGridTileVisibility VisibilityState
	);

	// get all tile coordinates with the given visibility for the given player
	// NOTE: only adds coordinates that are listed in the map, unlisted tiles are assumed hidden but wont be added
	void GetListedTilesWithVisibilityForPlayer(
		const int PlayerIndex,
		const EHexGridTileVisibility VisibilityState,
		FHexGridCoordinateList& OutCoordinates
	) const;

	// check if the given player index is valid
	bool IsValidPlayerIndex(const int PlayerIndex) const
	{
		return PlayerPerspectives.Contains(PlayerIndex);
	}

	// get a direct reference to the perspective data of the given player index
	FFogOfWarPerspectiveData& GetPlayerPerspectiveDataReference(const int PlayerIndex)
	{
		return PlayerPerspectives[PlayerIndex];
	}

};


// properties of the fog of war on a grid
USTRUCT()
struct HEXGRIDPLUGIN_API FFogOfWarProperties
{
	GENERATED_BODY()

public:

	FFogOfWarProperties()
		: FogColor_Hidden(FLinearColor::Black)
		, FogColor_Explored(FLinearColor::Gray)
		, RevealBrushTileRadiusScalar(5.f)
		, FogBoundsPadding(20.f)
		, FogHeight(150.f)
		, FogFadeDuration(1.f)
	{
	}

	FFogOfWarProperties(
		FLinearColor const& InFogColor_Hidden,
		FLinearColor const& InFogColor_Explored,
		const float InRevealBrushTileRadiusScalar,
		const float InFogBoundsPadding,
		const float InFogHeight,
		const float InFogFadeDuration
	)
		: FogColor_Hidden(InFogColor_Hidden)
		, FogColor_Explored(InFogColor_Explored)
		, RevealBrushTileRadiusScalar(InRevealBrushTileRadiusScalar)
		, FogBoundsPadding(InFogBoundsPadding)
		, FogHeight(InFogHeight)
		, FogFadeDuration(InFogFadeDuration)
	{
	}

	// the color of the fog of war
	UPROPERTY(EditAnywhere)
	FLinearColor FogColor_Hidden;

	// the color of the fog of war
	UPROPERTY(EditAnywhere)
	FLinearColor FogColor_Explored;

	// size of the brush used to apply visibility to hidden areas
	// scaled from the grid tile radius
	UPROPERTY(EditAnywhere)
	float RevealBrushTileRadiusScalar;

	// padding added to the bounds of the fog of war
	UPROPERTY(EditAnywhere)
	float FogBoundsPadding;

	// the height of the fog of war
	UPROPERTY(EditAnywhere)
	float FogHeight;

	// the fade duration of the fog of war
	UPROPERTY(EditAnywhere)
	float FogFadeDuration;

};


// properties used to setup fog of war on a grid
USTRUCT()
struct HEXGRIDPLUGIN_API FFogOfWarSetupProperties : public FFogOfWarProperties
{
	GENERATED_BODY()

public:

	FFogOfWarSetupProperties()
		: FFogOfWarProperties()
		, ComponentClass(nullptr)
	{
	}

	FFogOfWarSetupProperties(
		FLinearColor const& InFogColor_Hidden,
		FLinearColor const& InFogColor_Explored,
		const float InRevealBrushTileRadiusScalar,
		const float InFogBoundsPadding,
		const float InFogHeight,
		const float InFogFadeDuration,
		TSoftClassPtr<UHexGridFogOfWarComponent> const& InComponentClass
	)
		: FFogOfWarProperties(
			InFogColor_Hidden,
			InFogColor_Explored,
			InRevealBrushTileRadiusScalar,
			InFogBoundsPadding,
			InFogHeight,
			InFogFadeDuration
		)
		, ComponentClass(InComponentClass)
	{
	}

	// the fog of war component class
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UHexGridFogOfWarComponent> ComponentClass;

	// collect assets for preloading
	void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const;

};


// properties for a dynamic minimap indicator (e.g., quest marker).
USTRUCT(BlueprintType)
struct FMinimapIndicatorProperties
{
	GENERATED_BODY()

public:

	FMinimapIndicatorProperties()
		: Texture(nullptr)
		, TintColor(FLinearColor::White)
		, Size(32.f, 32.f)
		, OffScreenEdgePadding(16.f)
		, bRotateOffScreen(false)
		, bRotateOnScreen(false)
	{

	}

	FMinimapIndicatorProperties(
		TSoftObjectPtr<UTexture2D> const& InTexture, 
		FLinearColor const& InTintColor,
		FVector2D const& InSize,
		const float InOffScreenEdgePadding,
		bool ShouldRotateOnScreen, 
		bool ShouldRotateOffScreen
	)
		: Texture(InTexture)
		, TintColor(InTintColor)
		, Size(InSize)
		, OffScreenEdgePadding(InOffScreenEdgePadding)
		, bRotateOffScreen(ShouldRotateOffScreen)
		, bRotateOnScreen(ShouldRotateOnScreen)
	{

	}

	// the texture of the indicator
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Texture;

	// tint color applied to the texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TintColor;

	// size to render the texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Size;

	// padding from the edge of the minimap to place this indicator when off-screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OffScreenEdgePadding;

	// If true, the dynamic indicator will rotate to point towards their target while off-screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateOffScreen;

	// If true, the dynamic indicator will rotate to point towards their target while on-screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateOnScreen;

	// collect assets for preloading
	void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const;

};


// properties for a dynamic minimap indicator (e.g., quest marker) at runtime
USTRUCT()
struct FRuntimeMinimapIndicatorProperties
{
	GENERATED_BODY()

public:

	FRuntimeMinimapIndicatorProperties()
		: VisualProperties()
		, MinimapLocation(FVector2D::ZeroVector)
	{

	}

	FRuntimeMinimapIndicatorProperties(FMinimapIndicatorProperties const& InIndicatorProperties, FVector2D const& InMinimapLocation)
		: VisualProperties(InIndicatorProperties)
		, MinimapLocation(InMinimapLocation)
	{

	}

	// properties for the indicator
	UPROPERTY()
	FMinimapIndicatorProperties VisualProperties;

	// the location to render the indicator at on the minimap
	UPROPERTY()
	FVector2D MinimapLocation;

};


/**
 *
 */
UCLASS()
class HEXGRIDPLUGIN_API UHexGridTypes : public UObject
{
	GENERATED_BODY()

public:

	// collision channel used to trace hex tiles
	static const ECollisionChannel c_HexTileTraceChannel;

	// convert a graph A* result enum to a readable string
	static FString GraphAStarResultToString(const EGraphAStarResult Result);

	// convert a visibility state to a scalar value
	static float GetVisibilityScalarFromState(const EHexGridTileVisibility State);

};
