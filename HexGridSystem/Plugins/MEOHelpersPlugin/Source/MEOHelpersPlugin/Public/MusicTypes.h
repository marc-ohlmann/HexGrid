// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sound/QuartzQuantizationUtilities.h"
#include "MusicTypes.generated.h"


// musical constants

#define NUMBER_OF_MUSIC_NOTES (12)
#define MUSIC_NOTE_FREQUENCY_HERTZ_C0 (16.35f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_Db0 (17.32f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_D0 (18.35f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_Eb0 (19.45f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_E0 (20.60f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_F0 (21.83f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_Gb0 (23.12f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_G0 (24.50f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_Ab0 (25.96f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_A0 (27.50f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_Bb0 (29.14f)
#define MUSIC_NOTE_FREQUENCY_HERTZ_B0 (30.87f)
#define SEMITONE_FREQUENCY_RATIO (1.059463f)
#define INVERSE_SEMITONE_FREQUENCY_RATIO (0.943874f)


#define TEMPO_BPM__CENTER__GRAVE (30.f)
#define TEMPO_BPM__UPPER___GRAVE (40.f)

#define TEMPO_BPM__CENTER__LENTO (42.5f)
#define TEMPO_BPM__UPPER___LENTO (45.f)

#define TEMPO_BPM__CENTER__LARGO (47.5f)
#define TEMPO_BPM__UPPER___LARGO (50.f)

#define TEMPO_BPM__CENTER__LARGHETTO (52.5f)
#define TEMPO_BPM__UPPER___LARGHETTO (55.f)

#define TEMPO_BPM__CENTER__ADAGIO (60.f)
#define TEMPO_BPM__UPPER___ADAGIO (65.f)

#define TEMPO_BPM__CENTER__ADAGIETTO (67.f)
#define TEMPO_BPM__UPPER___ADAGIETTO (69.f)

#define TEMPO_BPM__CENTER__ANDANTE (75.f)
#define TEMPO_BPM__UPPER___ANDANTE (77.f)

#define TEMPO_BPM__CENTER__MODERATO (91.5f)
#define TEMPO_BPM__UPPER___MODERATO (97.f)

#define TEMPO_BPM__CENTER__ALLEGRETTO (103.5f)
#define TEMPO_BPM__UPPER___ALLEGRETTO (109.f)

#define TEMPO_BPM__CENTER__ALLEGRO (120.5f)
#define TEMPO_BPM__UPPER___ALLEGRO (132.f)

#define TEMPO_BPM__CENTER__VIVACE (136.f)
#define TEMPO_BPM__UPPER___VIVACE (140.f)

#define TEMPO_BPM__CENTER__PRESTO (172.5f)
#define TEMPO_BPM__UPPER___PRESTO (177.f)

#define TEMPO_BPM__CENTER__PRESTISSIMO (200.f)
#define TEMPO_BPM__UPPER___PRESTISSIMO (TNumericLimits<float>::Max())


// musical note symbols in there various forms
UENUM(BlueprintType)
enum class EMusicNoteToneType : uint8
{
	rest	UMETA(DisplayName = "Rest"),

	Cb		UMETA(DisplayName = "Cb"),
	C		UMETA(DisplayName = "C"),
	Cs		UMETA(DisplayName = "C#"),

	Db		UMETA(DisplayName = "Db"),
	D		UMETA(DisplayName = "D"),
	Ds		UMETA(DisplayName = "D#"),

	Eb		UMETA(DisplayName = "Eb"),
	E		UMETA(DisplayName = "E"),
	Es		UMETA(DisplayName = "E#"),

	Fb		UMETA(DisplayName = "Fb"),
	F		UMETA(DisplayName = "F"),
	Fs		UMETA(DisplayName = "F#"),

	Gb		UMETA(DisplayName = "Gb"),
	G		UMETA(DisplayName = "G"),
	Gs		UMETA(DisplayName = "G#"),

	Ab		UMETA(DisplayName = "Ab"),
	A		UMETA(DisplayName = "A"),
	As		UMETA(DisplayName = "A#"),

	Bb		UMETA(DisplayName = "Bb"),
	B		UMETA(DisplayName = "B"),
	Bs		UMETA(DisplayName = "B#"),
};


// different tempos that change the speed of music
// note that these values are placed in order, 
// such that there index is the power of 2 that equals their weight
UENUM(BlueprintType)
enum class EMusicNoteDurationType : uint8
{
	whole			UMETA(DisplayName = "Whole"),
	half			UMETA(DisplayName = "Half"),
	quarter			UMETA(DisplayName = "Quarter"),
	eighth			UMETA(DisplayName = "Eighth"),
	sixteenth		UMETA(DisplayName = "Sixteenth"),
	thirtysecond	UMETA(DisplayName = "Thirty-Second")
};


// different tempos that determine the speed of music
UENUM(BlueprintType)
enum class EMusicTempo : uint8
{
	// slow and solemn (20-40 bpm)
	grave			UMETA(DisplayName = "Grave"),

	// slowly (40-45 bpm)
	lento			UMETA(DisplayName = "Lento"),

	// broadly (45-50 bpm)
	largo			UMETA(DisplayName = "Largo"),

	// slow (50-55 bpm)
	larghetto		UMETA(DisplayName = "Larghetto"),

	// slow and stately, literally "at ease" (55-65 bpm)
	adagio			UMETA(DisplayName = "Adagio"),

	// rather slow (65-69 bpm)
	adagietto		UMETA(DisplayName = "Adagietto"),

	// moderate, walking speed (73-77 bpm)
	andante			UMETA(DisplayName = "Andante"),

	// moderately (86-97 bpm)
	moderato		UMETA(DisplayName = "Moderato"),

	// moderately fast (98-109 bpm)
	allegretto		UMETA(DisplayName = "Allegretto"),

	// quick and lively, bright (109-132 bpm)
	allegro			UMETA(DisplayName = "Allegro"),

	// lively and fast (132-140 bpm)
	vivace			UMETA(DisplayName = "Vivace"),

	// very fast (168-177 bpm)
	presto			UMETA(DisplayName = "Presto"),

	// extremely fast (178+ bpm)
	prestissimo		UMETA(DisplayName = "Prestissimo"),
};


// tempo modifiers that change the speed of music dynamically
UENUM(BlueprintType)
enum class EMusicTempoModifier : uint8
{
	// progressively faster
	accelerando		UMETA(DisplayName = "Accelerando"),

	// progressively slower
	rallentando		UMETA(DisplayName = "Rallentando"),

	// pause briefly at a single note
	fermata			UMETA(DisplayName = "Fermata"),

	// slow down or decelerate at a single note
	ritardando		UMETA(DisplayName = "Ritardando"),

	// speed up or accelerate at a single note
	ritenuto		UMETA(DisplayName = "Ritenuto"),
};


// musical scales
UENUM(BlueprintType)
enum class EMusicScale : uint8
{
	// classical, happy
	major						UMETA(DisplayName = "Major"),

	// scary, epic
	natural_minor				UMETA(DisplayName = "Natural Minor"),

	// haunting, creepy
	harmonic_minor				UMETA(DisplayName = "Harmonic Minor"),

