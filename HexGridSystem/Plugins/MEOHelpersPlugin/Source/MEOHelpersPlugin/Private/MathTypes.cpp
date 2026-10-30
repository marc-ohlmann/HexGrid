// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "MathTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"

#include "Math/DualQuat.h"


const float UMathTypes::c_Zero = 0.f;
const float UMathTypes::c_One = 1.f;
const float UMathTypes::c_Half = UMathTypes::c_One / 2.f;
const float UMathTypes::c_Third = UMathTypes::c_One / 3.f;
const float UMathTypes::c_Quarter = UMathTypes::c_One / 4.f;
const float UMathTypes::c_Fourth = UMathTypes::c_Quarter;
const float UMathTypes::c_Fifth = UMathTypes::c_One / 5.f;
const float UMathTypes::c_Sixth = UMathTypes::c_One / 6.f;
const float UMathTypes::c_Seventh = UMathTypes::c_One / 7.f;
const float UMathTypes::c_Eighth = UMathTypes::c_One / 8.f;
const float UMathTypes::c_Ninth = UMathTypes::c_One / 9.f;
const float UMathTypes::c_Tenth = UMathTypes::c_One / 10.f;

const float UMathTypes::c_Archimedes = MATH_ARCHIMEDES;
const float UMathTypes::c_Euler = MATH_EULER;
const float UMathTypes::c_GoldenRatio = MATH_GOLDEN_RATIO;
const float UMathTypes::c_Pythagoras = MATH_PYTHAGORAS;
const float UMathTypes::c_Theodorus = MATH_THEODORUS;
const float UMathTypes::c_Euler_Mascheroni = MATH_EULER_MASCHERONI;
const float UMathTypes::c_Khinchin = MATH_KHINCHIN;
const float UMathTypes::c_Glaisher_Kinkelin = MATH_GLAISHER_KINKELIN;
const float UMathTypes::c_Pi = MATH_PI;
const float UMathTypes::c_Two_Pi = MATH_TWO_PI;
const float UMathTypes::c_Root_Two = MATH_PYTHAGORAS;
const float UMathTypes::c_Root_Three = MATH_THEODORUS;

const float UMathTypes::c_DistanceToMetersRatio = 0.01f;
const float UMathTypes::c_MetersToDistanceRatio = 100.f;

const float UMathTypes::c_RadiansToDegreesRatio = (UMathTypes::c_Half_Circle_Degrees) / UMathTypes::c_Pi;
const float UMathTypes::c_DegreesToRadiansRatio = UMathTypes::c_Pi / (UMathTypes::c_Half_Circle_Degrees);

const float UMathTypes::c_Circle_Radians = UMathTypes::c_Two_Pi;
const float UMathTypes::c_Half_Circle_Radians = UMathTypes::c_Pi;
const float UMathTypes::c_Third_Circle_Radians = UMathTypes::c_Two_Pi * UMathTypes::c_Third;
const float UMathTypes::c_Quarter_Circle_Radians = UMathTypes::c_Pi * UMathTypes::c_Half;
const float UMathTypes::c_Fourth_Circle_Radians = UMathTypes::c_Quarter_Circle_Radians;
const float UMathTypes::c_Fifth_Circle_Radians = UMathTypes::c_Two_Pi * UMathTypes::c_Fifth;
const float UMathTypes::c_Sixth_Circle_Radians = UMathTypes::c_Pi * UMathTypes::c_Third;
const float UMathTypes::c_Seventh_Circle_Radians = UMathTypes::c_Two_Pi * UMathTypes::c_Seventh;
const float UMathTypes::c_Eighth_Circle_Radians = UMathTypes::c_Pi * UMathTypes::c_Fourth;
const float UMathTypes::c_Ninth_Circle_Radians = UMathTypes::c_Two_Pi * UMathTypes::c_Ninth;
const float UMathTypes::c_Tenth_Circle_Radians = UMathTypes::c_Pi * UMathTypes::c_Fifth;
const float UMathTypes::c_Circle_Degrees = 360.f;
const float UMathTypes::c_Half_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Half;
const float UMathTypes::c_Third_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Third;
const float UMathTypes::c_Quarter_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Quarter;
const float UMathTypes::c_Fourth_Circle_Degrees = UMathTypes::c_Quarter_Circle_Degrees;
const float UMathTypes::c_Fifth_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Fifth;
const float UMathTypes::c_Sixth_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Sixth;
const float UMathTypes::c_Seventh_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Seventh;
const float UMathTypes::c_Eighth_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Eighth;
const float UMathTypes::c_Ninth_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Ninth;
const float UMathTypes::c_Tenth_Circle_Degrees = UMathTypes::c_Circle_Degrees * UMathTypes::c_Tenth;

