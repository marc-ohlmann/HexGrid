// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "MusicTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "TimeTypes.h"


const int UMusicTypes::c_NumberOfMusicTones = NUMBER_OF_MUSIC_NOTES;
const int UMusicTypes::c_HighestAudibleOctave = 9;
const int UMusicTypes::c_MiddleOctave = c_HighestAudibleOctave / 2;

const float UMusicTypes::c_MusicToneFrequency_C0 = MUSIC_NOTE_FREQUENCY_HERTZ_C0;
const float UMusicTypes::c_MusicToneFrequency_Db0 = MUSIC_NOTE_FREQUENCY_HERTZ_Db0;
const float UMusicTypes::c_MusicToneFrequency_D0 = MUSIC_NOTE_FREQUENCY_HERTZ_D0;
const float UMusicTypes::c_MusicToneFrequency_Eb0 = MUSIC_NOTE_FREQUENCY_HERTZ_Eb0;
const float UMusicTypes::c_MusicToneFrequency_E0 = MUSIC_NOTE_FREQUENCY_HERTZ_E0;
const float UMusicTypes::c_MusicToneFrequency_F0 = MUSIC_NOTE_FREQUENCY_HERTZ_F0;
const float UMusicTypes::c_MusicToneFrequency_Gb0 = MUSIC_NOTE_FREQUENCY_HERTZ_Gb0;
const float UMusicTypes::c_MusicToneFrequency_G0 = MUSIC_NOTE_FREQUENCY_HERTZ_G0;
const float UMusicTypes::c_MusicToneFrequency_Ab0 = MUSIC_NOTE_FREQUENCY_HERTZ_Ab0;
const float UMusicTypes::c_MusicToneFrequency_A0 = MUSIC_NOTE_FREQUENCY_HERTZ_A0;
const float UMusicTypes::c_MusicToneFrequency_Bb0 = MUSIC_NOTE_FREQUENCY_HERTZ_Bb0;
const float UMusicTypes::c_MusicToneFrequency_B0 = MUSIC_NOTE_FREQUENCY_HERTZ_B0;

const float UMusicTypes::c_MusicTempoBpm_Center_Grave = TEMPO_BPM__CENTER__GRAVE;
const float UMusicTypes::c_MusicTempoBpm_Upper__Grave = TEMPO_BPM__UPPER___GRAVE;

const float UMusicTypes::c_MusicTempoBpm_Center_Lento = TEMPO_BPM__CENTER__LENTO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Lento = TEMPO_BPM__UPPER___LENTO;

const float UMusicTypes::c_MusicTempoBpm_Center_Largo = TEMPO_BPM__CENTER__LARGO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Largo = TEMPO_BPM__UPPER___LARGO;

const float UMusicTypes::c_MusicTempoBpm_Center_Larghetto = TEMPO_BPM__CENTER__LARGHETTO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Larghetto = TEMPO_BPM__UPPER___LARGHETTO;

const float UMusicTypes::c_MusicTempoBpm_Center_Adagio = TEMPO_BPM__CENTER__ADAGIO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Adagio = TEMPO_BPM__UPPER___ADAGIO;

const float UMusicTypes::c_MusicTempoBpm_Center_Adagietto = TEMPO_BPM__CENTER__ADAGIETTO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Adagietto = TEMPO_BPM__UPPER___ADAGIETTO;

const float UMusicTypes::c_MusicTempoBpm_Center_Andante = TEMPO_BPM__CENTER__ANDANTE;
const float UMusicTypes::c_MusicTempoBpm_Upper__Andante = TEMPO_BPM__UPPER___ANDANTE;

const float UMusicTypes::c_MusicTempoBpm_Center_Moderato = TEMPO_BPM__CENTER__MODERATO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Moderato = TEMPO_BPM__UPPER___MODERATO;

const float UMusicTypes::c_MusicTempoBpm_Center_Allegretto = TEMPO_BPM__CENTER__ALLEGRETTO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Allegretto = TEMPO_BPM__UPPER___ALLEGRETTO;

const float UMusicTypes::c_MusicTempoBpm_Center_Allegro = TEMPO_BPM__CENTER__ALLEGRO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Allegro = TEMPO_BPM__UPPER___ALLEGRO;

const float UMusicTypes::c_MusicTempoBpm_Center_Vivace = TEMPO_BPM__CENTER__VIVACE;
const float UMusicTypes::c_MusicTempoBpm_Upper__Vivace = TEMPO_BPM__UPPER___VIVACE;

const float UMusicTypes::c_MusicTempoBpm_Center_Presto = TEMPO_BPM__CENTER__PRESTO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Presto = TEMPO_BPM__UPPER___PRESTO;

const float UMusicTypes::c_MusicTempoBpm_Center_Prestissimo = TEMPO_BPM__CENTER__PRESTISSIMO;
const float UMusicTypes::c_MusicTempoBpm_Upper__Prestissimo = TEMPO_BPM__UPPER___PRESTISSIMO;


const EMusicNoteDurationType UMusicTypes::c_SinglePulseDuration = EMusicNoteDurationType::thirtysecond; // set to the smallest possible duration enum value
const int UMusicTypes::c_LargestSupportedPulsesPerBeat = UMusicTypes::GetMusicNoteDurationTypeInvertedBeatWeight(UMusicTypes::c_SinglePulseDuration);
const float UMusicTypes::c_LargestSupportedPulseMinutesPerBeatSeconds = (static_cast<float>(UMusicTypes::c_LargestSupportedPulsesPerBeat)) * UTimeTypes::c_MinutesPerSecondRatio;

const FMusicTone UMusicTypes::c_MiddleC = FMusicTone(EMusicNoteToneType::C, c_MiddleOctave);

const float UMusicTypes::c_SemitoneFrequencyRatio = SEMITONE_FREQUENCY_RATIO;
const float UMusicTypes::c_InverseSemitoneFrequencyRatio = INVERSE_SEMITONE_FREQUENCY_RATIO;

const float UMusicTypes::c_HzToBpmRatio = UTimeTypes::c_SecondsPerMinuteRatio;
const float UMusicTypes::c_BpmToHzRatio = 1.f / c_HzToBpmRatio;


const TMap<int, EQuartzTimeSignatureQuantization> FMusicTimeSignature::c_BeatsPerWholeNoteTimeSignatureQuantizationMap =
{
	{2, EQuartzTimeSignatureQuantization::HalfNote},
	{4, EQuartzTimeSignatureQuantization::QuarterNote},
	{8, EQuartzTimeSignatureQuantization::EighthNote},
	{16, EQuartzTimeSignatureQuantization::SixteenthNote},
	{32, EQuartzTimeSignatureQuantization::ThirtySecondNote},
};