	// wistful, mysterious
	melodic_minor_ascending		UMETA(DisplayName = "Ascending Melodic Minor"),

	// sombre, soulful
	melodic_minor_descending	UMETA(DisplayName = "Descending Melodic Minor"),

	//
	neapolitan_major			UMETA(DisplayName = "Neapolitan Major"),

	//
	neapolitan_minor			UMETA(DisplayName = "Neapolitan Minor"),

	//
	double_harmonic_minor		UMETA(DisplayName = "Double Harmonic Minor"),

	//
	double_harmonic_major		UMETA(DisplayName = "Double Harmonic Major"),

	// country, gleeful
	major_pentatonic			UMETA(DisplayName = "Major Pentatonic"),

	// blues, rock
	minor_pentatonic			UMETA(DisplayName = "Minor Pentatonic"),

	//
	major_triad					UMETA(DisplayName = "Major Triad"),

	//
	minor_triad					UMETA(DisplayName = "Minor Triad"),

	//
	augmented_triad				UMETA(DisplayName = "Augmented Triad"),

	//
	diminished_triad			UMETA(DisplayName = "Diminished Triad"),

	//
	augmented					UMETA(DisplayName = "Augmented"),

	//
	diminished					UMETA(DisplayName = "Diminished"),

	//
	blues						UMETA(DisplayName = "Blues"),

	// random, atonal: all twelve notes
	chromatic					UMETA(DisplayName = "Chromatic"),

	//
	ionian						UMETA(DisplayName = "Ionian"),

	// jazzy, cool
	dorian						UMETA(DisplayName = "Dorian"),

	//
	phrygian					UMETA(DisplayName = "Phrygian"),

	//
	lydian						UMETA(DisplayName = "Lydian"),

	// progressive, complex
	mixolydian					UMETA(DisplayName = "Mixolydian"),

	//
	aeolian						UMETA(DisplayName = "Aeolian"),

	//
	locrian						UMETA(DisplayName = "Locrian"),

	// bizarre, symmetrical
	diatonic					UMETA(DisplayName = "Diatonic"),

	// exotic, unfamiliar
	ahava_raba					UMETA(DisplayName = "Ahava Raba"),
};


// a music tone at a certain octave
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicTone
{
	GENERATED_BODY()

public:

	FMusicTone()
		: Tone(EMusicNoteToneType::rest)
		, Octave(-1)
	{
	}

	FMusicTone(const EMusicNoteToneType InTone, const int InOctave) 
		: Tone(InTone)
		, Octave(InOctave)
	{
	}

	// the symbol used to describe the tone
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EMusicNoteToneType Tone;

	// the octave that the tone-type resides in
	// the power of two that scales the tone type's base frequency
	// has no musical relevance for rest notes
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Octave;

	// get a unique id for the tone, using its normalized tone-type and octave
	int GetToneId() const;

	// check if the tone is strictly higher pitch than another tone
	bool IsToneHigherPitchThan(FMusicTone const& Other) const { return GetToneId() > Other.GetToneId(); }

	bool operator>(FMusicTone const& Other) const
	{
		return IsToneHigherPitchThan(Other);
	}

	bool operator>=(FMusicTone const& Other) const
	{
		return IsToneHigherPitchThan(Other) || IsToneEqual(Other);
	}

	// check if the tone is strictly lower pitch than another tone
	bool IsToneLowerPitchThan(FMusicTone const& Other) const { return GetToneId() < Other.GetToneId(); }

	bool operator<(FMusicTone const& Other) const
	{
		return IsToneLowerPitchThan(Other);
	}

	bool operator<=(FMusicTone const& Other) const
	{
		return IsToneLowerPitchThan(Other) || IsToneEqual(Other);
	}

	// check if this tone is equivalent to another
	bool IsToneEqual(FMusicTone const& Other) const;

	// get the semitonal distance of this tone from another
	int GetSemitoneDistanceFrom(FMusicTone const& Other) const;

	// get the frequency, in hertz, of this tone
	float GetFrequencyHertz() const;

	// increment by one semitone
	FMusicTone& MakeSharp();

	// operator to pre-increment the tone by a semitone
	FMusicTone& operator++()
	{
		return MakeSharp();
	}

	// operator to post-increment the tone by a semitone
	FMusicTone operator++(int)
	{
		return MakeSharp();
	}

	// decrement by one semitone
	FMusicTone& MakeFlat();

	// operator to post-decrement the tone by a semitone
	FMusicTone operator--(int)
	{
		return MakeFlat();
	}

	// operator to pre-decrement the tone by a semitone
	FMusicTone& operator--()
	{
		return MakeFlat();
	}

	// removes any sharp or flats from this tone
	// adjusts by one or zero semitones, negative or positive, depending on the current tone
	// the behavior of this function changes based on the form of the tone provided
	// the outcome will be different for two equivalent but non-equal notes 
	// ex: A# and Ab become A, B# and Bb become B, C# and Cb become C, D# and Db become D, etc.
	FMusicTone& MakeNatural();

	// changes the current tone to a normal equivalent tone value
	// ex: Fb becomes E, B# becomes C, etc.
	// useful for comparing the equivalency of two notes
	FMusicTone& NormalizeTone();

	// increment the tone by a given semitonal distance
	// negative distances will decrement the tone
	FMusicTone& IncrementBySemitonalDistance(const int Semitones);

	// operator to increment the tone by a given semitonal distance
	// negative distances will decrement the tone
	FMusicTone operator+(const int SemitonalDistance) const
	{
		FMusicTone Out = *this;
		return Out.IncrementBySemitonalDistance(SemitonalDistance);
	}

	// operator to decrement the tone by a given semitonal distance
	// negative distances will increment the tone
	FMusicTone operator-(const int SemitonalDistance) const
	{
		FMusicTone Out = *this;
		return Out + (-1 * SemitonalDistance);
	}

	// operator to increment the tone by a given semitonal distance
	// negative distances will decrement the tone
	FMusicTone& operator+=(const int SemitonalDistance)
	{
		return IncrementBySemitonalDistance(SemitonalDistance);
	}

	// operator to decrement the tone by a given semitonal distance
	// negative distances will increment the tone
	FMusicTone& operator-=(const int SemitonalDistance)
	{
		return IncrementBySemitonalDistance(-1 * SemitonalDistance);
	}

	// increment the tone by a given octave distance (12 semitones)
	// negative distances will decrement the tone
	FMusicTone& IncrementByOctaveDistance(const int Octaves);

	// increment by one octave
	FMusicTone& IncrementOctave()
	{
		return IncrementByOctaveDistance(1);
	}

	// decrement by one octave
	FMusicTone& DecrementOctave()
	{
		return IncrementByOctaveDistance(-1);
	}

	// converts the tone to a printable string
	FString ToneToString() const;

	// print the tone to the log as a string
	void PrintTone() const;

	// get the notes in the given scale using this tone as the root
	TArray<FMusicTone> GetRootedScale(const EMusicScale Scale) const;

	// check if the tone is a silent rest
	bool IsRest() const { return Tone == EMusicNoteToneType::rest; }

	// make the tone a silent rest
	FMusicTone& MakeRest() { Tone = EMusicNoteToneType::rest; return *this; }

	// change the tone type in a positive direction
	// includes the rest tone, and has no musical relevance
	// useful for ui that allows players to change a tone
	FMusicTone& IterateToneForward();

	// change the tone type in a negative direction
	// includes the rest tone, and has no musical relevance
	// useful for ui that allows players to change a tone
	FMusicTone& IterateToneBackward();

	bool IsValidTone() const
	{
		return Octave >= 0;
	}
};