const float UMathTypes::c_PercentageToFractionRatio = PERCENTAGE_TO_FRACTION;
const float UMathTypes::c_FractionToPercentageRatio = FRACTION_TO_PERCENTAGE;


float UMathTypes::NormalizeFloatToRange(const float Min, const float Max, const float Alpha)
{
	if (FMath::IsNearlyEqual(Min, Max))
	{
		if (Alpha < Min)
			return 0.f;
		else
			return 1.f;
	}
	else
		return ((Alpha - Min) / (Max - Min));
}


float UMathTypes::RangedFloatFromNormal(const float Min, const float Max, const float Normal)
{
	if (FMath::IsNearlyZero(Normal) == true) return Min;
	else if (FMath::IsNearlyEqual(Normal, 1.f) == true) return Max;
	else return FMath::Lerp(Min, Max, Normal);
}



bool UMathTypes::IsPointWithinRadiusOfLocation(FVector const& TestPoint, const float Radius, FVector const& Location)
{
	return FVector::DistSquared(TestPoint, Location) <= Radius * Radius;
}


FRotator UMathTypes::GetRandomYaw(FRandomStream const& RandomStream)
{
	return FRotator(
		0.f,
		RandomStream.FRandRange(-UMathTypes::c_Circle_Degrees, UMathTypes::c_Circle_Degrees),
		0.f
	);
}


FRotator UMathTypes::GetRandomYaw(FName const& Seed)
{
	const FRandomStream RandomStream = UMathTypes::GenerateSeededRandomStream(Seed);


	return UMathTypes::GetRandomYaw(RandomStream);
}


FVector UMathTypes::GetRandomHorizontalUnitVector(FName const& Seed)
{
	return UMathTypes::GetRandomYaw(Seed).Vector();
}


FVector UMathTypes::GetRandomHorizontalUnitVector(FRandomStream const& RandomStream)
{
	return UMathTypes::GetRandomYaw(RandomStream).Vector();
}


FRotator UMathTypes::GetRandomRotator(FRandomStream const& RandomStream)
{
	return FRotator(
		RandomStream.FRandRange(-UMathTypes::c_Circle_Degrees, UMathTypes::c_Circle_Degrees),
		RandomStream.FRandRange(-UMathTypes::c_Circle_Degrees, UMathTypes::c_Circle_Degrees),
		RandomStream.FRandRange(-UMathTypes::c_Circle_Degrees, UMathTypes::c_Circle_Degrees)
	);
}


FRotator UMathTypes::GetRandomRotator(FName const& Seed)
{
	const FRandomStream RandomStream = UMathTypes::GenerateSeededRandomStream(Seed);


	return UMathTypes::GetRandomRotator(RandomStream);
}


FVector UMathTypes::GetRandomUnitVector(FName const& Seed)
{
	return UMathTypes::GetRandomRotator(Seed).Vector();
}


FVector UMathTypes::GetRandomUnitVector(FRandomStream const& RandomStream)
{
	return UMathTypes::GetRandomRotator(RandomStream).Vector();
}