float UMusicTypes::GetMusicToneBaseFrequency(const EMusicNoteToneType Note)
{
	switch (Note)
	{


	case EMusicNoteToneType::rest:
		return 0.f;


	case EMusicNoteToneType::A:
		return UMusicTypes::c_MusicToneFrequency_A0;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return UMusicTypes::c_MusicToneFrequency_Bb0;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return UMusicTypes::c_MusicToneFrequency_B0;

	case EMusicNoteToneType::Bs:
	case EMusicNoteToneType::C:
		return UMusicTypes::c_MusicToneFrequency_C0;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return UMusicTypes::c_MusicToneFrequency_Db0;

	case EMusicNoteToneType::D:
		return UMusicTypes::c_MusicToneFrequency_D0;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return UMusicTypes::c_MusicToneFrequency_Eb0;

	case EMusicNoteToneType::E:
	case EMusicNoteToneType::Fb:
		return UMusicTypes::c_MusicToneFrequency_E0;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return UMusicTypes::c_MusicToneFrequency_F0;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return UMusicTypes::c_MusicToneFrequency_Gb0;

	case EMusicNoteToneType::G:
		return UMusicTypes::c_MusicToneFrequency_G0;

	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return UMusicTypes::c_MusicToneFrequency_Ab0;


	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return 0.f;
}


float UMusicTypes::GetMusicToneFrequency(const EMusicNoteToneType Note, const int Octave)
{
	if (Octave < 0 || 
		Note == EMusicNoteToneType::rest)
	{
		return 0.f;
	}

	// Get the semitone index (C = 0, C# = 1 ... B = 11)
	const int SemitoneIndex = GetSemitoneDistanceToOctaveRoot(Note);
	if (SemitoneIndex < 0)
	{
		return 0.f;
	}

	// Calculate the absolute number of semitones from C0
	const int TotalSemitonesFromC0 = (Octave * c_NumberOfMusicTones) + SemitoneIndex;

	// A4 is 4 octaves up (48) + 9 semitones (A) = 57 semitones from C0
	const int SemitonesFromA4 = TotalSemitonesFromC0 - 57;


	// Apply the Equal Temperament formula, which calculates the exact frequency relative to A4 (440 Hz)
	return 440.0f * FMath::Pow(2.0f, static_cast<float>(SemitonesFromA4) / 12.0f);
}


bool UMusicTypes::AreMusicTonesEquivalent(const EMusicNoteToneType NoteA, const EMusicNoteToneType NoteB)
{
	return UMusicTypes::NormalizeMusicTone(NoteA) == UMusicTypes::NormalizeMusicTone(NoteB);
}


EMusicNoteToneType UMusicTypes::MakeMusicToneSharp(const EMusicNoteToneType Note)
{
	switch (Note)
	{


	case EMusicNoteToneType::rest:
		return EMusicNoteToneType::rest;


	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return EMusicNoteToneType::A;

	case EMusicNoteToneType::A:
		return EMusicNoteToneType::Bb;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return EMusicNoteToneType::B;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return EMusicNoteToneType::C;

	case EMusicNoteToneType::Bs:
	case EMusicNoteToneType::C:
		return EMusicNoteToneType::Db;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return EMusicNoteToneType::D;

	case EMusicNoteToneType::D:
		return EMusicNoteToneType::Eb;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return EMusicNoteToneType::E;

	case EMusicNoteToneType::E:
	case EMusicNoteToneType::Fb:
		return EMusicNoteToneType::F;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return EMusicNoteToneType::Gb;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return EMusicNoteToneType::G;

	case EMusicNoteToneType::G:
		return EMusicNoteToneType::Ab;

	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return EMusicNoteToneType::rest;
}


EMusicNoteToneType UMusicTypes::MakeMusicToneFlat(const EMusicNoteToneType Note)
{
	switch (Note)
	{


	case EMusicNoteToneType::rest:
		return EMusicNoteToneType::rest;


	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return EMusicNoteToneType::G;

	case EMusicNoteToneType::A:
		return EMusicNoteToneType::Ab;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return EMusicNoteToneType::A;

	case EMusicNoteToneType::C:
	case EMusicNoteToneType::Bs:
		return EMusicNoteToneType::B;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return EMusicNoteToneType::Bb;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return EMusicNoteToneType::C;

	case EMusicNoteToneType::D:
		return EMusicNoteToneType::Db;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return EMusicNoteToneType::D;

	case EMusicNoteToneType::Fb:
	case EMusicNoteToneType::E:
		return EMusicNoteToneType::Eb;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return EMusicNoteToneType::E;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return EMusicNoteToneType::F;

	case EMusicNoteToneType::G:
		return EMusicNoteToneType::Gb;

	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return EMusicNoteToneType::rest;
}


EMusicNoteToneType UMusicTypes::MakeMusicToneNatural(const EMusicNoteToneType Note)
{
	switch (Note)
	{


	case EMusicNoteToneType::rest:
		return EMusicNoteToneType::rest;


	case EMusicNoteToneType::Ab:
	case EMusicNoteToneType::A:
	case EMusicNoteToneType::As:
		return EMusicNoteToneType::A;


	case EMusicNoteToneType::Bb:
	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Bs:
		return EMusicNoteToneType::B;


	case EMusicNoteToneType::Cb:
	case EMusicNoteToneType::C:
	case EMusicNoteToneType::Cs:
		return EMusicNoteToneType::C;


	case EMusicNoteToneType::Db:
	case EMusicNoteToneType::D:
	case EMusicNoteToneType::Ds:
		return EMusicNoteToneType::D;


	case EMusicNoteToneType::Eb:
	case EMusicNoteToneType::E:
	case EMusicNoteToneType::Es:
		return EMusicNoteToneType::E;


	case EMusicNoteToneType::Fb:
	case EMusicNoteToneType::F:
	case EMusicNoteToneType::Fs:
		return EMusicNoteToneType::F;


	case EMusicNoteToneType::Gb:
	case EMusicNoteToneType::G:
	case EMusicNoteToneType::Gs:
		return EMusicNoteToneType::G;


	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return EMusicNoteToneType::rest;
}


EMusicNoteToneType UMusicTypes::IterateMusicToneForward(const EMusicNoteToneType Note)
{
	switch (Note)
	{

	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return EMusicNoteToneType::A;

	case EMusicNoteToneType::A:
		return EMusicNoteToneType::Bb;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return EMusicNoteToneType::B;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return EMusicNoteToneType::C;

	case EMusicNoteToneType::Bs:
	case EMusicNoteToneType::C:
		return EMusicNoteToneType::Db;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return EMusicNoteToneType::D;

	case EMusicNoteToneType::D:
		return EMusicNoteToneType::Eb;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return EMusicNoteToneType::E;

	case EMusicNoteToneType::E:
	case EMusicNoteToneType::Fb:
		return EMusicNoteToneType::F;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return EMusicNoteToneType::Gb;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return EMusicNoteToneType::G;

	case EMusicNoteToneType::G:
		return EMusicNoteToneType::rest;

	case EMusicNoteToneType::rest:
		return EMusicNoteToneType::Ab;

	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return EMusicNoteToneType::rest;
}


EMusicNoteToneType UMusicTypes::IterateMusicToneBackward(const EMusicNoteToneType Note)
{
	switch (Note)
	{
	case EMusicNoteToneType::rest:
		return EMusicNoteToneType::G;

	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return EMusicNoteToneType::rest;

	case EMusicNoteToneType::A:
		return EMusicNoteToneType::Ab;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return EMusicNoteToneType::A;

	case EMusicNoteToneType::C:
	case EMusicNoteToneType::Bs:
		return EMusicNoteToneType::B;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return EMusicNoteToneType::Bb;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return EMusicNoteToneType::C;

	case EMusicNoteToneType::D:
		return EMusicNoteToneType::Db;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return EMusicNoteToneType::D;

	case EMusicNoteToneType::Fb:
	case EMusicNoteToneType::E:
		return EMusicNoteToneType::Eb;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return EMusicNoteToneType::E;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return EMusicNoteToneType::F;

	case EMusicNoteToneType::G:
		return EMusicNoteToneType::Gb;

	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return EMusicNoteToneType::rest;
}


EMusicNoteDurationType UMusicTypes::IterateMusicDurationTypeForward(const EMusicNoteDurationType Note)
{
	if ((static_cast<int>(Note)) + 1 >= UMCommon::GetEnumSize<EMusicNoteDurationType>())
	{
		return static_cast<EMusicNoteDurationType>(0);
	}

	const EMusicNoteDurationType NewNote = static_cast<EMusicNoteDurationType>((static_cast<int>(Note)) + 1);


	return NewNote;
}


EMusicNoteDurationType UMusicTypes::IterateMusicDurationTypeBackward(const EMusicNoteDurationType Note)
{
	int NewNoteIndex = (static_cast<int>(Note)) - 1;
	if (NewNoteIndex < 0)
	{
		NewNoteIndex = (UMCommon::GetEnumSize<EMusicNoteDurationType>()) - 1;
	}


	return static_cast<EMusicNoteDurationType>(NewNoteIndex);
}


EMusicNoteToneType UMusicTypes::NormalizeMusicTone(const EMusicNoteToneType Note)
{
	// making a note flat and then sharp should always return an equivalent note
	// these functions prefer to represent notes as flats, so the result is either natural or flat
	return UMusicTypes::MakeMusicToneFlat(UMusicTypes::MakeMusicToneSharp(Note));
}


TArray<FMusicTone> UMusicTypes::GetMusicTonesInScale(const EMusicScale Scale, const FMusicTone RootNote)
{
	if (RootNote.IsRest())
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return TArray<FMusicTone>();
	}

	TArray<int> ScaleSemitoneAdjustments = UMusicTypes::GetScaleNoteSemitonalDifferentials(Scale);
	TArray<FMusicTone> OutNotes;

	FMusicTone NoteIterator = RootNote;
	NoteIterator.NormalizeTone();

	FOR_FORWARD(i, ScaleSemitoneAdjustments.Num())
	{
		NoteIterator.IncrementBySemitonalDistance(ScaleSemitoneAdjustments[i]);
		OutNotes.Add(NoteIterator);
	}


	return OutNotes;
}


float FMusicTone::GetFrequencyHertz() const
{
	return UMusicTypes::GetMusicToneFrequency(Tone, Octave);
}


int FMusicTone::GetToneId() const
{
	return UMusicTypes::GetSemitoneDistanceToOctaveRoot(UMusicTypes::NormalizeMusicTone(Tone)) +
		(Octave * UMusicTypes::c_NumberOfMusicTones);
}


FMusicTone& FMusicTone::MakeSharp()
{
	if (IsRest() == true)
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return *this;
	}

	if (UMusicTypes::NormalizeMusicTone(Tone) == EMusicNoteToneType::B)
	{
		Octave += 1;
	}

	Tone = UMusicTypes::MakeMusicToneSharp(Tone);


	return *this;
}


FMusicTone& FMusicTone::MakeFlat()
{
	if (IsRest() == true)
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return *this;
	}

	if (UMusicTypes::NormalizeMusicTone(Tone) == EMusicNoteToneType::C)
	{
		Octave -= 1;
	}

	Tone = UMusicTypes::MakeMusicToneFlat(Tone);

	if (Octave < 0)
	{
		L_WARN(MEOHelpers, "negative octave '{0}' for note '{1}'", Octave, Tone);
	}

	return *this;
}


FMusicTone& FMusicTone::MakeNatural()
{
	if (IsRest() == true)
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return *this;
	}

	if (Tone == EMusicNoteToneType::Bs)
	{
		Octave -= 1;
	}
	else if (Tone == EMusicNoteToneType::Cb)
	{
		Octave += 1;
	}

	Tone = UMusicTypes::MakeMusicToneNatural(Tone);

	if (Octave < 0)
	{
		L_WARN(MEOHelpers, "negative octave '{0}' for note '{1}'", Octave, Tone);
	}


	return *this;
}


FMusicTone& FMusicTone::NormalizeTone()
{
	Tone = UMusicTypes::NormalizeMusicTone(Tone);


	return *this;
}


FMusicTone& FMusicTone::IterateToneForward()
{
	Tone = UMusicTypes::IterateMusicToneForward(Tone);


	return *this;
}


FMusicTone& FMusicTone::IterateToneBackward()
{
	Tone = UMusicTypes::IterateMusicToneBackward(Tone);


	return *this;
}


FMusicNote& FMusicNote::IterateDurationForward()
{
	Duration = UMusicTypes::IterateMusicDurationTypeForward(Duration);


	return *this;
}


FMusicNote& FMusicNote::IterateDurationBackward()
{
	Duration = UMusicTypes::IterateMusicDurationTypeBackward(Duration);


	return *this;
}


FMusicNote FMusicNote::RestPulse(const int InOctave)
{
	return FMusicNote(EMusicNoteToneType::rest, InOctave, UMusicTypes::c_SinglePulseDuration);
}


bool FMusicTone::IsToneEqual(FMusicTone const& Other) const
{
	return Octave == Other.Octave &&
		UMusicTypes::AreMusicTonesEquivalent(Tone, Other.Tone) == true;
}


bool FMusicNote::IsEqual(FMusicNote const& Other) const
{
	return IsToneEqual(Other) &&
		Duration == Other.Duration;
}


FMusicTone& FMusicTone::IncrementByOctaveDistance(const int Octaves)
{
	Octave += Octaves;
	if (Octave < 0)
	{
		L_WARN(MEOHelpers, "negative octave '{0}' for note '{1}'", Octave, Tone);
	}


	return *this;
}


FMusicTone& FMusicTone::IncrementBySemitonalDistance(const int Semitones)
{
	if (IsRest() == true)
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return *this;
	}

	if (Semitones > 0)
	{
		const int IncrementedNoteSemitones = Semitones % UMusicTypes::c_NumberOfMusicTones;

		// octave
		int IncrementedOctaves = Semitones / UMusicTypes::c_NumberOfMusicTones;
		const int DistanceToHigherOctaveRoot = UMusicTypes::GetSemitoneDistanceToHigherOctaveRoot(Tone);
		if (DistanceToHigherOctaveRoot > 0 && Semitones % UMusicTypes::c_NumberOfMusicTones >= DistanceToHigherOctaveRoot)
		{
			IncrementedOctaves += 1;
		}
		if (IncrementedOctaves > 0)
		{
			Octave += IncrementedOctaves;
		}

		// note
		FOR_FORWARD(i, Semitones)
		{
			Tone = UMusicTypes::MakeMusicToneSharp(Tone);
		}
	}
	else if (Semitones < 0)
	{
		const int DecrementedNoteSemitones = -1 * (FMath::Abs(Semitones) % UMusicTypes::c_NumberOfMusicTones);

		// octave
		int DecrementedOctaves = Semitones / UMusicTypes::c_NumberOfMusicTones;
		const int DistanceToRoot = UMusicTypes::GetSemitoneDistanceToOctaveRoot(Tone);
		if (DistanceToRoot > 0 && FMath::Abs(DecrementedNoteSemitones) >= DistanceToRoot)
		{
			DecrementedOctaves += 1;
		}
		if (DecrementedOctaves > 0)
		{
			Octave -= DecrementedOctaves;
		}

		// note
		FOR_FORWARD(i, FMath::Abs(Semitones))
		{
			Tone = UMusicTypes::MakeMusicToneFlat(Tone);
		}

		if (Octave < 0)
		{
			L_WARN(MEOHelpers, "negative octave '{0}' for note '{1}'", Octave, Tone);
		}
	}


	return *this;
}


int UMusicTypes::GetSemitoneDistanceToOctaveRoot(const EMusicNoteToneType Note)
{
	switch (Note)
	{

	case EMusicNoteToneType::rest:
		L_WARN(MEOHelpers, "note '{0}' semitone distance is undefined", Note);
		break;

	case EMusicNoteToneType::C:
	case EMusicNoteToneType::Bs:
		return 0;

	case EMusicNoteToneType::Cs:
	case EMusicNoteToneType::Db:
		return 1;

	case EMusicNoteToneType::D:
		return 2;

	case EMusicNoteToneType::Ds:
	case EMusicNoteToneType::Eb:
		return 3;

	case EMusicNoteToneType::Fb:
	case EMusicNoteToneType::E:
		return 4;

	case EMusicNoteToneType::Es:
	case EMusicNoteToneType::F:
		return 5;

	case EMusicNoteToneType::Fs:
	case EMusicNoteToneType::Gb:
		return 6;

	case EMusicNoteToneType::G:
		return 7;

	case EMusicNoteToneType::Gs:
	case EMusicNoteToneType::Ab:
		return 8;

	case EMusicNoteToneType::A:
		return 9;

	case EMusicNoteToneType::As:
	case EMusicNoteToneType::Bb:
		return 10;

	case EMusicNoteToneType::B:
	case EMusicNoteToneType::Cb:
		return 11;


	default:
		L_ERR(MEOHelpers, "default on note '{0}'", Note);
		break;
	}


	return -1;
}


int UMusicTypes::GetSemitoneDistanceToHigherOctaveRoot(const EMusicNoteToneType Note)
{
	return (UMusicTypes::c_NumberOfMusicTones)-GetSemitoneDistanceToOctaveRoot(Note);
}


int FMusicTone::GetSemitoneDistanceFrom(FMusicTone const& Other) const
{
	if (IsRest() || Other.IsRest())
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return -1;
	}

	if (IsToneEqual(Other))
	{
		return 0;
	}

	const int OctaveDifference = Octave - Other.Octave;
	if (OctaveDifference == 0)
	{
		return UMusicTypes::GetSemitoneDistanceBetweenMusicTonesWithinOctave(Tone, Other.Tone);
	}
	else if (OctaveDifference > 0)
	{
		const int OtherToNextOctave = UMusicTypes::GetSemitoneDistanceToHigherOctaveRoot(Other.Tone);
		const int ThisToLowerOctave = UMusicTypes::GetSemitoneDistanceToOctaveRoot(Tone);
		const int InBetweenOctaves = (OctaveDifference - 1) * UMusicTypes::c_NumberOfMusicTones;


		return OtherToNextOctave + ThisToLowerOctave + InBetweenOctaves;
	}
	else if (OctaveDifference < 0)
	{
		const int ThisToNextOctave = UMusicTypes::GetSemitoneDistanceToHigherOctaveRoot(Other.Tone);
		const int OtherToLowerOctave = UMusicTypes::GetSemitoneDistanceToOctaveRoot(Tone);
		const int InBetweenOctaves = (OctaveDifference - 1) * UMusicTypes::c_NumberOfMusicTones;


		return (ThisToNextOctave + OtherToLowerOctave + InBetweenOctaves) * -1;
	}


	return 0;
}