// a music tone for a given interval and volume
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicNote : public FMusicTone
{
	GENERATED_BODY()

public:

	FMusicNote()
		: FMusicTone()
		, Duration(EMusicNoteDurationType::whole)
		, Volume(1.f)
	{
	}

	FMusicNote(const EMusicNoteToneType InTone, const int InOctave, const EMusicNoteDurationType InDuration, const float InVolume = 1.f)
		: FMusicTone(InTone, InOctave)
		, Duration(InDuration)
		, Volume(InVolume)
	{
	}

	bool IsEqual(FMusicNote const& Other) const;

	bool operator==(FMusicNote const& Other) const
	{
		return IsEqual(Other);
	}

	bool operator!=(FMusicNote const& Other) const
	{
		return !IsEqual(Other);
	}

	// make a rest note for a given interval
	static FMusicNote RestNote(const EMusicNoteDurationType InDuration = EMusicNoteDurationType::quarter, const int InOctave = 0)
	{
		return FMusicNote(EMusicNoteToneType::rest, InOctave, InDuration);
	}

	// make a rest note for a single pulse
	static FMusicNote RestPulse(const int InOctave = 0);

	bool IsValidNote() const { return IsValidTone(); }

	// number of beats to play the note for
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EMusicNoteDurationType Duration;

	// change the duration type in a positive direction
	// useful for ui that allows players to change a note duration
	FMusicNote& IterateDurationForward();

	// change the duration type in a negative direction
	// useful for ui that allows players to change a note duration
	FMusicNote& IterateDurationBackward();

	// amplitude scalar of the tone's sinusoidal waveform
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Volume;

	// converts the note to a printable string
	FString ToString() const;

	// print the note to the log as a string
	void Print() const;

};


// time signature that determines the number of beats-per-measure
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicTimeSignature
{
	GENERATED_BODY()

public:

	FMusicTimeSignature()
		: BeatsPerMeasure(0)
		, BeatsPerWholeNote(0)
	{
	}

	// create a time signature of the form 'BeatsPerMeasure / BeatsPerWholeNote'
	FMusicTimeSignature(const int InBeatsPerMeasure, const int InBeatsPerWholeNote = 4)
		: BeatsPerMeasure(InBeatsPerMeasure)
		, BeatsPerWholeNote(InBeatsPerWholeNote)
	{
	}

	bool IsValid() const { return BeatsPerMeasure > 0 && BeatsPerWholeNote > 0; }

	// number of beats per measure
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int BeatsPerMeasure;

	int GetPulsesPerMeasure() const;

	// the number of beats that a whole note represents
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int BeatsPerWholeNote;

	int GetPulsesPerWholeNote() const;
	int GetMeasuresPerWholeNote() const;
	int GetWholeNotesPerMeasure() const;

	bool MakesValidQuartzClock() const;

	EQuartzTimeSignatureQuantization GetQuartzTimeSignatureQuantization() const;

	FQuartzClockSettings MakeQuartzClockSettings(const int QuartzBeatsPerMusicBeat) const;

	static const TMap<int, EQuartzTimeSignatureQuantization> c_BeatsPerWholeNoteTimeSignatureQuantizationMap;

};


// data for a pulse within a tone channel for a musical song
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicSongToneData : public FMusicNote
{
	GENERATED_BODY()

public:

	FMusicSongToneData()
		: FMusicNote()
		, bHasToneChange(false)
	{
	}

	FMusicSongToneData(
		bool InHasToneChange,
		const EMusicNoteToneType InToneType,
		const int InToneOctave,
		const EMusicNoteDurationType InDurationType,
		const float InToneVolume
	)
		: FMusicNote(InToneType, InToneOctave, InDurationType, InToneVolume)
		, bHasToneChange(InHasToneChange)
	{
	}

	// create pulse data that indicates no change in tone
	static FMusicSongToneData NoToneChange(const EMusicNoteDurationType DurationType)
	{
		return FMusicSongToneData(
			false,
			EMusicNoteToneType::rest,
			-1,
			DurationType,
			1.f
		);
	}

	// create pulse data that indicates a change to a given tone
	static FMusicSongToneData ToneChange(
		const EMusicNoteToneType InToneType,
		const int InToneOctave,
		const EMusicNoteDurationType DurationType,
		const float InToneVolume
	)
	{
		return FMusicSongToneData(
			true,
			InToneType,
			InToneOctave,
			DurationType,
			InToneVolume
		);
	}

	// create pulse data that indicates a change to a resting tone
	static FMusicSongToneData RestingToneChange(const EMusicNoteDurationType DurationType)
	{
		return FMusicSongToneData(
			true,
			EMusicNoteToneType::rest,
			0,
			DurationType,
			1.f
		);
	}

	// if true, indicates that the tone frequency should be changed
	UPROPERTY(EditAnywhere)
	bool bHasToneChange;

	// check if the tone frequency should be changed
	bool HasToneChange() const { return bHasToneChange; }

};


// a series of notes to play in order
// represents a single variable note that produces a variable tone within a composition
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicChordNoteChannel
{
	GENERATED_BODY()

public:

	FMusicChordNoteChannel()
		: NoteStream()
	{
	}

	FMusicChordNoteChannel(TArray<FMusicNote> const& InNoteStream)
		: FMusicChordNoteChannel()
	{
		NoteStream.Append(InNoteStream);
	}

	FMusicChordNoteChannel(FMusicNote const& InNote)
		: FMusicChordNoteChannel()
	{
		NoteStream.Add(InNote);
	}

	// the set of notes to play in order for their given intervals
	UPROPERTY(EditAnywhere)
	TArray<FMusicNote> NoteStream;

	// get a note to play at the given pulse number
	// returns true if there is a new note to play at the given beat
	bool GetNoteAtPulse(const EMusicNoteDurationType PulseDurationType, const int PulseNumber, FMusicNote& OutNote) const;

	// get total number of pulses required to play the entire stream of notes in this channel
	int GetTotalPulses(const EMusicNoteDurationType PulseDurationType) const;

	// fill the channel with rest notes so that its pulse length matches the given desired length
	FMusicChordNoteChannel& FillWithRests(const int DesiredPulseLength);

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

};


