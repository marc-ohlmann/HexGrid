// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MathTypes.generated.h"

// mathematical constants

#define MATH_ARCHIMEDES (3.141592f)
#define MATH_EULER (2.718281f)
#define MATH_GOLDEN_RATIO (1.618033f)
#define MATH_PYTHAGORAS (1.414213f)
#define MATH_THEODORUS (1.732050f)
#define MATH_EULER_MASCHERONI (0.577215f)
#define MATH_KHINCHIN (2.685452f)
#define MATH_GLAISHER_KINKELIN (1.282427f)

#define MATH_PI (MATH_ARCHIMEDES)
#define MATH_TWO_PI ((2.f) * (MATH_PI))
#define RADIAN_CIRCLE (MATH_TWO_PI)
#define RADIAN_HALF_CIRCLE (MATH_PI)

#define PERCENTAGE_TO_FRACTION (0.01f)
#define FRACTION_TO_PERCENTAGE (100.f)


// interpolation types
UENUM(BlueprintType)
enum class EInterpolationType : uint8
{
	interpolate,
	linear,
	interpolate_circ_in,
	interpolate_circ_inout,
	interpolate_circ_out,
	interpolate_ease_in,
	interpolate_ease_inout,
	interpolate_ease_out,
	interpolate_expo_in,
	interpolate_expo_inout,
	interpolate_expo_out,
	interpolate_sin_in,
	interpolate_sin_inout,
	interpolate_sin_out,
	interpolate_stepped
};


// interpolation types
UENUM(BlueprintType)
enum class ELerpMode : uint8
{
	euler,
	quat,
	dual_quat
};


// a struct that helps calculate an exponential moving average
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FExponentialMovingAverage
{
	GENERATED_BODY()

public:

	FExponentialMovingAverage()
		: Alpha(0.5f)
		, _AccumulatedAverage(0.f)
	{
	}

	FExponentialMovingAverage(const float InAlpha)
		: Alpha(InAlpha)
		, _AccumulatedAverage(0.f)
	{
	}

	// the closer this value is to zero, the less impact each new number has, and the longer
	// the effect of previous numbers lingers around
	// should be between zero and one
	UPROPERTY(EditAnywhere)
	float Alpha;

	// get current accumulated average
	float GetCurrentAverage() const
	{
		return _AccumulatedAverage;
	}

	// accumulate a new value into the average
	float AccumulateNewValue(float NewValue)
	{
		_AccumulatedAverage = (Alpha * NewValue) + (1.f - Alpha) * _AccumulatedAverage;
		return GetCurrentAverage();
	}

	// reset the accumulated average to zero
	void Reset() { _AccumulatedAverage = 0.f; }

private:

	UPROPERTY()
	float _AccumulatedAverage;

};


