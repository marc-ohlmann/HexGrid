// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ColorTypes.h"
#include "MusicTypes.h"
#include "ArtTypes.generated.h"


// a music tone that has an associated color
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FColoredMusicNote : public FMusicNote
{
	GENERATED_BODY()

public:

	FColoredMusicNote(const FMusicNote& InNote, FLinearColor const& InColor)
		: FMusicNote(InNote)
		, Color(InColor)
	{
	}

	FColoredMusicNote()
		: FColoredMusicNote(
			FMusicNote(
				EMusicNoteToneType::rest, 
				UMusicTypes::c_MiddleOctave,
				EMusicNoteDurationType::whole,
				1.f
			), 
			FColoredMusicNote::c_MusicToneColor_Rest
		)
	{
	}

	UPROPERTY(EditAnywhere)
	FLinearColor Color;

	// the color of a rest tone
	static const FLinearColor c_MusicToneColor_Rest;

	// get a music note octave index from the given color lightness value
	static int GetOctaveFromLightness(float Lightness)
	{
		if (Lightness < 0.f) Lightness = 0.f;
		else if (Lightness > 1.f) Lightness = 1.f;
		return FMath::FloorToInt(Lightness * UMusicTypes::c_HighestAudibleOctave);
	}

	// get a color lightness value from the corresponding music note octave
	static float GetLightnessFromOctave(const int Octave)
	{
		float Lightness = static_cast<float>(Octave) / UMusicTypes::c_HighestAudibleOctave;
		if (Lightness < 0.f) Lightness = 0.f;
		else if (Lightness > 1.f) Lightness = 1.f;


		return Lightness;
	}

	// get a music note volume value from the given color saturation
	static float GetVolumeFromSaturation(const float Saturation)
	{
		float VolumeSaturation = Saturation; // one-to-one
		if (VolumeSaturation > 1.f) VolumeSaturation = 1.f;
		else if (VolumeSaturation < 0.f) VolumeSaturation = 0.f;

		return VolumeSaturation;
	}

	// get a color saturation value from the given music note volume
	static float GetSaturationFromVolume(const float Volume)
	{
		float VolumeSaturation = Volume; // one-to-one
		if (VolumeSaturation > 1.f) VolumeSaturation = 1.f;
		else if (VolumeSaturation < 0.f) VolumeSaturation = 0.f;

		return VolumeSaturation;
	}

};


// a list of colored music tones
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FColoredMusicScale
{
	GENERATED_BODY()

public:

	FColoredMusicScale()
		: ColoredNotes()
	{
	}

	UPROPERTY(EditAnywhere)
	TArray<FColoredMusicNote> ColoredNotes;

	bool IsNoteIndexValid(const int Index) const;
	bool ContainsNote(const EMusicNoteToneType Tone) const;
	int GetNoteIndex(const EMusicNoteToneType Tone) const;
	FColoredMusicNote GetNote(const EMusicNoteToneType Tone) const;

	// generates a set of colored music notes using the given scale, root tone, color hue offset, octave, and volume
	// octave sets the lightness
	// volume sets the saturation
	// the note durations always come out as whole notes
	static FColoredMusicScale GenerateColoredMusicScaleFromSound(
		const EMusicScale Scale,
		const EMusicNoteToneType ScaleRootToneType = EMusicNoteToneType::C,
		const int Octave = UMusicTypes::c_MiddleOctave,
		const float Volume = 1.f,
		const float ColorHueOffset = 0.f
	);

	// generates a set of colored music notes using the given scale, root tone, color hue offset, saturation, and lightness
	// lightness determines the octave (with pretty low resolution
	// saturation sets the volume
	// the note durations always come out as whole notes
	static FColoredMusicScale GenerateColoredMusicScaleFromColor(
		const EMusicScale Scale,
		const EMusicNoteToneType ScaleRootToneType = EMusicNoteToneType::C,
		const float Saturation = 1.f,
		const float Lightness = 1.f,
		const float ColorHueOffset = 0.f
	);

};


/**
 * Functions and constants for art types
 * 
 * Used for creating associations between types related to artistic assets
 */
UCLASS()
class MEOHELPERSPLUGIN_API UArtTypes : public UObject
{
	GENERATED_BODY()

public:

	

};