// a channel of pulse data for a single variable tone within a musical song
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicSongToneChannel
{
	GENERATED_BODY()

public:

	FMusicSongToneChannel()
		: PulseData()
	{
	}

	FMusicSongToneChannel(FMusicChordNoteChannel const& InMusicNoteChannel, const EMusicNoteDurationType PulseDurationType)
		: FMusicSongToneChannel()
	{
		const int TotalPulses = InMusicNoteChannel.GetTotalPulses(PulseDurationType);
		for(int i = 0; i < TotalPulses; i++)
		{
			FMusicNote NewNote;
			if (InMusicNoteChannel.GetNoteAtPulse(PulseDurationType, i, NewNote))
			{
				if (NewNote.IsRest())
				{
					PulseData.Add(FMusicSongToneData::RestingToneChange(NewNote.Duration));
				}
				else
				{
					PulseData.Add(
						FMusicSongToneData::ToneChange(
							NewNote.Tone,
							NewNote.Octave,
							NewNote.Duration,
							NewNote.Volume
						)
					);
				}
			}
			else
			{
				PulseData.Add(FMusicSongToneData::NoToneChange(PulseDurationType));
			}
		}
	}

	// cached data for each pulse to manipulate a single variable tone at runtime
	UPROPERTY(EditAnywhere)
	TArray<FMusicSongToneData> PulseData;

	// safely get pulse data at a given pulse index
	// will return a silent resting tone change if the index is invalid
	FMusicSongToneData GetPulseData(const int PulseIndex) const;

	// fill pulse data with rests so that it matches the given size
	FMusicSongToneChannel& FillWithRests(const int TotalPulses, const EMusicNoteDurationType PulseDurationType);

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

};


// a set of chord note channels that form a musical composition
// channels are needed to play multiple notes simultaneously, to make chords
// a composition is made up of multiple variable notes
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicInstrumentComposition
{
	GENERATED_BODY()

public:

	FMusicInstrumentComposition()
		: InstrumentName()
		, ChordNoteChannels()
	{
	}

	FMusicInstrumentComposition(FName const& InInstrumentName)
		: InstrumentName(InInstrumentName)
		, ChordNoteChannels()
	{
	}

	FMusicInstrumentComposition(
		FName const& InInstrumentName,
		TArray<FMusicChordNoteChannel> const& InChordNoteChannels
	)
		: FMusicInstrumentComposition(InInstrumentName)
	{
		ChordNoteChannels.Append(InChordNoteChannels);
	}

	FMusicInstrumentComposition(
		FName const& InInstrumentName,
		FMusicChordNoteChannel const& InChordNoteChannel
	)
		: FMusicInstrumentComposition(InInstrumentName)
	{
		ChordNoteChannels.Add(InChordNoteChannel);
	}

	// the name of the instrument
	UPROPERTY(EditAnywhere)
	FName InstrumentName;

	// the set of note channels to play
	// each channel offers a single series of notes to play for a given interval
	// to play a chord made up of 5 notes played simultaneously, we need 5 channels
	UPROPERTY(EditAnywhere)
	TArray<FMusicChordNoteChannel> ChordNoteChannels;

	// apply a beat to each channel, return the results of each
	// notes are only added to the out array if they are new notes to play
	// ex:	channel:		1		2		3		4		5
	//		return		{	false,	false,	true,	false,	true	}
	//		OutNotes = {NewChannel3Note, NewChannel5Note}
	TArray<bool> GetNotesAtPulse(const EMusicNoteDurationType PulseDurationType, const int PulseNumber, TArray<FMusicNote>& OutNotes) const
	{
		TArray<bool> Out;
		for (int i = 0; i < ChordNoteChannels.Num(); i++)
		{
			FMusicNote NewNote;
			const bool bPulseResult = ChordNoteChannels[i].GetNoteAtPulse(PulseDurationType, PulseNumber, NewNote);
			Out.Add(bPulseResult);
			if (bPulseResult)
			{
				OutNotes.Add(NewNote);
			}
		}


		return Out;
	}

	// generates cached tone pulse data for every chord note channel
	TArray<FMusicSongToneChannel> GenerateSongToneChannels(const int ChannelLength, const EMusicNoteDurationType PulseDurationType) const
	{
		TArray<FMusicSongToneChannel> Out;
		for (int i = 0; i < ChordNoteChannels.Num(); i++)
		{
			Out.Add(FMusicSongToneChannel(ChordNoteChannels[i], PulseDurationType).FillWithRests(ChannelLength, PulseDurationType));
		}


		return Out;
	}

	// get the maximum number of required pulses to play through this composition
	int GetTotalPulses(const EMusicNoteDurationType PulseDurationType) const
	{
		int MaximumPulseCount = 0;
		for (int i = 0; i < ChordNoteChannels.Num(); i++)
		{
			const int InstrumentMaxPulses = ChordNoteChannels[i].GetTotalPulses(PulseDurationType);
			if (InstrumentMaxPulses > MaximumPulseCount)
			{
				MaximumPulseCount = InstrumentMaxPulses;
			}
		}


		return MaximumPulseCount;
	}

	FMusicInstrumentComposition& FillWithRests(const int DesiredPulseLength)
	{
		for (int i = 0; i < ChordNoteChannels.Num(); i++)
		{
			ChordNoteChannels[i].FillWithRests(DesiredPulseLength);
		}


		return *this;
	}

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

};


// a set of musical compositions for a section of a orchestration
// contains multiple instrumental compositions playing si
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicOrchestrationSection
{
	GENERATED_BODY()

public:

	FMusicOrchestrationSection()
		: Tempo(0.f)
		, InstrumentCompositions()
	{
	}

	FMusicOrchestrationSection(const float InTempo)
		: Tempo(InTempo)
		, InstrumentCompositions()
	{
	}

	FMusicOrchestrationSection(
		TArray<FMusicInstrumentComposition> const& InCompositions,
		const float InTempo
	)
		: FMusicOrchestrationSection(InTempo)
	{
		InstrumentCompositions.Append(InCompositions);
	}

	FMusicOrchestrationSection(
		FMusicInstrumentComposition const& InComposition,
		const float InTempo
	)
		: FMusicOrchestrationSection(InTempo)
	{
		InstrumentCompositions.Add(InComposition);
	}

	// music play rate, beats per minute (bpm)
	UPROPERTY(EditAnywhere)
	float Tempo;

	// get the time to play through this section, in seconds
	float GetTotalPlaytimeSeconds(const EMusicNoteDurationType PulseDurationType) const;

	// the set of musical compositions that play alongside eachother like instruments
	UPROPERTY(EditAnywhere)
	TArray<FMusicInstrumentComposition> InstrumentCompositions;

	int GetInstrumentIndex(FName const& InstrumentName) const;
	bool IsInstrumentIndexValid(const int InstrumentIndex) const;
	bool ContainsInstrument(FName const& InstrumentName) const;
	int GetInstrumentNoteChannelCount(FName const& InstrumentName) const;

	// get total number of variable tones required to play this orchestration
	// the summed number of note chennels used by each composition
	int GetTotalNumberOfVariableTones() const
	{
		int Tones = 0;
		for (int i = 0; i < InstrumentCompositions.Num(); i++)
		{
			Tones += InstrumentCompositions[i].ChordNoteChannels.Num();
		}


		return Tones;
	}

	// get the maxiumum number of required pulses to play through this section
	int GetTotalPulses(const EMusicNoteDurationType PulseDurationType) const
	{
		int MaximumPulseCount = 0;
		for (int i = 0; i < InstrumentCompositions.Num(); i++)
		{
			const int InstrumentMaxPulses = InstrumentCompositions[i].GetTotalPulses(PulseDurationType);
			if (InstrumentMaxPulses > MaximumPulseCount)
			{
				MaximumPulseCount = InstrumentMaxPulses;
			}
		}


		return MaximumPulseCount;
	}

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

};


