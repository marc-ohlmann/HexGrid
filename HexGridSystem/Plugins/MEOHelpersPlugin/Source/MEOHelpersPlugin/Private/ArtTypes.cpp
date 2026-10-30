// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "ArtTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"


const FLinearColor FColoredMusicNote::c_MusicToneColor_Rest = FLinearColor::White;


bool FColoredMusicScale::IsNoteIndexValid(const int Index) const
{
	return Index >= 0 && Index < ColoredNotes.Num();
}


int FColoredMusicScale::GetNoteIndex(const EMusicNoteToneType Tone) const
{
	FOR_FORWARD(i, ColoredNotes.Num())
	{
		if (ColoredNotes[i].Tone == Tone)
		{
			return i;
		}
	}


	return -1;
}


bool FColoredMusicScale::ContainsNote(const EMusicNoteToneType Tone) const
{
	return IsNoteIndexValid(GetNoteIndex(Tone));
}


FColoredMusicNote FColoredMusicScale::GetNote(const EMusicNoteToneType Tone) const
{
	if (ContainsNote(Tone) == false)
	{
		L_ERR(MEOHelpers, "Tone '{0}' not found", Tone);
		return FColoredMusicNote();
	}


	return ColoredNotes[GetNoteIndex(Tone)];
}


FColoredMusicScale FColoredMusicScale::GenerateColoredMusicScaleFromColor(
	const EMusicScale Scale,
	const EMusicNoteToneType ScaleRootToneType,
	const float Saturation,
	const float Lightness,
	const float ColorHueOffset
)
{
	// calculate octave from lightness
	const int LightnessOctave = FColoredMusicNote::GetOctaveFromLightness(Lightness);

	// calculate volume from saturation
	const float SaturationVolume = FColoredMusicNote::GetVolumeFromSaturation(Saturation);

	return FColoredMusicScale::GenerateColoredMusicScaleFromSound(
		Scale,
		ScaleRootToneType,
		LightnessOctave,
		SaturationVolume,
		ColorHueOffset
	);
}


FColoredMusicScale FColoredMusicScale::GenerateColoredMusicScaleFromSound(
	const EMusicScale Scale,
	const EMusicNoteToneType ScaleRootToneType,
	const int Octave,
	const float Volume,
	const float ColorHueOffset
)
{
	FColoredMusicScale Out;

	// calculate saturation from volume
	const float VolumeSaturation = FColoredMusicNote::GetSaturationFromVolume(Volume);

	// calculate lightness from octave
	const float OctaveLightness = FColoredMusicNote::GetLightnessFromOctave(Octave);

	// generate set of colors for the harmony
	const FColorHarmony ColorHarmony = FColorHarmony::GenerateColorHarmony(
		UMusicTypes::c_NumberOfMusicTones,
		ColorHueOffset,
		VolumeSaturation,
		OctaveLightness
	);

	// construct the root tone
	const FMusicTone ScaleRootTone = FMusicTone(ScaleRootToneType, Octave);

	// generate notes in the given scale from the root tone
	const TArray<FMusicTone> ScaleTones = UMusicTypes::GetMusicTonesInScale(Scale, ScaleRootTone);
	FOR_FORWARD(i, ScaleTones.Num())
	{
		const int ColorHarmonyIndex = ScaleRootTone.GetSemitoneDistanceFrom(ScaleTones[i]) % UMusicTypes::c_NumberOfMusicTones;
		if (UMCommon::IsValidIndex(ColorHarmony.Colors, ColorHarmonyIndex) == false)
		{
			L_WARN(MEOHelpers, "invalid index {0} for color harmony of size {1}", ColorHarmonyIndex, ColorHarmony.Colors.Num());
			continue;
		}

		FLinearColor MusicToneColor;
		if (ScaleTones[i].Octave != Octave)
		{
			// if new octave does not match, regenerate new color harmony for the new octave
			// calculate new lightness from tone octave
			const float NewOctaveLightness = FColoredMusicNote::GetLightnessFromOctave(ScaleTones[i].Octave);

			// generate set of colors for the harmony
			const FColorHarmony NewColorHarmony = FColorHarmony::GenerateColorHarmony(
				UMusicTypes::c_NumberOfMusicTones,
				ColorHueOffset,
				VolumeSaturation,
				NewOctaveLightness
			);

			MusicToneColor = NewColorHarmony.Colors[ColorHarmonyIndex];
		}
		else
		{
			MusicToneColor = ColorHarmony.Colors[ColorHarmonyIndex];
		}

		const FColoredMusicNote NewColoredMusicTone = FColoredMusicNote(
			FMusicNote(
				ScaleTones[i].Tone,
				ScaleTones[i].Octave,
				EMusicNoteDurationType::whole,
				VolumeSaturation
			),
			MusicToneColor
		);

		L_VERBOSE(MEOHelpers, "adding colored music tone {tone: '{0}{1}', color: '{1}",
			NewColoredMusicTone.Tone,
			NewColoredMusicTone.Octave,
			NewColoredMusicTone.Color
		);

		Out.ColoredNotes.Add(NewColoredMusicTone);
	}

	if (Out.ColoredNotes.Num() != ScaleTones.Num())
	{
		L_WARN(MEOHelpers, "generated incorrect number of colored tones ({0}) from {1} tones for scale '{2}' with root tone '{3}', VolumeSaturation '{4}', OctaveLightness '{5}', at color hue offset '{6}'",
			Out.ColoredNotes.Num(),
			ScaleTones.Num(),
			Scale,
			ScaleRootTone.ToneToString(),
			VolumeSaturation,
			OctaveLightness,
			ColorHueOffset
		);
	}
	else
	{
		L_VERBOSE(MEOHelpers, "created harmony from scale '{0}' with root tone '{1}', VolumeSaturation '{2}', OctaveLightness '{3}' (Octave: {4}), color hue offset '{5}' with {6} colored tones",
			Scale,
			ScaleRootTone.ToneToString(),
			VolumeSaturation,
			OctaveLightness,
			Octave,
			ColorHueOffset,
			Out.ColoredNotes.Num()
		);
	}


	return Out;
}