// a struct that helps map a single continuous value within a range
// into a coordinate within a circle, effectively mapping
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FLinearToCylindricalMapper
{
	GENERATED_BODY()

public:

	FLinearToCylindricalMapper()
		: AngularOffset(0.f)
		, AngularResolution(36)
		, RadialResolution(10)
		, HeightResolution(3)
	{
	}

	FLinearToCylindricalMapper(const float InAngularOffset, const int InAngularResolution, const int InRadialResolution, const int InHeightResolution)
		: AngularOffset(InAngularOffset)
		, AngularResolution(InAngularResolution)
		, RadialResolution(InRadialResolution)
		, HeightResolution(InHeightResolution)
	{
	}

	// check if the mapper always produces zero
	bool WillAlwaysBeZero() const { return AngularResolution == 0 || RadialResolution == 0 || HeightResolution == 0; }

	// get the maximum integer value that maps to a point on the circle
	// the total number of bites in the circle
	int GetLinearRange() const { return RadialResolution * AngularResolution * HeightResolution; }


	// get the angular index corresponding with the given linear index
	int GetAngularIndexFromLinearIndex(const int LinearIndex) const;

	// get the radial index corresponding with the given linear index
	int GetRadialIndexFromLinearIndex(const int LinearIndex) const;

	// get the height index corresponding with the given linear index
	int GetHeightIndexFromLinearIndex(const int LinearIndex) const;

	// get the linear index corresponding with the given angular, radial, and height indices
	int GetLinearIndexFromCylindricalIndices(const int AngularIndex, const int RadialIndex, const int HeightIndex) const;


	// get the linear index corresponding with the given angular index
	int GetLinearIndexFromAngularIndex(const int AngularIndex) const;

	// get the linear index corresponding with the given angular index
	int GetLinearIndexFromRadialIndex(const int RadialIndex) const;

	// get the linear index corresponding with the given angular index
	int GetLinearIndexFromHeightIndex(const int HeightIndex) const;


	// check if the given linear index value is within range
	bool IsValidLinearIndex(const int LinearIndex) const { return LinearIndex >= 0 && LinearIndex < GetLinearRange(); }

	// check if the given angular index value is within range
	bool IsValidAngularIndex(const int AngularIndex) const { return AngularIndex >= 0 && AngularIndex < AngularResolution; }

	// check if the given radial index value is within range
	bool IsValidRadialIndex(const int RadialIndex) const { return RadialIndex >= 0 && RadialIndex < RadialResolution; }

	// check if the given height index value is within range
	bool IsValidHeightIndex(const int HeightIndex) const { return HeightIndex >= 0 && HeightIndex < HeightResolution; }


	// get the radial distance from the center corresponding with the given linear value
	float RadialDistance(const int LinearIndex) const;

	// get the angular distance around the circle corresponding with the given linear value
	float ThetaAngle(const int LinearIndex) const;

	// get the height distance from the floor corresponding with the given linear value
	float HeightDistance(const int LinearIndex) const;

	// map the given linear value to a polar coordinate (r, theta)
	FVector2D GetPolarCoordinate2D(const int LinearIndex) const
	{
		return FVector2D(
			RadialDistance(LinearIndex),
			ThetaAngle(LinearIndex)
		);
	}

	// map the given linear value to a polar coordinate (r, theta, h)
	FVector GetPolarCoordinate(const int LinearIndex) const
	{
		return FVector(
			RadialDistance(LinearIndex),
			ThetaAngle(LinearIndex),
			HeightDistance(LinearIndex)
		);
	}

	// map the given linear value to a cartesian coordinate (x, y)
	FVector2D GetCartesianCoordinate2D(const int LinearIndex) const;

	// map the given linear value to a cartesian coordinate (x, y, z)
	FVector GetCartesianCoordinate(const int LinearIndex) const;

	// angular offset
	// the angle to cut the first slice
	UPROPERTY(EditAnywhere)
	float AngularOffset;

	// angular resolution
	// the number of pizza slices to make in the circle
	UPROPERTY(EditAnywhere)
	int AngularResolution;

	// radial resolution
	// the number of bites to take out of the slices made from the circle
	UPROPERTY(EditAnywhere)
	int RadialResolution;

	// height resolution
	// the number of pizza's stacked on top of eachother
	UPROPERTY(EditAnywhere)
	int HeightResolution;

};


/**
 * common math related functions and constants
 */
UCLASS()
class MEOHELPERSPLUGIN_API UMathTypes : public UObject
{
	GENERATED_BODY()

public:

	// Default case (Should trigger compile error if used with unsupported types, or default to double)
	// For real continuous floating numbers.
	template<typename T>
	static constexpr T GetRealTolerance() { return T(1.e-4); }

	// Specialization for Float
	// For real continuous floating numbers.
	template<>
	constexpr float GetRealTolerance<float>() { return UE_KINDA_SMALL_NUMBER; }

	// Specialization for Double (UE5 uses UE_KINDA_SMALL_NUMBER for double precision)
	// For real continuous floating numbers.
	template<>
	constexpr double GetRealTolerance<double>() { return UE_DOUBLE_KINDA_SMALL_NUMBER; }


	// Checks if A is greater than or nearly equal to B.
	// A >= B (Safe)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealGreaterThanOrNearlyEqual(const T A, const T B)
	{
		return A >= (B - GetRealTolerance<T>());
	}