// display data related to a music composition / song / orchestration
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicCompositionInfo
{
	GENERATED_BODY()

public:

	FMusicCompositionInfo()
		: Title()
		, Author()
		, TimeSignature()
	{
	}

	FMusicCompositionInfo(
		FText const& InTitle,
		FText const& InAuthor,
		FMusicTimeSignature const& InTimeSignature
	)
		: Title(InTitle)
		, Author(InAuthor)
		, TimeSignature(InTimeSignature)
	{
	}

	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY(EditAnywhere)
	FText Author;

	UPROPERTY(EditAnywhere)
	FMusicTimeSignature TimeSignature;

};


// properties used to generate music orchestrations
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicOrchestrationGenerator
{
	GENERATED_BODY()

public:

	FMusicOrchestrationGenerator()
		: Info()
	{
	}

	FMusicOrchestrationGenerator(
		FMusicCompositionInfo const& InInfo
	)
		: Info(InInfo)
	{
	}

	UPROPERTY(EditAnywhere)
	FMusicCompositionInfo Info;

};


// a fully complete music piece, made up of multiple sections that are played in series
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicOrchestration
{
	GENERATED_BODY()

public:

	FMusicOrchestration()
		: Info()
		, Sections()
	{
	}

	FMusicOrchestration(
		FMusicCompositionInfo const& InCompositionInfo,
		TArray<FMusicOrchestrationSection> const& InSections
	)
		: Info(InCompositionInfo)
		, Sections()
	{
		Sections.Append(InSections);
	}

	FMusicOrchestration(
		FMusicCompositionInfo const& InCompositionInfo,
		FMusicOrchestrationSection const& InSection
	)
		: FMusicOrchestration()
	{
		Info = InCompositionInfo;
		Sections.Add(InSection);
	}

	FMusicOrchestration(FMusicOrchestrationGenerator const& GeneratorProperties);

	// info about the music composition
	UPROPERTY(EditAnywhere)
	FMusicCompositionInfo Info;

	// the series of musical pieces to play in order
	UPROPERTY(EditAnywhere)
	TArray<FMusicOrchestrationSection> Sections;

	// get the required number of variable tones to play this piece
	int GetMaximumRequiredVariableTones() const
	{
		int MaximumToneCount = 0;
		for (int i = 0; i < Sections.Num(); i++)
		{
			const int SectionTotalTones = Sections[i].GetTotalNumberOfVariableTones();
			if (SectionTotalTones > MaximumToneCount)
			{
				MaximumToneCount = SectionTotalTones;
			}
		}


		return MaximumToneCount;
	}

	// get the number of pulses needed to play through this musical orchestration
	int GetTotalPulses() const
	{
		int PulseCount = 0;
		for (int i = 0; i < Sections.Num(); i++)
		{
			PulseCount += Sections[i].GetTotalPulses(FindSmallestNoteDurationType());
		}


		return PulseCount;
	}

	// get the time to play through this musical orchestration, in seconds
	float GetTotalPlaytimeSeconds() const
	{
		float Playtime = 0.f;
		for (int i = 0; i < Sections.Num(); i++)
		{
			Playtime += Sections[i].GetTotalPlaytimeSeconds(FindSmallestNoteDurationType());
		}


		return Playtime;
	}

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

};


// a pulse index that a tempo change occurs at
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicTempoChangePulseIndex
{
	GENERATED_BODY()

public:

	FMusicTempoChangePulseIndex()
		: PulseIndex(0)
		, PulsesPerSecond(0.f)
	{
	}

	FMusicTempoChangePulseIndex(const int InPulseIndex, const float InPulsesPerSecond)
		: PulseIndex(InPulseIndex)
		, PulsesPerSecond(InPulsesPerSecond)
	{
	}

	// the pulse index where tempo changes
	UPROPERTY(EditAnywhere)
	int PulseIndex;

	// the new tempo to start using, in pulses-per-second
	UPROPERTY(EditAnywhere)
	float PulsesPerSecond;

};


// list of tempo changes for a song
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicSongTempoChangelist
{
	GENERATED_BODY()

public:

	FMusicSongTempoChangelist()
		: TempoChangelist()
	{
	}

	// the pulse index where tempo changes
	UPROPERTY(EditAnywhere)
	TArray<FMusicTempoChangePulseIndex> TempoChangelist;

	int GetListIndexForPulse(const int PulseIndex) const
	{
		for (int i = 0; i < TempoChangelist.Num(); i++)
		{
			if (TempoChangelist[i].PulseIndex == PulseIndex)
			{
				return i;
			}
		}


		return -1;
	}

	bool IsListIndexValid(const int ListIndex) const
	{
		return ListIndex >= 0 && ListIndex < TempoChangelist.Num();
	}

	bool ContainsPulseIndex(const int PulseIndex) const
	{
		return IsListIndexValid(GetListIndexForPulse(PulseIndex));
	}

	// check for a change in tempo at the given pulse index
	// returns true if a new tempo value was found
	bool CheckForTempoChange(const int PulseIndex, float& NewTempoPps) const
	{
		const int ListIndex = GetListIndexForPulse(PulseIndex);
		if (IsListIndexValid(ListIndex) == false)
		{
			return false;
		}

		NewTempoPps = TempoChangelist[ListIndex].PulsesPerSecond;


		return true;
	}

	float GetStartingTempo() const
	{
		float StartingTempo;
		const bool bCheckResult = CheckForTempoChange(0, StartingTempo);
		if (bCheckResult == false)
		{
			return 0.f;
		}

		return StartingTempo;
	}

	bool IsValid() const
	{
		const float StartingTempo = GetStartingTempo();
		return StartingTempo > 0.f && FMath::IsNearlyZero(StartingTempo) == false;
	}

	// get the largest pulse index in the list that is prior to the given index
	int GetHighestPulseIndexPriorTo(const int PulseIndex) const;

	// try adding a tempo at the pulse index
	// if it is not a change in tempo from the previous pulse index, it will not be added
	void RegisterSectionTempo(const int SectionPulseIndex, float SectionTempo);

};