int UMusicTypes::GetSemitoneDistanceBetweenMusicTonesWithinOctave(const EMusicNoteToneType NoteA, const EMusicNoteToneType NoteB)
{
	if (NoteA == EMusicNoteToneType::rest || NoteB == EMusicNoteToneType::rest)
	{
		L_WARN(MEOHelpers, "undefined behavior for rest notes");
		return -1;
	}

	const int DistanceFromLowerOctave_NoteA = UMusicTypes::GetSemitoneDistanceToOctaveRoot(NoteA);
	const int DistanceFromLowerOctave_NoteB = UMusicTypes::GetSemitoneDistanceToOctaveRoot(NoteB);


	return DistanceFromLowerOctave_NoteB - DistanceFromLowerOctave_NoteA;
}


FString FMusicTone::ToneToString() const
{
	return FString::Printf(TEXT("%s%d:'%fHz'"), *UMCommon::EnumToString(Tone), Octave, GetFrequencyHertz());
}


void FMusicTone::PrintTone() const
{
	L_VERBOSE(MEOHelpers, "{0}", ToneToString());
}


FString FMusicNote::ToString() const
{
	return FString::Printf(TEXT("{%s, %s}"), *UMCommon::EnumToString(Duration), *ToneToString());
}


void FMusicNote::Print() const
{
	L_VERBOSE(MEOHelpers, "{0}", ToString());
}