	// For real continuous floating numbers.
	// Checks if A is strictly greater than B.
	// A > B (Safe)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealGreaterThan(const T A, const T B)
	{
		return A > (B + GetRealTolerance<T>());
	}


	// Checks if A is less than or nearly equal to B.
	// A <= B (Safe)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealLessThanOrNearlyEqual(const T A, const T B)
	{
		// We add the tolerance to B to include values of A 
		// that are slightly larger than B due to precision.
		return A <= (B + GetRealTolerance<T>());
	}

	// Checks if A is strictly less than B.
	// A < B (Safe)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealLessThan(const T A, const T B)
	{
		// We subtract the tolerance from B to ensure A is 
		// "definitely" smaller than the threshold.
		return A < (B - GetRealTolerance<T>());
	}


	// check if A equals B
	// |A - B| <= 0.0 + Tolerance
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealNearlyEqual(const T A, const T B)
	{
		return FMath::Abs(A - B) <= GetRealTolerance<T>();
	}


	// For real continuous floating numbers.
	// Value == 0.0 + Tolerance
	// (Ignores 0.0000001 drift)
	template <typename T>
	static FORCEINLINE bool IsRealNearlyZero(const T Value)
	{
		return IsRealNearlyEqual(Value, static_cast<T>(0));
	}

	// Checks if value is definitely positive.
	// Value > 0.0 + Tolerance
	// (Ignores 0.0000001 drift)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealPositive(const T Value)
	{
		return IsRealGreaterThan(Value, static_cast<T>(0));
	}

	// Checks if value is zero or negative
	// Value >= 0.0 + Tolerance
	// (Ignores 0.0000001 drift)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealPositiveOrNearlyZero(const T Value)
	{
		return IsRealPositive(Value) || IsRealNearlyZero(Value);
	}

	// Checks if value is definitely negative.
	// Value < 0.0 - Tolerance
	// (Ignores -0.0000001 drift)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealNegative(const T Value)
	{
		return IsRealLessThan(Value, static_cast<T>(0));
	}

	// Checks if value is zero or negative
	// Value <= 0.0 - Tolerance
	// (Ignores -0.0000001 drift)
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE bool IsRealNegativeOrNearlyZero(const T Value)
	{
		return IsRealNegative(Value) || IsRealNearlyZero(Value);
	}

	// Floors the value to an integer with an added tolerance 
	// For real continuous floating numbers.
	template <typename T>
	static FORCEINLINE int32 FloorToIntWithTolerance(const T Value)
	{
		return FMath::FloorToInt(Value + GetRealTolerance<T>());
	}
	
	// Floors the value to an integer with an added tolerance 
	// For real continuous floating numbers.
	static FORCEINLINE int64 FloorToIntWithTolerance(const double Value)
	{
		return FMath::FloorToInt(Value + GetRealTolerance<double>());
	}


	// template function to offset a given additive type by a given value
	// and then clamp it within the given range
	// optionally wraps around values that are above the max value back to the min value, and vice versa
	template <typename T>
	static FORCEINLINE T OffsetValueClamped(const T Val, const T Offset, const T Min, const T Max, bool bWrapAround = false)
	{
		T Out = Val + Offset;
		if (Out < Min) Out = bWrapAround ? Max : Min;
		else if (Out > Max) Out = bWrapAround ? Min : Max;
		return Out;
	}

	// template function to return a value multiplied by itself
	template <typename T>
	static FORCEINLINE T Squared(const T Val)
	{
		return Val * Val;
	}

	// converts the given polar coordinate (r, theta) to cartesian coordinates (x, y, z)
	static FVector ConvertPolarCoordinateToCartesian(const FVector PolarCoordinate);