// custom iterator that helps manage quartz, pulses, and music beats from a single iterator
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicBeatIterator
{
	GENERATED_BODY()

public:

	FMusicBeatIterator()
		: QuartzBeatIterator(0)
		, QuartzBeatsPerPulseBeat(0)
		, QuartzBeatsPerMusicBeat(0)
		, MusicBeatsPerBar(0)
		, PulseBeatsPerMusicBeat(0.f)
		, TotalPulses(0)
	{
	}

	FMusicBeatIterator(
		const int InQuartzBeatsPerPulseBeat,
		const int InQuartzBeatsPerMusicBeat,
		const int InMusicBeatsPerBar,
		const float InPulseBeatsPerMusicBeat,
		const int InTotalPulses)
		: QuartzBeatIterator(0)
		, QuartzBeatsPerPulseBeat(InQuartzBeatsPerPulseBeat)
		, QuartzBeatsPerMusicBeat(InQuartzBeatsPerMusicBeat)
		, MusicBeatsPerBar(InMusicBeatsPerBar)
		, PulseBeatsPerMusicBeat(InPulseBeatsPerMusicBeat)
		, TotalPulses(InTotalPulses)
	{
	}

	UPROPERTY()
	int QuartzBeatIterator;

	UPROPERTY()
	int QuartzBeatsPerPulseBeat;

	UPROPERTY()
	int QuartzBeatsPerMusicBeat;

	UPROPERTY()
	int MusicBeatsPerBar;

	UPROPERTY()
	float PulseBeatsPerMusicBeat;

	UPROPERTY()
	int TotalPulses;

	// resets the internal iterators to zero
	void Reset()
	{
		QuartzBeatIterator = 0;
	}

	// iterate this iterator forward by the given number of quartz beats
	FMusicBeatIterator& IterateQuartzBeats(const int QuartzBeats);

	// iterate this iterator forward by the given number of quartz beats
	FMusicBeatIterator operator+(const int QuartzBeats) const
	{
		return FMusicBeatIterator(*this).IterateQuartzBeats(QuartzBeats);
	}

	// iterate this iterator forward by the given number of quartz beats
	FMusicBeatIterator& operator+=(const int QuartzBeats)
	{
		return IterateQuartzBeats(QuartzBeats);
	}

	// iterate this iterator forward by the given number of quartz beats
	FMusicBeatIterator operator-(const int QuartzBeats) const
	{
		return *this + (-QuartzBeats);
	}

	// iterate this iterator forward by the given number of quartz beats
	FMusicBeatIterator& operator-=(const int QuartzBeats)
	{
		return IterateQuartzBeats(-QuartzBeats);
	}

	// get the current pulse beat index
	int GetCurrentPulseBeat() const;

	// check if the current iteration is a pulse beat
	bool IsPulseBeat() const;

	// check if the current iteration is a music beat
	bool IsMusicBeat() const;

	// check if the last music beat has been passed over
	bool HasPassedLastMusicBeat() const;

	// get the next queued music beat number that has been primed
	// and readied to play on the next beat iteration
	int GetCurrentQueuedMusicBeat() const;

	// get the music beat number for the current audible tone
	// audio is delayed by one beat as it needs to be primed
	// and readied to play on the next beat iteration
	int GetCurrentAudibleMusicBeat() const;

	// get number of played quartz beats
	int GetTotalQuartzBeats() const { return QuartzBeatsPerPulseBeat * TotalPulses; }

	// get number of played quartz beats
	int GetTotalPlayedQuartzBeats() const { return QuartzBeatIterator; }

	// get number of played pulse beats
	int GetTotalPlayedPulseBeats() const { return GetCurrentPulseBeat(); }

	// get number of played music beats
	int GetTotalPlayedMusicBeats() const;

};


// data needed to play a musical orchestration at runtime
// pre-cached version of an FMusicOrchestration, to support playback of the piece in constant O(1) time
// contains cached results of each FMusicInstrumentComposition::GetNotesAtPulse() for every pulse
// and a cached list of tempo changes, associated with the pulse index that they occur
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FMusicSong
{
	GENERATED_BODY()

public:

	FMusicSong()
		: Info()
		, ToneChannels()
		, TempoChangelist()
		, _PlaybackTimeSeconds(0.f)
		, _PulseDurationType(EMusicNoteDurationType::whole)
	{
	}

	FMusicSong(FMusicOrchestration const& InMusicOrchestrationSection);

	bool IsValid() const
	{
		return TempoChangelist.IsValid() &&
			GetTotalPulses() > 0;
	}

	// info about the music composition
	UPROPERTY(EditAnywhere)
	FMusicCompositionInfo Info;

	// cached data for each pulse to manipulate a set of variable tones at runtime
	UPROPERTY(EditAnywhere)
	TArray<FMusicSongToneChannel> ToneChannels;

	// a list of tempo changes, and the pulse index in which they occur
	UPROPERTY(EditAnywhere)
	FMusicSongTempoChangelist TempoChangelist;

	// get the initial tempo, in pulses-per-second
	float GetStartingTempo() const { return TempoChangelist.GetStartingTempo(); }

	// apply a pulse to the song, fetching data to apply to variable tones, and any tempo changes
	// returns true if a tempo change occurs
	// this is used to play the song at runtime
	bool GetPulsePlaybackData(const int PulseIndex, TArray<FMusicSongToneData>& OutToneData, float& OutTempoChange) const;

	// get total pulses required to play through this song
	int GetTotalPulses() const
	{
		int MaximumRequiredPulses = 0;
		for (int i = 0; i < ToneChannels.Num(); i++)
		{
			if (ToneChannels[i].PulseData.Num() > MaximumRequiredPulses)
			{
				MaximumRequiredPulses = ToneChannels[i].PulseData.Num();
			}
		}


		return MaximumRequiredPulses;
	}

	// get total play time
	float GetPlaybackTimeSeconds() const { return _PlaybackTimeSeconds; }

	// check if the given channel index is valid
	bool IsChannelIndexValid(const int ChannelIndex) const
	{
		return ChannelIndex >= 0 || ChannelIndex < ToneChannels.Num();
	}

	// get the notes in the given channel index for the given bar number
	// bar number is indexed from 1 to the number of bars
	TArray<FMusicSongToneData> GetNotesInChannelForBar(const int ChannelIndex, const int BarNumber, bool bIncludeToneChangesOnly) const;

	// get the note in the given channel index for the given bar and beat number
	// bar and beat numbers are indexed from 1 to the number of bars / beats
	FMusicSongToneData GetNoteInChannelForBarBeat(const int ChannelIndex, const int BarNumber, const int BeatNumber) const;

	// get the total number of bars
	int GetTotalNumberOfBars() const;

	// check if the given bar number is in the valid range
	// bar number is indexed from 1 to the number of bars
	bool IsValidMusicBarNumber(const int BarNumber) const
	{
		const int BarIndex = BarNumber - 1;
		return BarIndex < 0 && BarIndex >= GetTotalNumberOfBars();
	}

	// check if the given beat number is in the valid range
	// bar number is indexed from 1 to the number of bars
	bool IsValidMusicBeatNumber(const int MusicBeatNumber) const
	{
		const int BeatIndex = MusicBeatNumber - 1;
		return BeatIndex < 0 && BeatIndex >= Info.TimeSignature.BeatsPerMeasure;
	}

	// convert a number of beats into a number of bars
	static int GetNumberOfBarsFromBeats(const int NumberOfBeats, const int BeatsPerBar);

	// convert a number of beats into a number of bars
	static int GetNumberOfBeatsFromBars(const int NumberOfBars, const int BeatsPerBar);

	// get the number of pulses in the given channel
	int GetNumberOfPulsesInChannel(const int ChannelIndex) const;

	// get the number of bars in the given channel
	int GetNumberOfBarsInChannel(const int ChannelIndex) const;

	// search for the smallest used note duration type
	EMusicNoteDurationType FindSmallestNoteDurationType() const;

	// get the duration type that a single pulse represents
	EMusicNoteDurationType GetPulseDurationType() const { return _PulseDurationType; }

	// get the number of pulses needed to play a single whole note
	int GetPulsesPerWholeNote() const;

	// get the number of pulses needed to play a single music beat
	float GetPulsesPerMusicBeat() const;

	// get the number of quartz beats needed to play a single music beat
	int GetQuartzBeatsPerMusicBeat() const;

	// get the number of quartz beats needed to play a single whole note
	int GetQuartzBeatsPerWholeNote() const;

	// get the number of quartz beats needed to play a single pulse
	int GetQuartzBeatsPerPulse() const;

	// generate quartz clock settings needed to play this song
	FQuartzClockSettings MakeQuartzClockSettings() const;

	// generate beat iterator to manage music, pulse, and quartz beats
	FMusicBeatIterator MakeBeatIterator() const;

private:

	// cached playback time, in seconds, calculated from the converted orchestration
	UPROPERTY()
	float _PlaybackTimeSeconds;

	// the duration type that is represented by a single pulse
	UPROPERTY()
	EMusicNoteDurationType _PulseDurationType;

};