TArray<FMusicTone> FMusicTone::GetRootedScale(const EMusicScale Scale) const
{
	return UMusicTypes::GetMusicTonesInScale(Scale, *this);
}


bool FMusicChordNoteChannel::GetNoteAtPulse(const EMusicNoteDurationType PulseDurationType, const int PulseNumber, FMusicNote& OutNote) const
{
	if (PulseNumber == 0 &&
		NoteStream.Num() > 0 &&
		NoteStream[0].IsRest() == false)
	{
		OutNote = NoteStream[0];
		return true;
	}

	int BeatPulsesRemaining = PulseNumber;
	FOR_FORWARD(i, NoteStream.Num() - 1)
	{
		BeatPulsesRemaining -= UMusicTypes::GetMusicNoteDurationTypePulseCount(NoteStream[i].Duration, PulseDurationType);
		if (BeatPulsesRemaining == 0)
		{
			OutNote = NoteStream[i + 1];
			return true;
		}
		else if (BeatPulsesRemaining < 1)
		{
			break;
		}
	}


	return false;
}


int FMusicChordNoteChannel::GetTotalPulses(const EMusicNoteDurationType PulseDurationType) const
{
	int Pulses = 0;
	FOR_FORWARD(i, NoteStream.Num())
	{
		Pulses += UMusicTypes::GetMusicNoteDurationTypePulseCount(NoteStream[i].Duration, PulseDurationType);
	}


	return Pulses;
}


FMusicChordNoteChannel& FMusicChordNoteChannel::FillWithRests(const int DesiredPulseLength)
{
	const int PulsesToAdd = DesiredPulseLength - NoteStream.Num();
	if (PulsesToAdd > 0)
	{
		FOR_FORWARD(i, PulsesToAdd)
		{
			NoteStream.Add(FMusicNote::RestPulse());
		}
	}


	return *this;
}


int FMusicOrchestrationSection::GetInstrumentIndex(FName const& InstrumentName) const
{
	FOR_FORWARD(i, InstrumentCompositions.Num())
	{
		if (InstrumentCompositions[i].InstrumentName.IsEqual(InstrumentName))
		{
			return i;
		}
	}


	return -1;
}


bool FMusicOrchestrationSection::IsInstrumentIndexValid(const int InstrumentIndex) const
{
	return InstrumentIndex >= 0 && InstrumentIndex < InstrumentCompositions.Num();
}


bool FMusicOrchestrationSection::ContainsInstrument(FName const& InstrumentName) const
{
	return IsInstrumentIndexValid(GetInstrumentIndex(InstrumentName));
}


int FMusicOrchestrationSection::GetInstrumentNoteChannelCount(FName const& InstrumentName) const
{
	const int InstrumentIndex = GetInstrumentIndex(InstrumentName);
	if (IsInstrumentIndexValid(InstrumentIndex) == false)
	{
		return 0;
	}


	return InstrumentCompositions[InstrumentIndex].ChordNoteChannels.Num();
}


float FMusicOrchestrationSection::GetTotalPlaytimeSeconds(const EMusicNoteDurationType PulseDurationType) const
{
	const float PulsesPerSecond = Tempo * UTimeTypes::c_MinutesPerSecondRatio;
	if (PulsesPerSecond < 0.f || FMath::IsNearlyZero(PulsesPerSecond))
	{
		return 0.f;
	}

	const float PulseCount = static_cast<float>(GetTotalPulses(PulseDurationType));


	return PulseCount / PulsesPerSecond;
}


int FMusicTimeSignature::GetPulsesPerMeasure() const
{
	return BeatsPerMeasure * UMusicTypes::c_LargestSupportedPulsesPerBeat;
}


int FMusicTimeSignature::GetPulsesPerWholeNote() const
{
	return BeatsPerWholeNote * UMusicTypes::c_LargestSupportedPulsesPerBeat;
}


int FMusicTimeSignature::GetMeasuresPerWholeNote() const
{
	if (IsValid() == false) return 0;


	return BeatsPerWholeNote / BeatsPerMeasure;
}


int FMusicTimeSignature::GetWholeNotesPerMeasure() const
{
	if (IsValid() == false) return 0;


	return BeatsPerMeasure / BeatsPerWholeNote;
}


FMusicSongToneData FMusicSongToneChannel::GetPulseData(const int PulseIndex) const
{
	if (PulseIndex < 0 || PulseIndex >= PulseData.Num())
	{
		L_WARN(MEOHelpers, "invalid pulse index '{0}/{1}'", PulseIndex, PulseData.Num());
		return FMusicSongToneData::RestingToneChange(EMusicNoteDurationType::whole);
	}


	return PulseData[PulseIndex];
}


FMusicSongToneChannel& FMusicSongToneChannel::FillWithRests(const int TotalPulses, const EMusicNoteDurationType PulseDurationType)
{
	const int PulsesToAdd = TotalPulses - PulseData.Num();
	if (PulsesToAdd > 0)
	{
		FOR_FORWARD(i, PulsesToAdd)
		{
			PulseData.Add(FMusicSongToneData::RestingToneChange(PulseDurationType));
		}
	}


	return *this;
}


int FMusicSongTempoChangelist::GetHighestPulseIndexPriorTo(const int PulseIndex) const
{
	int HighestPulseIndex = -1;
	FOR_FORWARD(i, TempoChangelist.Num())
	{
		if (TempoChangelist[i].PulseIndex < PulseIndex &&
			HighestPulseIndex < TempoChangelist[i].PulseIndex)
		{
			HighestPulseIndex = TempoChangelist[i].PulseIndex;
		}
	}


	return HighestPulseIndex;
}


void FMusicSongTempoChangelist::RegisterSectionTempo(const int SectionPulseIndex, float SectionTempo)
{
	if (ContainsPulseIndex(SectionPulseIndex) == true)
	{
		return;
	}

	// reject if there is a previous pulse index and it's tempo matches
	float PreviousIndexTempo = 0.f;
	const int PreviousIndex = GetHighestPulseIndexPriorTo(SectionPulseIndex);
	if (PreviousIndex >= 0 &&
		CheckForTempoChange(PreviousIndex, PreviousIndexTempo) &&
		FMath::IsNearlyEqual(PreviousIndexTempo, SectionTempo))
	{
		return;
	}

	// otherwise add it
	TempoChangelist.Add(FMusicTempoChangePulseIndex(SectionPulseIndex, SectionTempo));


	return;
}