	// converts the given cartesian coordinates (x, y, z) to polar coordinates (r, theta, h)
	static FVector ConvertCartesianCoordinateToPolar(const FVector CartesianCoordinate, double ArctanZeroCheckErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// converts the given polar coordinate (r, theta) to cartesian coordinates (x, y)
	static FVector2D ConvertPolarCoordinateToCartesian2D(const FVector2D PolarCoordinate);

	// converts the given cartesian coordinates (x, y) to polar coordinates (r, theta)
	static FVector2D ConvertCartesianCoordinateToPolar2D(const FVector2D CartesianCoordinate, double ArctanZeroCheckErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// normalizes a given alpha value to a value between zero and one, using the given min and max values
	static float NormalizeFloatToRange(const float RangeMin, const float RangeMax, const float Alpha);

	// convert a value between zero and one, into a value within the given range
	static float RangedFloatFromNormal(const float RangeMin, const float RangeMax, const float Normal);

	// check if two vectors are nearly equal
	static bool AreVectorsNearlyEqual(FVector const& VectorA, FVector const& VectorB, double ErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// check if two vectors are nearly equal
	static bool AreVectorsNearlyEqual(FVector2D const& VectorA, FVector2D const& VectorB, double ErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// check if two rotations are nearly equal
	static bool AreRotationsNearlyEqual(FRotator const& RotatorA, FRotator const& RotatorB, double ErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// check if two quaternions are nearly equal
	static bool AreQuaternionsNearlyEqual(FQuat const& QuatA, FQuat const& QuatB, double ErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// check if two transforms are nearly equal
	static bool AreTransformsNearlyEqual(FTransform const& TransformA, FTransform const& TransformB, double ErrorTolerance = UE_DOUBLE_SMALL_NUMBER);

	// check if the give location is within the given radius from a given center location
	static bool IsPointWithinRadiusOfLocation(FVector const& TestPoint, const float Radius, FVector const& RadiusCenterLocation);

	// generate a randomly seeded random stream
	// optionally provide a name to use as a seed (hashed value of the string is used as the seed)
	// if no name is provided, the seed is FMath::Rand() multiplied by the UtcNow() timestamp
	static FRandomStream GenerateSeededRandomStream(FName const& Seed = FName());

	// generates a random integer within the given range
	static int GetRandomInt(const int Min, const int Max, FRandomStream const& RandomStream)
	{
		return RandomStream.RandRange(Min, Max);
	}

	// generates a random integer within the given range
	static int GetRandomInt(const int Min, const int Max, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomInt(Min, Max, RandomStream);
	}

	// generates a random double within the given range
	static double GetRandomDouble(const double Min, const double Max, FRandomStream const& RandomStream)
	{
		return RandomStream.FRandRange(Min, Max);
	}

	// generates a random double within the given range
	static double GetRandomDouble(const double Min, const double Max, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomDouble(Min, Max, RandomStream);
	}

	// generates a random boolean value
	static bool GetRandomBool(FRandomStream const& RandomStream)
	{
		return GetRandomDouble(0.0, 1.0, RandomStream) >= UMathTypes::c_Half;
	}

	// generates a random boolean value
	static bool GetRandomBool(FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomBool(RandomStream);
	}

	// generates a random float within the given range
	// equivalent to GetRandomDouble(), but converts inputs and outputs to floats
	static float GetRandomFloat(const float Min, const float Max, FRandomStream const& RandomStream)
	{
		return static_cast<float>(GetRandomDouble(Min, Max, RandomStream));
	}

	// generates a random float within the given range
	// equivalent to GetRandomDouble(), but converts inputs and outputs to floats
	static float GetRandomFloat(const float Min, const float Max, FName const& Seed = FName())
	{
		return static_cast<float>(GetRandomDouble(Min, Max, Seed));
	}

	// get a rotator with a randomly generated value within range for yaw
	static FRotator GetRandomYaw(FRandomStream const& RandomStream);

	// get a rotator with a randomly generated value within range for yaw
	static FRotator GetRandomYaw(FName const& Seed = FName());

	// get a vector that points in a random yaw with a length of one
	static FVector GetRandomHorizontalUnitVector(FRandomStream const& RandomStream);

	// get a vector that points in a random yaw with a length of one
	static FVector GetRandomHorizontalUnitVector(FName const& Seed = FName());

	// get a rotator with randomly generated values within range for pitch, yaw, and roll
	static FRotator GetRandomRotator(FRandomStream const& RandomStream);

	// get a rotator with randomly generated values within range for pitch, yaw, and roll
	static FRotator GetRandomRotator(FName const& Seed = FName());

	// get a vector that points in a random direction with a length of one
	static FVector GetRandomUnitVector(FRandomStream const& RandomStream);

	// get a vector that points in a random direction with a length of one
	static FVector GetRandomUnitVector(FName const& Seed = FName());

	// returns a randomized location within a given radius to the given location
	// optionally randomizing only in the horizontal plane
	static FVector GetRandomLocationWithinRadiusOfLocation(
		FVector const& SearchLocation,
		const float SearchRadius,
		FRandomStream const& RandomStream,
		bool bHorizontalOnly = false);

	// returns a randomized location within a given radius to the given location
	// optionally randomizing only in the horizontal plane
	static FVector GetRandomLocationWithinRadiusOfLocation(
		FVector const& SearchLocation,
		const float SearchRadius,
		FName const& Seed = FName(),
		bool bHorizontalOnly = false);

	// returns a randomized location within a given radius to the given location, guaranteed to be between the inner and outer radius
	// optionally randomizing only in the horizontal plane
	static FVector GetRandomLocationWithinRadiusOfLocation(
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		FRandomStream const& RandomStream,
		bool bHorizontalOnly = false);

	// returns a randomized location within a given radius to the given location, guaranteed to be between the inner and outer radius
	// optionally randomizing only in the horizontal plane
	static FVector GetRandomLocationWithinRadiusOfLocation(
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		FName const& Seed = FName(),
		bool bHorizontalOnly = false);


	// returns a randomized linear color
	static FLinearColor GetRandomLinearColor(FRandomStream const& RandomStream, const float Alpha = 1.f)
	{
		return FLinearColor(
			GetRandomFloat(0.f, 1.f, RandomStream),
			GetRandomFloat(0.f, 1.f, RandomStream),
			GetRandomFloat(0.f, 1.f, RandomStream),
			Alpha
		);
	}

	static FLinearColor GetRandomLinearColor(FName const& Seed = FName(), const float Alpha = 1.f)
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomLinearColor(RandomStream, Alpha);
	}

	static FColor GetRandomColor(FRandomStream const& RandomStream, const float Alpha = 1.f)
	{
		return GetRandomLinearColor(RandomStream, Alpha).QuantizeRound();
	}

	static FColor GetRandomColor(FName const& Seed = FName(), const float Alpha = 1.f)
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomColor(RandomStream, Alpha);
	}

	// returns a random index for a given array
	template <class T>
	static int GetRandomIndex(const TArray<T>& Array, FRandomStream const& RandomStream)
	{
		if (Array.Num() <= 0)
		{
			return -1;
		}


		return UMathTypes::GetRandomInt(0, Array.Num() - 1, RandomStream);
	}

	// returns a random index for a given array
	template <class T>
	static int GetRandomIndex(const TArray<T>& Array, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return UMathTypes::GetRandomIndex<T>(Array, RandomStream);
	}

	// returns a random copy of an element in the given array
	template <class T>
	static T GetRandomElement(const TArray<T>& Array, FRandomStream const& RandomStream)
	{
		const int RandomIndex = UMathTypes::GetRandomIndex(Array, RandomStream);
		if (RandomIndex < 0 || RandomIndex >= Array.Num())
		{
			return T();
		}


		return Array[RandomIndex];
	}

	// returns a random copy of an element in the given array
	template <class T>
	static T GetRandomElement(const TArray<T>& Array, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomElement<T>(Array, RandomStream);
	}

	// returns a random copy of the given array, with the given max number of elements
	template <class T>
	static TArray<T> GetRandomElements(const TArray<T>& Array, const int MaximumNumberOfElements, FRandomStream const& RandomStream)
	{
		if (Array.Num() <= MaximumNumberOfElements)
		{
			return Array;
		}

		TArray<T> Out = Array;
		for(int i = 0; i < MaximumNumberOfElements; i++)
		{
			const int SwapIndex = RandomStream.RandRange(i, Out.Num() - 1);

			Out.Swap(i, SwapIndex);
		}

		Out.SetNum(MaximumNumberOfElements, EAllowShrinking::No);


		return Out;
	}

	// returns a random copy of the given array, with the given max number of elements
	template <class T>
	static TArray<T> GetRandomElements(const TArray<T>& Array, const int MaximumNumberOfElements, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomElements<T>(Array, MaximumNumberOfElements, RandomStream);
	}

	// returns a random ptr of an element in the given array
	template <class T>
	static T* GetRandomElementPtr(const TArray<T*>& Array, FRandomStream const& RandomStream)
	{
		const int RandomIndex = UMathTypes::GetRandomIndex(Array, RandomStream);
		if (RandomIndex < 0 || RandomIndex >= Array.Num())
		{
			return nullptr;
		}


		return Array[RandomIndex];
	}

	// returns a random ptr of an element in the given array
	template <class T>
	static T* GetRandomElementPtr(const TArray<T*>& Array, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);


		return GetRandomElementPtr<T>(Array, RandomStream);
	}


	// swaps every element in the array with random indices
	template <class T>
	static void RandomizeArray(TArray<T>& Array, FRandomStream const& RandomStream)
	{
		const int LastIndex = Array.Num() - 1;
		for (int i = 0; i <= LastIndex; ++i)
		{
			const int RandomIndex = GetRandomInt(i, LastIndex, RandomStream);
			if (i != RandomIndex)
			{
				Array.Swap(i, RandomIndex);
			}
		}
	}


	// swaps every element in the array with random indices
	template <class T>
	static void RandomizeArray(TArray<T>& Array, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = GenerateSeededRandomStream(Seed);
		RandomizeArray<T>(Array, RandomStream);


		return;
	}

	// interpolates the given alpha value using the given interpolation type and modifier value
	static float InterpolateAlphaByType(const EInterpolationType Type, const float Alpha, const float InterpModifier);

	// Linearly interpolates between A and B based on Alpha (100% of A when Alpha=0 and 100% of B when Alpha=1)
	static FRotator Rotator_Lerp(FRotator const& A, FRotator const& B, const float Alpha, bool bShortestPath);

	// Linearly interpolates between A and B based on Alpha (100% of A when Alpha=0 and 100% of B when Alpha=1)
	static FTransform Transform_Lerp(const FTransform& A, const FTransform& B, const float Alpha, const ELerpMode LerpMode = ELerpMode::quat);

	// Ease between A and B using a specified interpolation function
	static FTransform Transform_InterpolateByType(const FTransform& A, const FTransform& B, const float Alpha, const EInterpolationType InterpType, const float InterpModifier = 2.f);

	// Tries to reach Target transform based on distance from Current position, giving a nice smooth feeling when tracking a position.
	static FTransform Transform_InterpTo(const FTransform& Current, const FTransform& Target, float DeltaTime, float InterpSpeed);

	// integer fraction constants
	static const float c_Zero;
	static const float c_One;
	static const float c_Half;
	static const float c_Third;
	static const float c_Quarter;
	static const float c_Fourth;
	static const float c_Fifth;
	static const float c_Sixth;
	static const float c_Seventh;
	static const float c_Eighth;
	static const float c_Ninth;
	static const float c_Tenth;

	// the ratio of a circle's circumference to its diameter
	// the definition for pi
	static const float c_Archimedes;

	// the base of the natural logarithm
	static const float c_Euler;

	// two quantities are in the golden ratio if their ratio is the same as the ratio of their sum to the larger of the two quantities
	// algebraically: where a > b > 0, the solution to (a + b) / a = a / b
	// an irrational number that is a solution to the quadratic equation x^2 - x - 1 = 0
	static const float c_GoldenRatio;

	// the square root of two
	static const float c_Pythagoras;

	// the square root of three
	static const float c_Theodorus;

	// the limiting difference between the harmonic series and the natural logarithm
	static const float c_Euler_Mascheroni;

	// for almost all real numbers x, coefficients of the continued fraction expansion of x have a finite geometric mean that is 
	// independent of the value of x and is known as Khinchin's constant. 
	static const float c_Khinchin;

	// related to the K-function and the Barnes G-function. The constant appears in a number of sums and integrals, especially
	// those involving gamma functions and zeta functions.
	static const float c_Glaisher_Kinkelin;

	// the ratio of a circle's circumference to its diameter
	static const float c_Pi;

	// double the ratio of a circle's circumference to its diameter
	static const float c_Two_Pi;

	// the square root of two
	static const float c_Root_Two;

	// the square root of three
	static const float c_Root_Three;

	// a full circle in radian measurement
	// equals two pi
	static const float c_Circle_Radians;

	// a half circle in radian measurement
	// equals pi
	static const float c_Half_Circle_Radians;

	// a third of a circle in radian measurement
	// equals 2 * pi / 3
	static const float c_Third_Circle_Radians;

	// a quarter circle in radian measurement
	// equals pi / 2
	static const float c_Quarter_Circle_Radians;

	// a fourth of a circle in radian measurement
	// equals pi / 2
	static const float c_Fourth_Circle_Radians;

	// a fifth of a circle in radian measurement
	// equals 2 * pi / 5
	static const float c_Fifth_Circle_Radians;

	// a sixth of a circle in radian measurement
	// equals pi / 3
	static const float c_Sixth_Circle_Radians;

	// a seventh of a circle in radian measurement
	// equals 2 * pi / 7
	static const float c_Seventh_Circle_Radians;

	// an eighth of a circle in radian measurement
	// equals pi / 4
	static const float c_Eighth_Circle_Radians;

	// a ninth of a circle in radian measurement
	// equals 2 * pi / 9
	static const float c_Ninth_Circle_Radians;

	// a tenth of a circle in radian measurement
	// equals pi / 5
	static const float c_Tenth_Circle_Radians;

	// a full circle in degrees measurement
	// 360
	static const float c_Circle_Degrees;

	// a half circle in degrees measurement
	// 180
	static const float c_Half_Circle_Degrees;

	// a third of a circle in degrees measurement
	// 120
	static const float c_Third_Circle_Degrees;

	// a quarter circle in degrees measurement
	// 90
	static const float c_Quarter_Circle_Degrees;

	// a fourth of a circle in degrees measurement
	// 90
	static const float c_Fourth_Circle_Degrees;

	// a fifth of a circle in degrees measurement
	// 72
	static const float c_Fifth_Circle_Degrees;

	// a sixth of a circle in degrees measurement
	// 60
	static const float c_Sixth_Circle_Degrees;

	// a seventh of a circle in degrees measurement
	// 51.42857
	static const float c_Seventh_Circle_Degrees;

	// an eighth of a circle in degrees measurement
	// 45
	static const float c_Eighth_Circle_Degrees;

	// a ninth of a circle in degrees measurement
	// 40
	static const float c_Ninth_Circle_Degrees;

	// a tenth of a circle in degrees measurement
	// 36
	static const float c_Tenth_Circle_Degrees;

	// multiply by a distance ( from FVector::Distance() ) to get a value in meters
	static const float c_DistanceToMetersRatio;

	// multiply by a value in meters to get unreal engine distance ( from FVector::Distance() )
	static const float c_MetersToDistanceRatio;

	// multiply by a radian value to convert to a value in degrees
	static const float c_RadiansToDegreesRatio;

	// multiply by a degree value to convert to a value in radians
	static const float c_DegreesToRadiansRatio;

	// multiply by a percentage value to convert to a fractional value
	// ex: change %17 into 0.17
	static const float c_PercentageToFractionRatio;

	// multiply by a fraction value to convert to a 'whole-ish' value in percentage
	// ex: change 0.17 into 17 to display as %17
	static const float c_FractionToPercentageRatio;


};
