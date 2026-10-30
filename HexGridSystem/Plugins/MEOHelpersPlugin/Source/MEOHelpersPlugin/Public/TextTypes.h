// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Misc/Char.h"
#include "TextTypes.generated.h"


// a character in an alphabet with an upper and lower case
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FAlphabetCharacter
{
	GENERATED_BODY()

public:

	FAlphabetCharacter()
		: Lowercase()
		, Uppercase()
	{
	}

	FAlphabetCharacter(FString const& InLowercase)
		: Lowercase(InLowercase)
		, Uppercase()
	{
	}

	FAlphabetCharacter(FString const& InLowercase, FString const& InUppercase)
		: Lowercase(InLowercase)
		, Uppercase(InUppercase)
	{
	}

	UPROPERTY()
	FString Lowercase;

	UPROPERTY()
	FString Uppercase;

	bool HasUpper() const
	{
		return Uppercase.Len() == 1;
	}

	bool HasLower() const
	{
		return Lowercase.Len() == 1;
	}

	bool IsNull() const
	{
		return !HasUpper() && !HasLower();
	}

	FString GetAny() const
	{
		return HasLower() ? Lowercase : Uppercase;
	}

	FString GetCase(bool bUseLowercase) const
	{
		return bUseLowercase ? Lowercase : Uppercase;
	}

};


// an alphabet of characters
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FAlphabet
{
	GENERATED_BODY()

public:

	FAlphabet()
		: Characters()
	{
	}

	FAlphabet(TArray<FAlphabetCharacter> const& InCharacters)
		: FAlphabet()
	{
		Characters.Append(InCharacters);
	}

	FAlphabet(FString const& InAlphabet_Lowercase);

	FAlphabet(FString const& InAlphabet_Lowercase, FString const& InAlphabet_Uppercase);

	UPROPERTY()
	TArray<FAlphabetCharacter> Characters;

	FString ToString(bool bUseLowercase = true) const;

	int GetIndexOfCharacter(FString const& Character) const;

	bool ContainsCharacter(FString const& Character) const;

	// check if the contained character is lowercase or not
	// only valid if ContainsCharacter returns true
	bool IsContainedCharacterLowercase(FString const& ContainedCharacter) const;

	bool HasUppercase() const;
	bool HasLowercase() const;
	bool IsEmpty() const
	{
		return !HasUppercase() && !HasLowercase();
	}

	// create a copy of this alphabet with it's characters in reverse order
	FAlphabet Reverse() const
	{
		return FAlphabet(ToString(true).Reverse(), ToString(false).Reverse());
	}

	// create a copy of this alphabet with it's characters in a randomized order
	// uppercase and lowercase letters will remain aligned
	FAlphabet Shuffle(FName const& Seed = FName()) const;

};

/**
 * Functions and constants for text types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UTextTypes : public UObject
{
	GENERATED_BODY()

public:

	static FString GetStringifiedCharacter(FString const& String, const int CharacterIndex)
	{
		return FString() + String[CharacterIndex];
	}

	// transliterate a given character by converting from a given alphabet into another
	// this only works for alphabets that have equal sizes
	static FString TransliterateCharacter(
		FAlphabet const& FromAlphabet,
		FAlphabet const& ToAlphabet,
		FString const& Character,
		bool bUseOnlyOneCase,
		bool bUseOnlyLowerCase
	);

	// transliterate a given string by converting all instances of characters from a given alphabet into another
	// this only works for alphabets that have equal sizes
	static FString TransliterateString(
		FAlphabet const& FromAlphabet,
		FAlphabet const& ToAlphabet,
		FString const& String
	);

	// the alphabet of latin / english characters
	static const FAlphabet c_Alphabet_English;

	// the alphabet of greek characters
	static const FAlphabet c_Alphabet_Greek;

	// the alphabet of gothic characters
	static const FAlphabet c_Alphabet_Gothic;

	// converts the latin / english alphabet character
	// in the given string to a gothic character, by index 
	static FString GothifyCharacter(FString const& Character);

	// converts all the latin / english alphabet characters
	// in the given string to gothic characters, by index 
	static FString GothifyString(FString const& String);


	// get a randomized player name FText to use while debugging
	static FText GetRandomizedDebugPlayerNameText();


	// append two FText's with an optional \n newline in between
	// the returned text is culture invariant (because we cannot provide localization for two unknown strings mashed together)
	static FText AppendText(FText const& InTextA, FText const& InTextB, bool bDoNotNewline = false);


	// append two FText's with a specific separating character in between
	// the returned text is culture invariant (because we cannot provide localization for two unknown strings mashed together)
	static FText AppendText(FText const& InTextA, FText const& InTextB, FString const& SeparatorString);


	// quick number formatting options
	static FNumberFormattingOptions GetNumberFormattingOptions_Generic();
	static FNumberFormattingOptions GetNumberFormattingOptions_AlwaysSign();
	static FNumberFormattingOptions GetNumberFormattingOptions_AlwaysFraction();
	static FNumberFormattingOptions GetNumberFormattingOptions_AlwaysSign_AlwaysFraction();
	static FNumberFormattingOptions GetNumberFormattingOptions_NoFraction();
	static FNumberFormattingOptions GetNumberFormattingOptions_AlwaysSign_NoFraction();
	static FNumberFormattingOptions GetNumberFormattingOptions_Percentage();
	static FNumberFormattingOptions GetNumberFormattingOptions_Percentage_AlwaysSign();
	static FNumberFormattingOptions GetNumberFormattingOptions_Percentage_NoFraction();
	static FNumberFormattingOptions GetNumberFormattingOptions_Percentage_AlwaysSign_NoFraction();


};