// music song classifications
UENUM(BlueprintType)
enum class EMusicSongGenre : uint8
{
	classical		UMETA(DisplayName = "Classical"),
	rock			UMETA(DisplayName = "Rock"),
	metal			UMETA(DisplayName = "Metal"),
	r_and_b			UMETA(DisplayName = "R&B"),
	country			UMETA(DisplayName = "Country"),
	jazz			UMETA(DisplayName = "Jazz"),
	rap				UMETA(DisplayName = "Rap"),
	hip_hop			UMETA(DisplayName = "Hip Hop"),
	latin			UMETA(DisplayName = "Latin"),
	electronic		UMETA(DisplayName = "Electronic"),

	lofi			UMETA(DisplayName = "LoFi"),
};


// song chronological section types
// provides classifications for the pieces that divide a song into a series, that are played one at at time
UENUM(BlueprintType)
enum class EMusicSongChronologicalSection : uint8
{
	// the beginning piece
	intro		UMETA(DisplayName = "Intro"),

	// a repeated melody with unique lyrics
	// provides background and setting to lead us to the meaning of the song
	verse		UMETA(DisplayName = "Verse"),

	// a completely repeated section
	// cuts right to the chase to the meaning of the song
	chorus		UMETA(DisplayName = "Chorus"),

	// unique melody with unique lyrics
	// provides relief from reptition, and helps transition to different verse-chorus segments
	bridge		UMETA(DisplayName = "Bridge"),

	// the ending piece
	outro		UMETA(DisplayName = "Outro"),
};


// musical clef types
UENUM(BlueprintType)
enum class EMusicClefTypes : uint8
{
	// each line and space is assigned to a percussion instrument with no precise pitch
	neutral		UMETA(DisplayName = "Neutral"),

	// an F-clef focused around the first F note below middle C (F3)
	bass		UMETA(DisplayName = "Bass"),

	// a G-clef focused around the first G note above middle C (G4)
	treble		UMETA(DisplayName = "Treble"),

	// a C-clef focused around middle C, with the middle line on C4
	// mainly used for the viola, viola de gamba, the alto trambone, and the mandola
	alto		UMETA(DisplayName = "Alto"),

	// a C-clef focused around middle C, with the second line from the top on C4
	// mainly used for the bassoon, cello, euphonium, double bass, and trombone
	tenor		UMETA(DisplayName = "Tenor"),
};


// song instrumental section types
// provides classifications for the pieces that divide a song into different sound sources, each playing together in parallel
UENUM(BlueprintType)
enum class EMusicSongInstrumentalSection : uint8
{
	// beat-maker, usually percussion
	beat		UMETA(DisplayName = "Beat"),

	// lower tone-setter that plays closer to the bass clef
	bass		UMETA(DisplayName = "Bass"),

	// upper tone-setter that plays closer to the treble clef
	treble		UMETA(DisplayName = "Treble"),

	// speaker, guide, leader, message
	melody		UMETA(DisplayName = "Melody"),
};


// song generation methods for procedurally created music
UENUM(BlueprintType)
enum class EMusicSongGenerationMethod : uint8
{
	// create a song using the AA-BA-BA pattern
	aababa,
};