FMusicSong::FMusicSong(FMusicOrchestration const& InMusicOrchestration)
	:FMusicSong()
{
	// direct transfer of composition info
	Info = InMusicOrchestration.Info;

	// cache pulse duration
	_PulseDurationType = InMusicOrchestration.FindSmallestNoteDurationType();

	// cache playback time
	_PlaybackTimeSeconds = InMusicOrchestration.GetTotalPlaytimeSeconds();

	// cache tempo changes
	int SectionStartingPulseIndex = 0;
	FOR_FORWARD(SectionIterator, InMusicOrchestration.Sections.Num())
	{
		TempoChangelist.RegisterSectionTempo(SectionStartingPulseIndex, InMusicOrchestration.Sections[SectionIterator].Tempo);
		SectionStartingPulseIndex += InMusicOrchestration.Sections[SectionIterator].GetTotalPulses(_PulseDurationType);
	}

	// setup tone channel data structure
	const int VariableToneCount = InMusicOrchestration.GetMaximumRequiredVariableTones();
	ToneChannels.Reserve(VariableToneCount);

	const int TotalPulseCount = InMusicOrchestration.GetTotalPulses();
	FOR_FORWARD(i, VariableToneCount)
	{
		ToneChannels.Add(FMusicSongToneChannel());
		ToneChannels[i].PulseData.Reserve(TotalPulseCount);
	}

	// cache tone channel data
	// initialize tone channels using each section's set of instrument compositions
	// we dont want to just create new channels for every instrument, we want to recycle channels between sections
	FOR_FORWARD(SectionIterator, InMusicOrchestration.Sections.Num())
	{
		const int SectionMaxRequiredPulses = InMusicOrchestration.Sections[SectionIterator].GetTotalPulses(_PulseDurationType);

		// get all the parallel channel tone data
		TArray<FMusicSongToneChannel> ToneData;
		FOR_FORWARD(InstrumentIterator, InMusicOrchestration.Sections[SectionIterator].InstrumentCompositions.Num())
		{
			ToneData.Append(InMusicOrchestration.Sections[SectionIterator].InstrumentCompositions[InstrumentIterator].GenerateSongToneChannels(SectionMaxRequiredPulses, _PulseDurationType));
		}

		// place it in series data
		FOR_FORWARD(i, ToneData.Num())
		{
			if (i >= ToneChannels.Num())
			{
				L_ERR(MEOHelpers, "not enough tone channels '{0}' for section '{1}'", i, SectionIterator);
				break;
			}

			ToneChannels[i].PulseData.Append(ToneData[i].PulseData);
		}
	}
}


bool FMusicSong::GetPulsePlaybackData(const int PulseIndex, TArray<FMusicSongToneData>& OutToneData, float& OutTempoChange) const
{
	FOR_FORWARD(i, ToneChannels.Num())
	{
		OutToneData.Add(ToneChannels[i].GetPulseData(PulseIndex));
	}


	return TempoChangelist.CheckForTempoChange(PulseIndex, OutTempoChange);
}


float UMusicTypes::GetBpmFromTempoType(const EMusicTempo Tempo)
{
	switch (Tempo)
	{

	case EMusicTempo::grave:
		return c_MusicTempoBpm_Center_Grave;

	case EMusicTempo::lento:
		return c_MusicTempoBpm_Center_Lento;

	case EMusicTempo::largo:
		return c_MusicTempoBpm_Center_Largo;

	case EMusicTempo::larghetto:
		return c_MusicTempoBpm_Center_Larghetto;

	case EMusicTempo::adagio:
		return c_MusicTempoBpm_Center_Adagio;

	case EMusicTempo::adagietto:
		return c_MusicTempoBpm_Center_Adagietto;

	case EMusicTempo::andante:
		return c_MusicTempoBpm_Center_Andante;

	case EMusicTempo::moderato:
		return c_MusicTempoBpm_Center_Moderato;

	case EMusicTempo::allegretto:
		return c_MusicTempoBpm_Center_Allegretto;

	case EMusicTempo::allegro:
		return c_MusicTempoBpm_Center_Allegro;

	case EMusicTempo::vivace:
		return c_MusicTempoBpm_Center_Vivace;

	case EMusicTempo::presto:
		return c_MusicTempoBpm_Center_Presto;

	case EMusicTempo::prestissimo:
		return c_MusicTempoBpm_Center_Prestissimo;

	default:
		L_ERR(MEOHelpers, "default on tempo '{0}'", Tempo);
		break;
	}


	return 0.f;
}


EMusicTempo UMusicTypes::GetTempoTypeFromBpm(const float Bpm)
{
	if (Bpm > 0.f)
	{
		if (Bpm < c_MusicTempoBpm_Upper__Grave)
		{
			return EMusicTempo::grave;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Lento)
		{
			return EMusicTempo::lento;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Largo)
		{
			return EMusicTempo::largo;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Larghetto)
		{
			return EMusicTempo::larghetto;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Adagio)
		{
			return EMusicTempo::adagio;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Adagietto)
		{
			return EMusicTempo::adagietto;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Andante)
		{
			return EMusicTempo::andante;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Moderato)
		{
			return EMusicTempo::moderato;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Allegretto)
		{
			return EMusicTempo::allegretto;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Allegro)
		{
			return EMusicTempo::allegro;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Vivace)
		{
			return EMusicTempo::vivace;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Presto)
		{
			return EMusicTempo::presto;
		}
		else if (Bpm < c_MusicTempoBpm_Upper__Prestissimo)
		{
			return EMusicTempo::prestissimo;
		}
		else
		{
			L_ERR(MEOHelpers, "no definition for Bpm {0}", Bpm);
		}
	}

	L_ERR(MEOHelpers, "invalid Bpm {0}", Bpm);


	return static_cast<EMusicTempo>(0);
}


TArray<int> UMusicTypes::GetScaleNoteSemitonalDifferentials(const EMusicScale Scale)
{
	switch (Scale)
	{

	case EMusicScale::major:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				1  // half step
			}
		);

	case EMusicScale::natural_minor:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2,  // whole step
			}
		);

	case EMusicScale::major_pentatonic:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				3, // minor third
				2, // whole step
				3, // minor third
			}
		);

	case EMusicScale::minor_pentatonic:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				3, // minor third
				2, // whole step
				2, // whole step
				3, // minor third
				2, // whole step
			}
		);

	case EMusicScale::lydian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				1,  // half step
			}
		);

	case EMusicScale::blues:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				3, // minor third
				2, // whole step
				1, // half step
				1, // half step
				3, // minor third
				2, // whole step
			}
		);

	case EMusicScale::chromatic:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
				1, // half step
			}
		);

	case EMusicScale::neapolitan_major:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
				3, // minor third
				1, // half step
			}
		);

	case EMusicScale::neapolitan_minor:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
			}
		);

	case EMusicScale::diminished:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				1, // half step
				2, // whole step
				1, // half step
				2, // whole step
			}
		);

	case EMusicScale::harmonic_minor:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				1, // half step
				3, // minor third
				1, // half step
			}
		);

	case EMusicScale::melodic_minor_ascending:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
			}
		);

	case EMusicScale::melodic_minor_descending:
		return TArray<int>(
			{
				12, // descending scale starts one octave higher than the root note
				-2, // descending whole step
				-2, // descending whole step
				-1, // descending half step
				-2, // descending whole step
				-2, // descending whole step
				-1, // descending half step
				-2, // descending whole step
			}
		);

	case EMusicScale::double_harmonic_major:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				3, // minor third
				1, // half step
				2, // whole step
				1, // half step
				3, // minor third
				1, // half step
			}
		);

	case EMusicScale::double_harmonic_minor:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				3, // minor third
				1, // half step
				1, // half step
				3, // minor third
				1, // half step
			}
		);

	case EMusicScale::ionian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
			}
		);

	case EMusicScale::dorian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
			}
		);

	case EMusicScale::phrygian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
			}
		);

	case EMusicScale::mixolydian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
			}
		);

	case EMusicScale::aeolian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
			}
		);

	case EMusicScale::locrian:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				2, // whole step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
				2, // whole step
			}
		);

	case EMusicScale::augmented:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				3, // minor third
				1, // half step
				3, // minor third
				1, // half step
				3, // minor third
				1, // half step
			}
		);

	case EMusicScale::major_triad:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				4, // major third
				3, // minor third
			}
		);

	case EMusicScale::minor_triad:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				3, // minor third
				4, // major third
			}
		);

	case EMusicScale::diminished_triad:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				3, // minor third
				3, // minor third
			}
		);

	case EMusicScale::augmented_triad:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				4, // major third
				4, // major third
			}
		);

	case EMusicScale::diatonic:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
				2, // whole step
			}
		);

	case EMusicScale::ahava_raba:
		return TArray<int>(
			{
				0, // ascending scales start on the root note
				1, // half step
				3, // minor third
				1, // half step
				2, // whole step
				1, // half step
				2, // whole step
				2, // whole step
			}
		);

	default:
		L_ERR(MEOHelpers, "default on scale '{0}'", Scale);
		break;
	}


	return TArray<int>();
}


EMusicNoteToneType UMusicTypes::RandomToneType(bool bIncludeRest, FName const& Seed)
{
	return static_cast<EMusicNoteToneType>(UMathTypes::GetRandomInt(bIncludeRest == false, (UMCommon::GetEnumSize<EMusicNoteToneType>() - 1), Seed));
}