FVector UMathTypes::GetRandomLocationWithinRadiusOfLocation(FVector const& SearchLocation, const float SearchRadius, FRandomStream const& RandomStream, bool bHorizontalOnly)
{
	return GetRandomLocationWithinRadiusOfLocation(SearchLocation, 0.f, SearchRadius, RandomStream, bHorizontalOnly);
}


FVector UMathTypes::GetRandomLocationWithinRadiusOfLocation(FVector const& SearchLocation, const float SearchRadius, FName const& Seed, bool bHorizontalOnly)
{
	return GetRandomLocationWithinRadiusOfLocation(SearchLocation, 0.f, SearchRadius, Seed, bHorizontalOnly);
}


FVector UMathTypes::GetRandomLocationWithinRadiusOfLocation(FVector const& SearchLocation, const float InnerSearchRadius, const float OuterSearchRadius, FName const& Seed, bool bHorizontalOnly)
{
	const FRandomStream SRand = UMathTypes::GenerateSeededRandomStream(Seed);


	return GetRandomLocationWithinRadiusOfLocation(SearchLocation, InnerSearchRadius, OuterSearchRadius, SRand, bHorizontalOnly);
}


FVector UMathTypes::GetRandomLocationWithinRadiusOfLocation(FVector const& SearchLocation, const float InnerSearchRadius, const float OuterSearchRadius, FRandomStream const& RandomStream, bool bHorizontalOnly)
{
	const float RandomMagnitude = RandomStream.FRandRange(InnerSearchRadius, OuterSearchRadius);
	FVector RandomDirection = FVector::ZeroVector;
	if (bHorizontalOnly)
	{
		RandomDirection = UMathTypes::GetRandomHorizontalUnitVector(RandomStream);
	}
	else
	{
		RandomDirection = UMathTypes::GetRandomUnitVector(RandomStream);
	}

	if (RandomDirection.IsNearlyZero() || FMath::IsNearlyZero(RandomMagnitude))
	{
		return SearchLocation;
	}

	// apply magnitude
	const FVector RandomOffset = RandomDirection * RandomMagnitude;


	return SearchLocation + RandomOffset;
}



FRandomStream UMathTypes::GenerateSeededRandomStream(FName const& Seed)
{
	if (Seed.IsNone() == false)
	{
		return FRandomStream(Seed);
	}


	return FRandomStream(FMath::Rand() * FDateTime::UtcNow().ToUnixTimestamp());
}


bool UMathTypes::AreVectorsNearlyEqual(FVector const& VectorA, FVector const& VectorB, double ErrorTolerance)
{
	const FVector Difference = VectorA - VectorB;


	return Difference.IsNearlyZero(ErrorTolerance);
}


bool UMathTypes::AreVectorsNearlyEqual(FVector2D const& VectorA, FVector2D const& VectorB, double ErrorTolerance)
{
	const FVector2D Difference = VectorA - VectorB;


	return Difference.IsNearlyZero(ErrorTolerance);
}


bool UMathTypes::AreRotationsNearlyEqual(FRotator const& RotatorA, FRotator const& RotatorB, double ErrorTolerance)
{
	const FRotator Difference = RotatorA - RotatorB;


	return Difference.IsNearlyZero(ErrorTolerance);
}


bool UMathTypes::AreQuaternionsNearlyEqual(FQuat const& QuatA, FQuat const& QuatB, double ErrorTolerance)
{
	const FQuat Difference = QuatA - QuatB;


	return	FMath::IsNearlyZero(QuatA.W, ErrorTolerance) &&
		FMath::IsNearlyZero(QuatA.X, ErrorTolerance) &&
		FMath::IsNearlyZero(QuatA.Y, ErrorTolerance) &&
		FMath::IsNearlyZero(QuatA.Z, ErrorTolerance);
}


