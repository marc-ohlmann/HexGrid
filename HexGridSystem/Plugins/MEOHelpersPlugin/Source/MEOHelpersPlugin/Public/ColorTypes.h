// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MathTypes.h"
#include "Math/Color.h"
#include "ColorTypes.generated.h"


// a struct that helps generate harmonies of colors
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FColorHarmony 
{
	GENERATED_BODY()

public:

	FColorHarmony()
		: Colors()
	{
	}

	static FColorHarmony GenerateColorHarmony(
		const int NumberOfColors,
		const float ColorHueOffset,
		const float Saturation = 1.f,
		const float Lightness = 1.f
	);

	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> Colors;

};


// a struct that helps generate harmonies of colors
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FColorHarmonyGenerator : public FLinearToCylindricalMapper
{
	GENERATED_BODY()

public:

	FColorHarmonyGenerator(
		const float ColorHueOffset, 
		const int NumberOfColors, 
		const int SaturationResolution, 
		const int LightnessResolution
	)
		: FLinearToCylindricalMapper(
			ColorHueOffset, 
			NumberOfColors, 
			SaturationResolution, 
			LightnessResolution)
	{
	}

	FColorHarmonyGenerator(const float ColorHueOffset, const int NumberOfColors)
		: FLinearToCylindricalMapper(ColorHueOffset, NumberOfColors, 
			c_DefaultSaturationLevels, 
			c_DefaultLightnessLevels)
	{
	}

	FColorHarmonyGenerator()
		: FColorHarmonyGenerator(0.f, 6)
	{
	}

	// generates a set of colors from the parameters
	FColorHarmony GenerateColorHarmony(const int SaturationIndex, const int LightnessIndex) const;

	// generates ALL color harmonies for the given mapping parameters
	TArray<FColorHarmony> GenerateAllColorHarmonies() const;

	// generates ALL colors for the given mapping parameters
	TArray<FLinearColor> GenerateFullColorList() const;

	// converts the given polar coordinate into a linear color using an HSV color coordinate system
	static FLinearColor GetColorFromPolarCoordinate(const FVector PolarCoordinate, const float Alpha = 1.f);

	// converts the given linear index into a linear color
	FLinearColor GetColorFromLinearIndex(const int LinearIndex, const float Alpha = 1.f) const
	{
		return GetColorFromPolarCoordinate(
			GetPolarCoordinate(LinearIndex),
			Alpha
		);
	}

	// the number of saturation levels to use as the radial resolution
	static const int c_DefaultSaturationLevels;

	// the number of lightness levels to use as the height resolution
	static const int c_DefaultLightnessLevels;

};


/**
 * Functions and constants for color types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UColorTypes : public UObject
{
	GENERATED_BODY()

public:

	// make a list of colors in a harmony with the given number of desired colors
	static TArray<FLinearColor> MakeHarmonyOfColors(const int NumberOfColors, FName const& Seed = FName());

};