int UMusicTypes::RandomToneOctave(FName const& Seed)
{
	return UMathTypes::GetRandomInt(0, UMusicTypes::c_HighestAudibleOctave, Seed);
}


EMusicSongGenre UMusicTypes::RandomGenre(FName const& Seed)
{
	return UMCommon::GetRandomValueForEnum<EMusicSongGenre>(Seed);
}


FMusicTone UMusicTypes::RandomTone(bool bIncludeRest, FName const& Seed)
{
	return FMusicTone(UMusicTypes::RandomToneType(bIncludeRest, Seed), UMusicTypes::RandomToneOctave(Seed));
}


FMusicTone UMusicTypes::RandomToneFromScale(const EMusicScale Scale, FMusicTone const& RootTone, FName const& Seed)
{
	const TArray<FMusicTone> Tones = UMusicTypes::GetMusicTonesInScale(Scale, RootTone);
	if (Tones.Num() == 1)
	{
		return Tones[0];
	}
	else if (Tones.Num() <= 0)
	{
		return FMusicTone();
	}

	const int RandomIndex = UMathTypes::GetRandomIndex<FMusicTone>(Tones, Seed);


	return Tones[RandomIndex];
}


EMusicScale UMusicTypes::RandomScale(const EMusicSongGenre Genre, FName const& Seed)
{
	// todo
	// something more smart?


	return UMCommon::GetRandomValueForEnum<EMusicScale>(Seed);
}


EMusicTempo UMusicTypes::RandomTempoType(const EMusicSongGenre Genre, const EMusicSongChronologicalSection ChronologicalType, FName const& Seed)
{
	// todo
	// something more smart?


	return UMCommon::GetRandomValueForEnum<EMusicTempo>(Seed);
}


EMusicSongGenerationMethod UMusicTypes::RandomGenerationMethod(FName const& Seed)
{
	// select a generation method in a way that defines weights for each method
	// this function should be manipulated if we want a generation method to have
	// more or less weight
	// todo


	return UMCommon::GetRandomValueForEnum<EMusicSongGenerationMethod>(Seed);
}


FMusicOrchestrationSection UMusicTypes::GenerateOrchestrationSection(
	FMusicTimeSignature const& TimeSignature,
	FMusicTone const& RootTone,
	const EMusicSongGenre Genre,
	const EMusicSongChronologicalSection ChronologicalType
)
{
	FMusicOrchestrationSection Out;
	const EMusicScale Scale = UMusicTypes::RandomScale(Genre);

	// instrument
	Out.InstrumentCompositions.Add(UMusicTypes::GenerateInstrumentComposition(TimeSignature, RootTone, Genre, Scale, ChronologicalType, EMusicSongInstrumentalSection::beat, 4));
	Out.InstrumentCompositions.Add(UMusicTypes::GenerateInstrumentComposition(TimeSignature, RootTone, Genre, Scale, ChronologicalType, EMusicSongInstrumentalSection::bass, 4));
	Out.InstrumentCompositions.Add(UMusicTypes::GenerateInstrumentComposition(TimeSignature, RootTone, Genre, Scale, ChronologicalType, EMusicSongInstrumentalSection::treble, 4));
	Out.InstrumentCompositions.Add(UMusicTypes::GenerateInstrumentComposition(TimeSignature, RootTone, Genre, Scale, ChronologicalType, EMusicSongInstrumentalSection::melody, 4));

	// tempo
	Out.Tempo = UMusicTypes::GetBpmFromTempoType(UMusicTypes::RandomTempoType(Genre, ChronologicalType));


	return Out;
}


FMusicOrchestration UMusicTypes::GenerateOrchestration(
	FMusicTimeSignature const& TimeSignature,
	FMusicTone const& RootTone,
	const EMusicSongGenre Genre
)
{
	const EMusicSongGenerationMethod GenerationMethod = UMusicTypes::RandomGenerationMethod();
	switch (GenerationMethod)
	{

	case EMusicSongGenerationMethod::aababa:
		return UMusicTypes::GenerateOrchestrationMethod_AABABA(TimeSignature, RootTone, Genre);


	default:
		L_ERR(MEOHelpers, "default on generation method: '{0}'", GenerationMethod);
		break;
	}


	return FMusicOrchestration();
}


FMusicOrchestration UMusicTypes::GenerateOrchestrationMethod_AABABA(
	FMusicTimeSignature const& TimeSignature,
	FMusicTone const& RootTone,
	const EMusicSongGenre Genre
)
{
	FMusicOrchestration Out;

	// beginning
	Out.Sections.Add(UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::intro));

	// create a song using the form A-A-B-A
	FMusicOrchestrationSection VerseA = UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::verse);
	FMusicOrchestrationSection VerseB = UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::verse);
	FMusicOrchestrationSection Chorus = UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::chorus);

	// A-A
	Out.Sections.Add(VerseA);
	Out.Sections.Add(VerseA);

	Out.Sections.Add(Chorus);

	// B-A
	Out.Sections.Add(VerseB);
	Out.Sections.Add(VerseA);

	Out.Sections.Add(Chorus);

	// usually followed by another B-A
	Out.Sections.Add(VerseB);
	Out.Sections.Add(VerseA);

	Out.Sections.Add(Chorus);

	// bridge
	Out.Sections.Add(UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::bridge));

	// ending
	Out.Sections.Add(UMusicTypes::GenerateOrchestrationSection(TimeSignature, RootTone, Genre, EMusicSongChronologicalSection::outro));


	return Out;
}


FMusicInstrumentComposition UMusicTypes::GenerateInstrumentComposition(
	FMusicTimeSignature const& TimeSignature,
	const FMusicTone RootTone,
	const EMusicSongGenre Genre,
	const EMusicScale Scale,
	const EMusicSongChronologicalSection ChronologicalType,
	const EMusicSongInstrumentalSection InstrumentType,
	const int NumMeasures
)
{
	const FName InstrumentName = FName(FString(UMCommon::EnumToString(InstrumentType)));
	const int TotalPulses = NumMeasures * TimeSignature.GetPulsesPerMeasure();

	FMusicInstrumentComposition Out = FMusicInstrumentComposition(InstrumentName);

	switch (Genre)
	{

	case EMusicSongGenre::lofi:
		break;


	default:
		L_ERR(MEOHelpers, "default on music genre: '{0}'", Genre);
		break;
	}

	switch (InstrumentType)
	{

	case EMusicSongInstrumentalSection::treble:
	case EMusicSongInstrumentalSection::melody:
	case EMusicSongInstrumentalSection::bass:
		switch (ChronologicalType)
		{

		case EMusicSongChronologicalSection::bridge:
			break;

		case EMusicSongChronologicalSection::chorus:
			break;

		case EMusicSongChronologicalSection::verse:
			break;

		case EMusicSongChronologicalSection::intro:
			break;

		case EMusicSongChronologicalSection::outro:
			break;

		default:
			L_ERR(MEOHelpers, "default on chronological section: '{0}'", ChronologicalType);
			break;
		}
		break;

	case EMusicSongInstrumentalSection::beat:
		switch (ChronologicalType)
		{

		case EMusicSongChronologicalSection::intro:
		case EMusicSongChronologicalSection::bridge:
		case EMusicSongChronologicalSection::outro:
			Out.ChordNoteChannels.Add(FMusicChordNoteChannel().FillWithRests(TotalPulses));
			break;

		case EMusicSongChronologicalSection::chorus:
		case EMusicSongChronologicalSection::verse:
			break;

		default:
			L_ERR(MEOHelpers, "default on chronological section: '{0}'", ChronologicalType);
			break;
		}
		break;


	default:
		L_ERR(MEOHelpers, "default on instrumental section: '{0}'", InstrumentType);
		break;
	}


	NOT_IMPLEMENTED(MEOHelpers); // todo - DYNAMIC MUSIC GENERATION figure out a way to generate music compositions


	return FMusicInstrumentComposition();
}