/**
 * Functions and constants for musical types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UMusicTypes : public UObject
{
	GENERATED_BODY()

public:

	// total number of musical tones (using western classical system as the base)
	static const int c_NumberOfMusicTones;

	// the highest audible octave
	static const int c_HighestAudibleOctave;

	// the center octave between limits in the audible range
	static const int c_MiddleOctave;

	// the common central note on the staff
	static const FMusicTone c_MiddleC;

	// ratio of frequencies from one semitone to the next raised semitone
	// multiply by the frequency of a tone to produce a tone that is one semitone above
	static const float c_SemitoneFrequencyRatio;

	// ratio of frequencies from one semtione to the previous lowered semitone
	// multiply by the frequency of a tone to produce a tone that is one semitone below
	static const float c_InverseSemitoneFrequencyRatio;

	// ratio used to convert from Bpm (beats-per-minute) to Hertz frequency (beats-per-second)
	static const float c_BpmToHzRatio;

	// ratio used to convert from Hertz frequency (beats-per-second) to Bpm (beats-per-minute)
	static const float c_HzToBpmRatio;

	// music tone frequencies
	static const float c_MusicToneFrequency_C0;
	static const float c_MusicToneFrequency_Db0;
	static const float c_MusicToneFrequency_D0;
	static const float c_MusicToneFrequency_Eb0;
	static const float c_MusicToneFrequency_E0;
	static const float c_MusicToneFrequency_F0;
	static const float c_MusicToneFrequency_Gb0;
	static const float c_MusicToneFrequency_G0;
	static const float c_MusicToneFrequency_Ab0;
	static const float c_MusicToneFrequency_A0;
	static const float c_MusicToneFrequency_Bb0;
	static const float c_MusicToneFrequency_B0;


	// music tempo Bpm's
	static const float c_MusicTempoBpm_Center_Grave;
	static const float c_MusicTempoBpm_Upper__Grave;

	static const float c_MusicTempoBpm_Center_Lento;
	static const float c_MusicTempoBpm_Upper__Lento;

	static const float c_MusicTempoBpm_Center_Largo;
	static const float c_MusicTempoBpm_Upper__Largo;

	static const float c_MusicTempoBpm_Center_Larghetto;
	static const float c_MusicTempoBpm_Upper__Larghetto;

	static const float c_MusicTempoBpm_Center_Adagio;
	static const float c_MusicTempoBpm_Upper__Adagio;

	static const float c_MusicTempoBpm_Center_Adagietto;
	static const float c_MusicTempoBpm_Upper__Adagietto;

	static const float c_MusicTempoBpm_Center_Andante;
	static const float c_MusicTempoBpm_Upper__Andante;

	static const float c_MusicTempoBpm_Center_Moderato;
	static const float c_MusicTempoBpm_Upper__Moderato;

	static const float c_MusicTempoBpm_Center_Allegretto;
	static const float c_MusicTempoBpm_Upper__Allegretto;

	static const float c_MusicTempoBpm_Center_Allegro;
	static const float c_MusicTempoBpm_Upper__Allegro;

	static const float c_MusicTempoBpm_Center_Vivace;
	static const float c_MusicTempoBpm_Upper__Vivace;

	static const float c_MusicTempoBpm_Center_Presto;
	static const float c_MusicTempoBpm_Upper__Presto;

	static const float c_MusicTempoBpm_Center_Prestissimo;
	static const float c_MusicTempoBpm_Upper__Prestissimo;


	// get the base frequency (octave zero), in Hertz, for a given musical note
	static float GetMusicToneBaseFrequency(const EMusicNoteToneType Note);

	// get the frequency, in Hertz, for a given musical note and octave
	// todo
	// this is not very accurate, error grows pretty rapidly, could be better implemented
	static float GetMusicToneFrequency(const EMusicNoteToneType Note, const int Octave);

	// get the distance, in semitones, from one note to another within the same octave
	static int GetSemitoneDistanceBetweenMusicTonesWithinOctave(const EMusicNoteToneType NoteA, const EMusicNoteToneType NoteB);

	// check if two given music notes are equivalent
	static bool AreMusicTonesEquivalent(const EMusicNoteToneType NoteA, const EMusicNoteToneType NoteB);

	// normalize the given music note to an equivalent form
	// ex: B# becomes C, Fb becomes E, etc.
	static EMusicNoteToneType NormalizeMusicTone(const EMusicNoteToneType Note);

	// apply a sharp adjustment to the given music note (plus one semitone)
	static EMusicNoteToneType MakeMusicToneSharp(const EMusicNoteToneType Note);

	// apply a flat adjustment to the given music note (minus one semitone)
	static EMusicNoteToneType MakeMusicToneFlat(const EMusicNoteToneType Note);

	// apply a sharp adjustment to the given music note (adjusts by one or zero semitones, depending on which note is provided)
	static EMusicNoteToneType MakeMusicToneNatural(const EMusicNoteToneType Note);

	// change a music note tone type in a positive direction
	// includes the rest note, and has no musical relevance
	// useful for ui that allows players to change a note
	static EMusicNoteToneType IterateMusicToneForward(const EMusicNoteToneType Note);

	// change a music note tone type in a negative direction
	// includes the rest note, and has no musical relevance
	// useful for ui that allows players to change a note
	static EMusicNoteToneType IterateMusicToneBackward(const EMusicNoteToneType Note);

	// change a music note duration type in a positive direction
	// useful for ui that allows players to change a note
	static EMusicNoteDurationType IterateMusicDurationTypeForward(const EMusicNoteDurationType Note);

	// change a music note duration type in a negative direction
	// useful for ui that allows players to change a note
	static EMusicNoteDurationType IterateMusicDurationTypeBackward(const EMusicNoteDurationType Note);

	// get the distance, in semitones, between the given music note and the 'C' note in the next higher octave
	static int GetSemitoneDistanceToHigherOctaveRoot(const EMusicNoteToneType Note);

	// get the distance, in semitones, between the given music note and the 'C' note within this octave
	static int GetSemitoneDistanceToOctaveRoot(const EMusicNoteToneType Note);

	// get the list of semitonal distances for all notes of the given scale type from the root node
	// used to determine the set of notes in a scale
	static TArray<int> GetScaleNoteSemitonalDifferentials(const EMusicScale Scale);

	// get the list of music notes in the given scale starting from the given root note
	static TArray<FMusicTone> GetMusicTonesInScale(const EMusicScale Scale, const FMusicTone RootNote);

	// get the value associated with a note's duration type
	static int GetMusicNoteDurationTypeInvertedBeatWeight(const EMusicNoteDurationType DurationType)
	{
		return 1 << static_cast<int>(DurationType);
	}

	// get the beat weight value of a given music note duration type
	static int GetMusicNoteDurationTypeBeatWeight(const EMusicNoteDurationType DurationType);

	// get the beat weight value of a given music note duration type
	static int GetMusicNoteDurationTypePulseCount(const EMusicNoteDurationType DurationType, const EMusicNoteDurationType PulseDurationType);

	// the note duration value that corresponds with a single pulse
	static const EMusicNoteDurationType c_SinglePulseDuration;

	// value represents the largest supported number of pulses per beat
	// based on the number of enumerators in EMusicNoteDurationType
	static const int c_LargestSupportedPulsesPerBeat;

	// multiply by a value in beats per minute to convert to pulses per second
	// beats * pulse-minutes   pulses
	//------------------ =---
	// minutes * beat-seconds  seconds
	static const float c_LargestSupportedPulseMinutesPerBeatSeconds;

	// check if the given BeatsPerWholeNote time signature denominator value is valid
	static bool IsValidTimeSignatureBeatsPerWholeNote(const int BeatsPerWholeNote);

	// get quartz quantization time signature denominator from beats per whole note
	static EQuartzTimeSignatureQuantization GetQuartzTimeSignatureQuantization(const int BeatsPerWholeNote);

	// get beats per minute associated with a music tempo
	static EMusicTempo GetTempoTypeFromBpm(const float TempoBpm);

	// get beats per minute associated with a music tempo
	static float GetBpmFromTempoType(const EMusicTempo Tempo);

	// randomly select a scale based on the given genre
	static EMusicScale RandomScale(const EMusicSongGenre Genre, FName const& Seed = FName());

	// randomly select a tempo based on the given genre
	static EMusicTempo RandomTempoType(const EMusicSongGenre Genre, const EMusicSongChronologicalSection ChronologicalType, FName const& Seed = FName());

	// randomly select a tone type
	static EMusicNoteToneType RandomToneType(bool bIncludeRest, FName const& Seed = FName());

	// randomly select a tone octave
	static int RandomToneOctave(FName const& Seed = FName());

	// randomly select a tone
	static FMusicTone RandomTone(bool bIncludeRest, FName const& Seed = FName());

	// randomly select a music genre
	static EMusicSongGenre RandomGenre(FName const& Seed = FName());

	// randomly select a music song generation method for procedural music
	static EMusicSongGenerationMethod RandomGenerationMethod(FName const& Seed = FName());

	// randomly generate a tone in a given scale
	static FMusicTone RandomToneFromScale(const EMusicScale Scale, FMusicTone const& RootTone, FName const& Seed = FName());

	static FMusicInstrumentComposition GenerateInstrumentComposition(
		FMusicTimeSignature const& TimeSignature,
		const FMusicTone RootTone,
		const EMusicSongGenre Genre,
		const EMusicScale Scale,
		const EMusicSongChronologicalSection ChronologicalType,
		const EMusicSongInstrumentalSection InstrumentType,
		const int NumMeasures
	);

	// expiremental function for randomly generating a section of a song orchestration
	static FMusicOrchestrationSection GenerateOrchestrationSection(
		FMusicTimeSignature const& TimeSignature,
		FMusicTone const& RootTone,
		const EMusicSongGenre Genre,
		const EMusicSongChronologicalSection ChronologicalType
	);

	// expiremental function for randomly generating a song orchestration
	static FMusicOrchestration GenerateOrchestration(
		FMusicTimeSignature const& TimeSignature,
		FMusicTone const& RootTone,
		const EMusicSongGenre Genre
	);

	// expiremental function for randomly generating a song orchestration
	static FMusicOrchestration GenerateOrchestrationMethod_AABABA(
		FMusicTimeSignature const& TimeSignature,
		FMusicTone const& RootTone,
		const EMusicSongGenre Genre
	);



};