bool UMathTypes::AreTransformsNearlyEqual(FTransform const& TransformA, FTransform const& TransformB, double ErrorTolerance)
{
	return  UMathTypes::AreRotationsNearlyEqual(TransformA.Rotator(), TransformB.Rotator(), ErrorTolerance) &&
			UMathTypes::AreVectorsNearlyEqual(TransformA.GetLocation(), TransformB.GetLocation(), ErrorTolerance) &&
			UMathTypes::AreVectorsNearlyEqual(TransformA.GetScale3D(), TransformB.GetScale3D(), ErrorTolerance);
}


FRotator UMathTypes::Rotator_Lerp(FRotator const& A, FRotator const& B, const float Alpha, bool bShortestPath)
{
	// if shortest path, we use Quaternion to interpolate instead of using FRotator
	if (bShortestPath)
	{
		FQuat AQuat(A);
		FQuat BQuat(B);

		FQuat Result = FQuat::Slerp(AQuat, BQuat, Alpha);

		return Result.Rotator();
	}

	const FRotator DeltaAngle = B - A;
	return A + Alpha * DeltaAngle;
}


FTransform UMathTypes::Transform_Lerp(const FTransform& A, const FTransform& B, const float Alpha, const ELerpMode LerpMode)
{
	FTransform Result;

	FTransform NA = A;
	FTransform NB = B;
	NA.NormalizeRotation();
	NB.NormalizeRotation();

	switch (LerpMode)
	{

	case ELerpMode::euler:
		Result.SetTranslation(FMath::Lerp(NA.GetTranslation(), NB.GetTranslation(), Alpha));
		Result.SetScale3D(FMath::Lerp(NA.GetScale3D(), NB.GetScale3D(), Alpha));
		Result.SetRotation(FQuat(UMathTypes::Rotator_Lerp(NA.Rotator(), NB.Rotator(), Alpha, false)));
		return Result;

	case ELerpMode::quat:
		Result.Blend(NA, NB, Alpha);
		return Result;

	case ELerpMode::dual_quat:
		if ((NB.GetRotation() | NA.GetRotation()) < 0.0f)
		{
			NB.SetRotation(NB.GetRotation() * -1.0f);
		}
		return (FDualQuat(NA) * (1 - Alpha) + FDualQuat(NB) * Alpha).Normalized().AsFTransform(FMath::Lerp(NA.GetScale3D(), NB.GetScale3D(), Alpha));

	default:
		L_ERR(MEOHelpers, "default on lerp mode '{0}'", LerpMode);
		break;
	}


	return FTransform();
}

// Ease between A and B using a specified interpolation function

// Tries to reach Target transform based on distance from Current position, giving a nice smooth feeling when tracking a position.
FTransform UMathTypes::Transform_InterpolateByType(const FTransform& A, const FTransform& B, const float Alpha, const EInterpolationType InterpType, const float InterpModifier)
{
	return Transform_Lerp(A, B, InterpolateAlphaByType(InterpType, Alpha, InterpModifier));
}

FTransform UMathTypes::Transform_InterpTo(const FTransform& Current, const FTransform& Target, float DeltaTime, float InterpSpeed)
{
	if (InterpSpeed <= 0.f)
	{
		return Current;
	}

	const float Alpha = FMath::Clamp(DeltaTime * InterpSpeed, 0.f, 1.f);

	return Transform_Lerp(Current, Target, Alpha);
}