FMusicSongToneData FMusicSong::GetNoteInChannelForBarBeat(const int ChannelIndex, const int BarNumber, const int BeatNumber) const
{
	if (IsChannelIndexValid(ChannelIndex) == false)
	{
		L_ERR(MEOHelpers, "invalid ChannelIndex '{0}'", ChannelIndex);
		return FMusicSongToneData();
	}

	const int BarIndex = BarNumber - 1;
	if (BarIndex < 0 && BarIndex >= GetNumberOfBarsInChannel(ChannelIndex))
	{
		L_ERR(MEOHelpers, "invalid BarNumber '{0}'", BarNumber);
		return FMusicSongToneData();
	}

	const int BeatIndex = BeatNumber - 1;
	if (BarIndex < 0 && BarIndex >= Info.TimeSignature.BeatsPerMeasure)
	{
		L_ERR(MEOHelpers, "invalid BeatNumber '{0}'", BeatNumber);
		return FMusicSongToneData();
	}

	const int TotalPulses = GetTotalPulses();
	const float PulsesPerBeat = GetPulsesPerMusicBeat();
	const int PulsesPerMeasure = Info.TimeSignature.BeatsPerMeasure * PulsesPerBeat;
	const int PulseIndex = (PulsesPerMeasure * BarIndex) + BeatIndex;

	if (PulseIndex < 0 || PulseIndex >= TotalPulses)
	{
		return FMusicSongToneData();
	}


	return ToneChannels[ChannelIndex].PulseData[PulseIndex];
}


TArray<FMusicSongToneData> FMusicSong::GetNotesInChannelForBar(const int ChannelIndex, const int BarNumber, bool bIncludeToneChangesOnly) const
{
	if (IsChannelIndexValid(ChannelIndex) == false)
	{
		L_ERR(MEOHelpers, "invalid ChannelIndex '{0}'", ChannelIndex);
		return TArray<FMusicSongToneData>();
	}

	const int BarIndex = BarNumber - 1;
	if (BarIndex < 0 && BarIndex >= GetNumberOfBarsInChannel(ChannelIndex))
	{
		L_ERR(MEOHelpers, "invalid BarNumber '{0}'", BarNumber);
		return TArray<FMusicSongToneData>();
	}

	const int TotalPulses = GetTotalPulses();
	const int TotalBeats = GetNumberOfBeatsFromBars(BarNumber, Info.TimeSignature.BeatsPerMeasure);
	const float PulsesPerBeat = GetPulsesPerMusicBeat();
	const int PulsesPerMeasure = Info.TimeSignature.BeatsPerMeasure * PulsesPerBeat;

	TArray<FMusicSongToneData> Out;
	const int PulseStartIndex = PulsesPerMeasure * BarIndex;
	const int PulseStopIndex = PulseStartIndex + PulsesPerMeasure;

	if (PulseStartIndex >= 0 && PulseStartIndex < TotalPulses)
	{
		for (int i = PulseStartIndex; i < PulseStopIndex; i++)
		{
			if (i < 0 || i >= TotalPulses)
			{
				break;
			}

			// only add the actual tone changes -- durations on other notes may be incorrect
			if (bIncludeToneChangesOnly == false ||
				ToneChannels[ChannelIndex].PulseData[i].HasToneChange()
				)
			{
				Out.Add(ToneChannels[ChannelIndex].PulseData[i]);
			}
		}
	}


	return Out;
}


int FMusicSong::GetNumberOfBarsFromBeats(const int NumberOfBeats, const int BeatsPerBar)
{
	if (BeatsPerBar <= 0)
	{
		L_ERR(MEOHelpers, "invalid BeatsPerBar '{0}'", BeatsPerBar);
		return -1;
	}

	const int RemainderBar = NumberOfBeats % BeatsPerBar > 0;


	return (NumberOfBeats / BeatsPerBar) + RemainderBar;
}


int FMusicSong::GetNumberOfBeatsFromBars(const int NumberOfBars, const int BeatsPerBar)
{
	return NumberOfBars * BeatsPerBar;
}


int FMusicSong::GetTotalNumberOfBars() const
{
	int LongestChannelBarSize = 0;
	FOR_FORWARD(i, ToneChannels.Num())
	{
		const int ChannelBarSize = GetNumberOfBarsInChannel(i);
		if (LongestChannelBarSize < ChannelBarSize)
		{
			LongestChannelBarSize = ChannelBarSize;
		}
	}


	return LongestChannelBarSize;
}


int FMusicSong::GetNumberOfPulsesInChannel(const int ChannelIndex) const
{
	if (IsChannelIndexValid(ChannelIndex) == false)
	{
		L_ERR(MEOHelpers, "invalid ChannelIndex '{0}'", ChannelIndex);
		return -1;
	}


	return ToneChannels[ChannelIndex].PulseData.Num();
}


int FMusicSong::GetNumberOfBarsInChannel(const int ChannelIndex) const
{
	if (IsChannelIndexValid(ChannelIndex) == false)
	{
		L_ERR(MEOHelpers, "invalid ChannelIndex '{0}'", ChannelIndex);
		return -1;
	}

	const int PulsesInChannel = GetNumberOfPulsesInChannel(ChannelIndex);
	const int PulsesPerBar = static_cast<int>(FMath::RoundHalfToEven(static_cast<float>(Info.TimeSignature.BeatsPerMeasure) * GetPulsesPerMusicBeat()));
	if (PulsesPerBar <= 0)
	{
		L_ERR(MEOHelpers, "invalid PulsesPerBar '{0}'", PulsesPerBar);
		return -1;
	}

	const int BarsInChannel = (PulsesInChannel / PulsesPerBar);
	const int BeatsInChannel = BarsInChannel * Info.TimeSignature.BeatsPerMeasure;


	return GetNumberOfBarsFromBeats(BeatsInChannel, Info.TimeSignature.BeatsPerMeasure);
}


bool UMusicTypes::IsValidTimeSignatureBeatsPerWholeNote(const int BeatsPerWholeNote)
{
	return FMusicTimeSignature::c_BeatsPerWholeNoteTimeSignatureQuantizationMap.Contains(BeatsPerWholeNote);
}


EQuartzTimeSignatureQuantization UMusicTypes::GetQuartzTimeSignatureQuantization(const int BeatsPerWholeNote)
{
	if (IsValidTimeSignatureBeatsPerWholeNote(BeatsPerWholeNote) == false)
	{
		L_ERR(MEOHelpers, "invalid BeatsPerWholeNote: {0}", BeatsPerWholeNote);
		return EQuartzTimeSignatureQuantization::QuarterNote;
	}


	return FMusicTimeSignature::c_BeatsPerWholeNoteTimeSignatureQuantizationMap[BeatsPerWholeNote];
}


EQuartzTimeSignatureQuantization FMusicTimeSignature::GetQuartzTimeSignatureQuantization() const
{
	return UMusicTypes::GetQuartzTimeSignatureQuantization(BeatsPerWholeNote);
}


bool FMusicTimeSignature::MakesValidQuartzClock() const
{
	return IsValid() && UMusicTypes::IsValidTimeSignatureBeatsPerWholeNote(BeatsPerWholeNote);
}


FQuartzClockSettings FMusicTimeSignature::MakeQuartzClockSettings(const int QuartzBeatsPerMusicBeat) const
{
	FQuartzClockSettings Out;

	Out.TimeSignature.NumBeats = BeatsPerMeasure * QuartzBeatsPerMusicBeat;
	Out.TimeSignature.BeatType = GetQuartzTimeSignatureQuantization();


	return Out;
}


FQuartzClockSettings FMusicSong::MakeQuartzClockSettings() const
{
	return Info.TimeSignature.MakeQuartzClockSettings(GetQuartzBeatsPerMusicBeat());
}


