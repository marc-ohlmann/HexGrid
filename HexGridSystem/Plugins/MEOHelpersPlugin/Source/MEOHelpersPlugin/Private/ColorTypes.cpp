// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "ColorTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"

#include "Kismet/KismetMathLibrary.h"


const int FColorHarmonyGenerator::c_DefaultSaturationLevels = 256;
const int FColorHarmonyGenerator::c_DefaultLightnessLevels = 256;


FColorHarmony FColorHarmonyGenerator::GenerateColorHarmony(const int SaturationIndex, const int LightnessIndex) const
{
	if (IsValidRadialIndex(SaturationIndex) == false)
	{
		L_WARN(MEOHelpers, "SaturationIndex '{0}' out of range '{1}'", SaturationIndex, RadialResolution);
		return FColorHarmony();
	}

	if (IsValidHeightIndex(LightnessIndex) == false)
	{
		L_WARN(MEOHelpers, "LightnessIndex '{0}' out of range '{1}'", LightnessIndex, HeightResolution);
		return FColorHarmony();
	}

	FColorHarmony Out = FColorHarmony();
	const int LinearIndexBegin = GetLinearIndexFromCylindricalIndices(0, SaturationIndex, LightnessIndex);
	const int LinearIndexEnd = LinearIndexBegin + AngularResolution;
	for (int LinearIndexIterator = LinearIndexBegin; LinearIndexIterator < LinearIndexEnd; LinearIndexIterator++)
	{
		Out.Colors.Add(GetColorFromLinearIndex(LinearIndexIterator));
	}


	return Out;
}


FLinearColor FColorHarmonyGenerator::GetColorFromPolarCoordinate(const FVector PolarCoordinate, const float Alpha)
{
	FLinearColor Out;
	UKismetMathLibrary::LinearColor_SetFromHSV(Out, 
		PolarCoordinate.Y, // theta
		PolarCoordinate.X, // r, 
		PolarCoordinate.Z, // h
		Alpha
	);


	return Out;
}


TArray<FColorHarmony> FColorHarmonyGenerator::GenerateAllColorHarmonies() const
{
	TArray<FColorHarmony> Out;

	FOR_FORWARD(SaturationIndexIterator, RadialResolution)
	{
		FOR_FORWARD(LightnessIndexIterator, HeightResolution)
		{
			Out.Add(GenerateColorHarmony(SaturationIndexIterator, LightnessIndexIterator));
		}
	}


	return Out;
}


TArray<FLinearColor> FColorHarmonyGenerator::GenerateFullColorList() const
{
	const TArray<FColorHarmony> AllHarmonies = GenerateAllColorHarmonies();
	TArray<FLinearColor> Out;

	FOR_FORWARD(i, AllHarmonies.Num())
	{
		Out.Append(AllHarmonies[i].Colors);

	}


	return Out;
}



FColorHarmony FColorHarmony::GenerateColorHarmony(
	const int NumberOfColors,
	const float ColorHueOffset,
	const float Saturation,
	const float Lightness
)
{
	static const int MaxSaturationIndex = FColorHarmonyGenerator::c_DefaultSaturationLevels - 1;
	static const int MaxLightnessIndex = FColorHarmonyGenerator::c_DefaultLightnessLevels - 1;
	int SaturationIndex = FMath::FloorToInt(Saturation * MaxSaturationIndex);
	if (SaturationIndex < 0) SaturationIndex = 0;
	else if (SaturationIndex > MaxSaturationIndex) SaturationIndex = MaxSaturationIndex;

	int LightnessIndex = FMath::FloorToInt(Lightness * MaxLightnessIndex);
	if (LightnessIndex < 0) LightnessIndex = 0;
	else if (LightnessIndex > MaxLightnessIndex) LightnessIndex = MaxLightnessIndex;

	return FColorHarmonyGenerator(
		ColorHueOffset,
		NumberOfColors,
		FColorHarmonyGenerator::c_DefaultSaturationLevels,
		FColorHarmonyGenerator::c_DefaultLightnessLevels
	).GenerateColorHarmony(
		SaturationIndex,
		LightnessIndex
	);
}


TArray<FLinearColor> UColorTypes::MakeHarmonyOfColors(const int NumberOfColors, FName const& Seed)
{
	const float ColorHueOffset = UMathTypes::GetRandomFloat(0.f, UMathTypes::c_Circle_Degrees, Seed);
	static const int SaturationIndex = FColorHarmonyGenerator::c_DefaultSaturationLevels - 1;
	static const int LightnessIndex = FColorHarmonyGenerator::c_DefaultLightnessLevels - 1;

	const FColorHarmonyGenerator PlayerColorHarmonyGenerator = FColorHarmonyGenerator(ColorHueOffset, NumberOfColors);
	const FColorHarmony PlayerColorHarmony = PlayerColorHarmonyGenerator.GenerateColorHarmony(SaturationIndex, LightnessIndex);
	if (NumberOfColors != PlayerColorHarmony.Colors.Num())
	{
		L_ERR(MEOHelpers, "color list count '{0}' does not match number of desired colors '{1}'", NumberOfColors, PlayerColorHarmony.Colors.Num());
		return TArray<FLinearColor>();
	}


	return PlayerColorHarmony.Colors;
}