float UMathTypes::InterpolateAlphaByType(const EInterpolationType Type, const float Alpha, const float InterpModifier)
{
	switch (Type)
	{
	case EInterpolationType::interpolate:				return Alpha;
	case EInterpolationType::linear:					return Alpha;
	case EInterpolationType::interpolate_stepped:		return FMath::InterpStep<float>(0.f, 1.f, Alpha, InterpModifier);
	case EInterpolationType::interpolate_sin_in:		return FMath::InterpSinIn<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_sin_out:		return FMath::InterpSinOut<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_sin_inout:		return FMath::InterpSinInOut<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_ease_in:		return FMath::InterpEaseIn<float>(0.f, 1.f, Alpha, InterpModifier);
	case EInterpolationType::interpolate_ease_out:		return FMath::InterpEaseOut<float>(0.f, 1.f, Alpha, InterpModifier);
	case EInterpolationType::interpolate_ease_inout:	return FMath::InterpEaseInOut<float>(0.f, 1.f, Alpha, InterpModifier);
	case EInterpolationType::interpolate_expo_in:		return FMath::InterpExpoIn<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_expo_out:		return FMath::InterpExpoOut<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_expo_inout:	return FMath::InterpExpoInOut<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_circ_in:		return FMath::InterpCircularIn<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_circ_out:		return FMath::InterpCircularOut<float>(0.f, 1.f, Alpha);
	case EInterpolationType::interpolate_circ_inout:	return FMath::InterpCircularInOut<float>(0.f, 1.f, Alpha);

	default:
		L_ERR(MEOHelpers, "default on interp type '{0}'", Type);
	}


	return Alpha;
}


FVector2D UMathTypes::ConvertPolarCoordinateToCartesian2D(const FVector2D PolarCoordinate)
{
	return FVector2D(
		PolarCoordinate.X * FMath::Cos(PolarCoordinate.Y),
		PolarCoordinate.X * FMath::Sin(PolarCoordinate.Y)
	);
}


FVector2D UMathTypes::ConvertCartesianCoordinateToPolar2D(const FVector2D CartesianCoordinate, double ArctanZeroCheckErrorTolerance)
{
	return FVector2D(
		FMath::Sqrt(CartesianCoordinate.X * CartesianCoordinate.X + CartesianCoordinate.Y * CartesianCoordinate.Y),
		FMath::IsNearlyZero(CartesianCoordinate.X, ArctanZeroCheckErrorTolerance) ? 0.f : FMath::Atan(CartesianCoordinate.Y / CartesianCoordinate.X)
	);
}


FVector UMathTypes::ConvertPolarCoordinateToCartesian(const FVector PolarCoordinate)
{
	return FVector(
		ConvertCartesianCoordinateToPolar2D(FVector2D(PolarCoordinate.X, PolarCoordinate.Y)),
		PolarCoordinate.Z
	);
}


FVector UMathTypes::ConvertCartesianCoordinateToPolar(const FVector CartesianCoordinate, double ArctanZeroCheckErrorTolerance)
{
	return FVector(
		ConvertCartesianCoordinateToPolar2D(FVector2D(CartesianCoordinate.X, CartesianCoordinate.Y), ArctanZeroCheckErrorTolerance),
		CartesianCoordinate.Z
	);
}


int FLinearToCylindricalMapper::GetAngularIndexFromLinearIndex(const int LinearIndex) const
{
	if (WillAlwaysBeZero() ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0;
	}


	return LinearIndex % AngularResolution;
}


int FLinearToCylindricalMapper::GetRadialIndexFromLinearIndex(const int LinearIndex) const
{
	if (WillAlwaysBeZero() ||
		RadialResolution == 0 ||
		AngularResolution == 0 ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0;
	}


	return (LinearIndex % (RadialResolution * AngularResolution)) / AngularResolution;
}


int FLinearToCylindricalMapper::GetHeightIndexFromLinearIndex(const int LinearIndex) const
{
	if (WillAlwaysBeZero() ||
		RadialResolution == 0 ||
		AngularResolution == 0 ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0;
	}


	return LinearIndex / (RadialResolution * AngularResolution);
}