EMusicNoteDurationType FMusicSongToneChannel::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, PulseData.Num())
	{
		const EMusicNoteDurationType NoteDurationType = PulseData[i].Duration;
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


EMusicNoteDurationType FMusicSong::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, ToneChannels.Num())
	{
		const EMusicNoteDurationType NoteDurationType = ToneChannels[i].FindSmallestNoteDurationType();
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


EMusicNoteDurationType FMusicChordNoteChannel::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, NoteStream.Num())
	{
		const EMusicNoteDurationType NoteDurationType = NoteStream[i].Duration;
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


EMusicNoteDurationType FMusicInstrumentComposition::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, ChordNoteChannels.Num())
	{
		const EMusicNoteDurationType NoteDurationType = ChordNoteChannels[i].FindSmallestNoteDurationType();
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


EMusicNoteDurationType FMusicOrchestrationSection::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, InstrumentCompositions.Num())
	{
		const EMusicNoteDurationType NoteDurationType = InstrumentCompositions[i].FindSmallestNoteDurationType();
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


EMusicNoteDurationType FMusicOrchestration::FindSmallestNoteDurationType() const
{
	EMusicNoteDurationType SmallestDurationType = EMusicNoteDurationType::whole;
	FOR_FORWARD(i, Sections.Num())
	{
		const EMusicNoteDurationType NoteDurationType = Sections[i].FindSmallestNoteDurationType();
		if (static_cast<int>(NoteDurationType) > static_cast<int>(SmallestDurationType))
		{
			SmallestDurationType = NoteDurationType;
		}
	}


	return SmallestDurationType;
}


int FMusicSong::GetPulsesPerWholeNote() const
{
	const EMusicNoteDurationType PulseDurationType = GetPulseDurationType();
	const int PulseDurationTypePulseCount = UMusicTypes::GetMusicNoteDurationTypePulseCount(EMusicNoteDurationType::whole, PulseDurationType);
	if (PulseDurationTypePulseCount <= 0)
	{
		L_WARN(MEOHelpers, "PulseDurationTypePulseCount '{0} : {1}' is invalid", PulseDurationTypePulseCount, PulseDurationType);
	}


	return PulseDurationTypePulseCount;
}


int FMusicSong::GetQuartzBeatsPerWholeNote() const
{
	return FMath::Max(Info.TimeSignature.BeatsPerWholeNote, GetPulsesPerWholeNote());
}


int FMusicSong::GetQuartzBeatsPerMusicBeat() const
{
	const int PulsesPerWholeNote = GetPulsesPerWholeNote();
	if (PulsesPerWholeNote <= 0)
	{
		L_ERR(MEOHelpers, "PulsesPerWholeNote '{0}' is invalid", PulsesPerWholeNote);
		return 0;
	}

	const float PulsesPerMusicBeat = GetPulsesPerMusicBeat();
	if (PulsesPerMusicBeat <= 0)
	{
		L_WARN(MEOHelpers, "PulsesPerMusicBeat '{0}' is invalid", PulsesPerMusicBeat);
	}

	const int QuartzBeatsPerWholeNote = GetQuartzBeatsPerWholeNote();
	if (QuartzBeatsPerWholeNote <= 0)
	{
		L_WARN(MEOHelpers, "QuartzBeatsPerWholeNote '{0}' is invalid", QuartzBeatsPerWholeNote);
	}

	const int PulseQuartzBeatsPerMusicBeatWholeNotes = static_cast<int>(FMath::RoundHalfToEven(PulsesPerMusicBeat * QuartzBeatsPerWholeNote));
	if (PulseQuartzBeatsPerMusicBeatWholeNotes < PulsesPerWholeNote)
	{
		L_WARN(MEOHelpers, "PulseQuartzBeatsPerMusicBeatWholeNotes '{0}' less than PulsesPerWholeNote '{1}' -- results in zero", PulseQuartzBeatsPerMusicBeatWholeNotes, PulsesPerWholeNote);
	}


	return (PulseQuartzBeatsPerMusicBeatWholeNotes) / PulsesPerWholeNote;
}


float FMusicSong::GetPulsesPerMusicBeat() const
{
	if (Info.TimeSignature.BeatsPerWholeNote <= 0)
	{
		L_ERR(MEOHelpers, "BeatsPerWholeNote '{0}' is invalid", Info.TimeSignature.BeatsPerWholeNote);
		return 0;
	}

	const int PulsesPerWholeNote = GetPulsesPerWholeNote();
	if (PulsesPerWholeNote <= 0)
	{
		L_WARN(MEOHelpers, "PulsesPerWholeNote '{0}' is invalid", PulsesPerWholeNote);
	}


	return static_cast<float>(PulsesPerWholeNote) / static_cast<float>(Info.TimeSignature.BeatsPerWholeNote);
}


int FMusicSong::GetQuartzBeatsPerPulse() const
{
	const float PulsesPerMusicBeat = GetPulsesPerMusicBeat();
	if (PulsesPerMusicBeat <= 0)
	{
		L_ERR(MEOHelpers, "PulsesPerMusicBeat '{0}' is invalid", PulsesPerMusicBeat);
		return 0;
	}

	const int QuartzBeatsPerMusicBeat = GetQuartzBeatsPerMusicBeat();
	if (QuartzBeatsPerMusicBeat <= 0)
	{
		L_WARN(MEOHelpers, "QuartzBeatsPerMusicBeat '{0}' is invalid", QuartzBeatsPerMusicBeat);
	}


	return static_cast<int>(FMath::RoundHalfToEven(static_cast<float>(QuartzBeatsPerMusicBeat) / PulsesPerMusicBeat));
}


int UMusicTypes::GetMusicNoteDurationTypePulseCount(const EMusicNoteDurationType DurationType, const EMusicNoteDurationType PulseDurationType)
{
	const int PulseWeight = GetMusicNoteDurationTypeBeatWeight(PulseDurationType);
	if (PulseWeight <= 0)
	{
		L_ERR(MEOHelpers, "pulse weight '{0}' is invalid", PulseWeight);
		return 0;
	}

	const int DurationWeight = GetMusicNoteDurationTypeBeatWeight(DurationType);
	if (DurationWeight <= 0)
	{
		L_WARN(MEOHelpers, "duration weight '{0}' is invalid", DurationWeight);
	}
	if (DurationWeight < PulseWeight)
	{
		L_WARN(MEOHelpers, "duration weight '{0}' less than pulse weight '{1}' -- results in zero", DurationWeight, PulseWeight);
	}


	return DurationWeight / PulseWeight;
}


int UMusicTypes::GetMusicNoteDurationTypeBeatWeight(const EMusicNoteDurationType DurationType)
{
	const int InvertedBeatWeight = UMusicTypes::GetMusicNoteDurationTypeInvertedBeatWeight(DurationType);
	if (InvertedBeatWeight == 0)
	{
		L_ERR(MEOHelpers, "InvertedBeatWeight is zero");
		return 0;
	}


	return UMusicTypes::c_LargestSupportedPulsesPerBeat / InvertedBeatWeight;
}


FMusicBeatIterator& FMusicBeatIterator::IterateQuartzBeats(const int QuartzBeats)
{
	QuartzBeatIterator = QuartzBeatIterator + QuartzBeats;


	return *this;
}


int FMusicBeatIterator::GetCurrentPulseBeat() const
{
	if (QuartzBeatsPerPulseBeat <= 0)
	{
		L_ERR(AudioRhythm, "QuartzBeatsPerPulseBeat '{0}' is invalid", QuartzBeatsPerPulseBeat);
		return 0;
	}


	return QuartzBeatIterator / QuartzBeatsPerPulseBeat;
}


bool FMusicBeatIterator::IsPulseBeat() const
{
	const int PulseBeat = GetCurrentPulseBeat();
	return	QuartzBeatIterator >= QuartzBeatsPerPulseBeat ||
		PulseBeat <= 0 ||
		PulseBeat > TotalPulses;
}


bool FMusicBeatIterator::IsMusicBeat() const
{
	if (QuartzBeatsPerMusicBeat <= 0)
	{
		L_ERR(AudioRhythm, "QuartzBeatsPerMusicBeat '{0}' is invalid", QuartzBeatsPerMusicBeat);
		return 0;
	}


	return GetCurrentPulseBeat() % QuartzBeatsPerMusicBeat == 0;
}


int FMusicBeatIterator::GetCurrentQueuedMusicBeat() const
{
	return GetTotalPlayedMusicBeats();
}


int FMusicBeatIterator::GetCurrentAudibleMusicBeat() const
{
	return ((*this) - 1).GetCurrentQueuedMusicBeat();
}


int FMusicBeatIterator::GetTotalPlayedMusicBeats() const
{
	if (QuartzBeatsPerMusicBeat <= 0)
	{
		L_ERR(AudioRhythm, "QuartzBeatsPerMusicBeat '{0}' is invalid", QuartzBeatsPerMusicBeat);
		return 0;
	}


	return GetTotalPlayedQuartzBeats() / QuartzBeatsPerMusicBeat;
}


bool FMusicBeatIterator::HasPassedLastMusicBeat() const
{
	return GetTotalPlayedMusicBeats() > GetTotalQuartzBeats();
}


FMusicBeatIterator FMusicSong::MakeBeatIterator() const
{
	return FMusicBeatIterator(
		GetQuartzBeatsPerPulse(),
		GetQuartzBeatsPerMusicBeat(),
		Info.TimeSignature.BeatsPerMeasure,
		GetPulsesPerMusicBeat(),
		GetTotalPulses()
	);
}


FMusicOrchestration::FMusicOrchestration(FMusicOrchestrationGenerator const& GeneratorProperties)
{
	NOT_IMPLEMENTED(MEOHelpers); // todo - generate orchestration from generator properties
}