int FLinearToCylindricalMapper::GetLinearIndexFromCylindricalIndices(const int AngularIndex, const int RadialIndex, const int HeightIndex) const
{
	if (IsValidAngularIndex(AngularIndex) == false)
	{
		L_WARN(MEOHelpers, "AngularIndex {0} outside range {1}", AngularIndex, AngularResolution);
		return 0;
	}

	else if (IsValidRadialIndex(RadialIndex) == false)
	{
		L_WARN(MEOHelpers, "RadialIndex {0} outside range {1}", RadialIndex, RadialResolution);
		return 0;
	}

	else if (IsValidHeightIndex(HeightIndex) == false)
	{
		L_WARN(MEOHelpers, "HeightIndex {0} outside range {1}", HeightIndex, HeightResolution);
		return 0;
	}


	return	GetLinearIndexFromAngularIndex(AngularIndex) +
			GetLinearIndexFromRadialIndex(RadialIndex) +
			GetLinearIndexFromHeightIndex(HeightIndex);
}


int FLinearToCylindricalMapper::GetLinearIndexFromAngularIndex(const int AngularIndex) const
{
	if (WillAlwaysBeZero() ||
		IsValidAngularIndex(AngularIndex) == false)
	{
		L_WARN(MEOHelpers, "Angular Index {0} outside range {1}", AngularIndex, GetLinearRange());
		return 0;
	}


	return AngularIndex % AngularResolution;
}


int FLinearToCylindricalMapper::GetLinearIndexFromRadialIndex(const int RadialIndex) const
{
	if (WillAlwaysBeZero() ||
		IsValidRadialIndex(RadialIndex) == false)
	{
		L_WARN(MEOHelpers, "Radial Index {0} outside range {1}", RadialIndex, GetLinearRange());
		return 0;
	}


	return RadialIndex * AngularResolution;
}


int FLinearToCylindricalMapper::GetLinearIndexFromHeightIndex(const int HeightIndex) const
{
	if (WillAlwaysBeZero() ||
		IsValidHeightIndex(HeightIndex) == false)
	{
		L_WARN(MEOHelpers, "Height Index {0} outside range {1}", HeightIndex, GetLinearRange());
		return 0;
	}


	return HeightIndex * AngularResolution * RadialResolution;
}


float FLinearToCylindricalMapper::RadialDistance(const int LinearIndex) const
{
	if (WillAlwaysBeZero() || 
		RadialResolution == 0 ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0.f;
	}

	const int RadialIndex = GetRadialIndexFromLinearIndex(LinearIndex);
	const float DistancePerIndex = 1.f / RadialResolution;


	return DistancePerIndex * (RadialIndex);
}


float FLinearToCylindricalMapper::ThetaAngle(const int LinearIndex) const
{
	if (WillAlwaysBeZero() || 
		AngularResolution == 0 ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0.f;
	}

	const int AngularIndex = GetAngularIndexFromLinearIndex(LinearIndex);
	const float AnglesPerIndex = UMathTypes::c_Circle_Degrees / AngularResolution;


	return AnglesPerIndex * AngularIndex + AngularOffset;
}


float FLinearToCylindricalMapper::HeightDistance(const int LinearIndex) const
{
	if (WillAlwaysBeZero() ||
		HeightResolution == 0 ||
		IsValidLinearIndex(LinearIndex) == false)
	{
		L_WARN(MEOHelpers, "Linear Index {0} outside range {1}", LinearIndex, GetLinearRange());
		return 0.f;
	}

	const int HeightIndex = GetHeightIndexFromLinearIndex(LinearIndex);
	const float DistancePerIndex = 1.f / HeightResolution;


	return DistancePerIndex * HeightIndex;
}


FVector2D FLinearToCylindricalMapper::GetCartesianCoordinate2D(const int LinearIndex) const
{
	return UMathTypes::ConvertPolarCoordinateToCartesian2D(
		GetPolarCoordinate2D(LinearIndex)
	);
}


FVector FLinearToCylindricalMapper::GetCartesianCoordinate(const int LinearIndex) const
{
	return UMathTypes::ConvertPolarCoordinateToCartesian(
		GetPolarCoordinate(LinearIndex)
	);
}