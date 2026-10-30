// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "TextTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "MathTypes.h"


const FAlphabet UTextTypes::c_Alphabet_English = FAlphabet(TEXT("abcdefghijklmnopqrstuvwxyz"), TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
const FAlphabet UTextTypes::c_Alphabet_Greek = FAlphabet(TEXT("αβγδεζηθικλμνξοπρστυφχψω"), TEXT("ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ"));
const FAlphabet UTextTypes::c_Alphabet_Gothic = FAlphabet(TEXT("𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹𐌺𐌻𐌼𐌽𐌾𐌿𐍀𐍁𐍂𐍃𐍄𐍅𐍆𐍇𐍈𐍉"));


FAlphabet::FAlphabet(FString const& InAlphabet_Lowercase)
	: FAlphabet()
{
	FOR_FORWARD(i, InAlphabet_Lowercase.Len())
	{
		if (InAlphabet_Lowercase[i] == '\0')
		{
			break;
		}

		Characters.Add(
			FAlphabetCharacter(
				UTextTypes::GetStringifiedCharacter(InAlphabet_Lowercase, i)
			)
		);
	}
}

FAlphabet::FAlphabet(FString const& InAlphabet_Lowercase, FString const& InAlphabet_Uppercase)
	: FAlphabet()
{
	if (InAlphabet_Lowercase.Len() == InAlphabet_Uppercase.Len())
	{
		FOR_FORWARD(i, InAlphabet_Lowercase.Len())
		{
			if (InAlphabet_Lowercase[i] == '\0' || InAlphabet_Uppercase[i] == '\0')
			{
				break;
			}

			Characters.Add(
				FAlphabetCharacter(
					UTextTypes::GetStringifiedCharacter(InAlphabet_Lowercase, i),
					UTextTypes::GetStringifiedCharacter(InAlphabet_Uppercase, i)
				)
			);
		}
	}
	else
	{
		L_ERR(MEOHelpers, "alphabet casing length mismatched '{0}' size {1} : '{2}' size {3}",
			InAlphabet_Lowercase,
			InAlphabet_Lowercase.Len(),
			InAlphabet_Uppercase,
			InAlphabet_Uppercase.Len());
	}
}


FString FAlphabet::ToString(bool bUseLowercase) const
{
	FString Out;
	FOR_FORWARD(i, Characters.Num())
	{
		Out += Characters[i].GetCase(bUseLowercase);
	}


	return Out;
}


int FAlphabet::GetIndexOfCharacter(FString const& Character) const
{
	FOR_FORWARD(i, Characters.Num())
	{
		if ((Characters[i].HasLower() && Characters[i].GetCase(true).Equals(Character)) ||
			(Characters[i].HasUpper() && Characters[i].GetCase(false).Equals(Character)))
		{
			return i;
		}
	}


	return -1;
}


bool FAlphabet::ContainsCharacter(FString const& Character) const
{
	return UMCommon::IsValidIndex(Characters, GetIndexOfCharacter(Character));
}


bool FAlphabet::IsContainedCharacterLowercase(FString const& ContainedCharacter) const
{
	FOR_FORWARD(i, Characters.Num())
	{
		if (Characters[i].HasLower() && Characters[i].GetCase(true).Equals(ContainedCharacter))
		{
			return true;
		}
		else if (Characters[i].HasUpper() && Characters[i].GetCase(false).Equals(ContainedCharacter))
		{
			return false;
		}
	}

	L_ERR(MEOHelpers, "Alphabet does not contain the character '{0}'", ContainedCharacter);


	return false;
}


bool FAlphabet::HasUppercase() const
{
	FOR_FORWARD(i, Characters.Num())
	{
		if (Characters[i].HasUpper() == false)
		{
			return false;
		}
	}


	return Characters.Num() > 0;
}


bool FAlphabet::HasLowercase() const
{
	FOR_FORWARD(i, Characters.Num())
	{
		if (Characters[i].HasLower() == false)
		{
			return false;
		}
	}


	return Characters.Num() > 0;
}


FAlphabet FAlphabet::Shuffle(FName const& Seed) const
{
	TArray<FAlphabetCharacter> CharacterList;
	CharacterList.Append(Characters);
	UMathTypes::RandomizeArray(CharacterList, Seed);


	return FAlphabet(CharacterList);
}


FText UTextTypes::AppendText(FText const& InTextA, FText const& InTextB, bool bDoNotNewline)
{
	const FString Separator = bDoNotNewline ? TEXT(" ") : TEXT("\n");


	return UTextTypes::AppendText(InTextA, InTextB, Separator);
}


FText UTextTypes::AppendText(FText const& InTextA, FText const& InTextB, FString const& SeparatorString)
{
	FFormatNamedArguments LocalArgs;
	LocalArgs.Add("InTextA", InTextA);
	LocalArgs.Add("InTextB", InTextB);
	LocalArgs.Add("Separator", FText::FromString(SeparatorString));


	return FText::Format(NSLOCTEXT("MEOHelpers", "UTextTypes::AppendText", "{InTextA}{Separator}{InTextB}"), LocalArgs);
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_Generic()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_AlwaysSign()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(true);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_NoFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_AlwaysSign_NoFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(true);
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_Percentage()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_Percentage_NoFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_Percentage_AlwaysSign_NoFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(true);
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_Percentage_AlwaysSign()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(true);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_AlwaysSign_AlwaysFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(true);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(2);


	return NumberFormattingOptions;
}


FNumberFormattingOptions UTextTypes::GetNumberFormattingOptions_AlwaysFraction()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::HalfFromZero);
	NumberFormattingOptions.SetUseGrouping(true);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumFractionalDigits(2);
	NumberFormattingOptions.SetMinimumFractionalDigits(2);


	return NumberFormattingOptions;
}


FText UTextTypes::GetRandomizedDebugPlayerNameText()
{
	// todo
	// better name generation

	// for now, just randomly pick from a static list
	TArray<FText> StaticPlayerNameList;

	// names

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Futz")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Futzbuckle5")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Perp")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Moustachio")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Patounir")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Cmoaquhdipauxto")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Tiggol Bitties")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Triptych")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Choocher")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("The Amazingly Randy")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Randy")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("RageCage")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("CaptainAhab")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Ithos")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Lil Timmy")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Steve")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Stevie")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Fred")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Bob")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Joe")));
	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("Nip")));

	StaticPlayerNameList.Add(FText::AsCultureInvariant(TEXT("TheDude420")));

	//

	const int RandomIndex = UMathTypes::GetRandomIndex(StaticPlayerNameList);


	if (StaticPlayerNameList.Num() <= 0)
	{
		L_ERR(MEOHelpers, "static player name list is empty");
		return FText();
	}


	return StaticPlayerNameList[RandomIndex];
}


FString UTextTypes::TransliterateCharacter(
	FAlphabet const& FromAlphabet,
	FAlphabet const& ToAlphabet,
	FString const& Character,
	bool bUseOnlyOneCase,
	bool bUseOnlyLowerCase
)
{
	if (FromAlphabet.Characters.Num() == ToAlphabet.Characters.Num())
	{
		if (FromAlphabet.ContainsCharacter(Character))
		{
			bool bUseLowercase = true;
			if (bUseOnlyOneCase == false)
			{
				bUseLowercase = FromAlphabet.IsContainedCharacterLowercase(Character);
			}
			else
			{
				bUseLowercase = bUseOnlyLowerCase;
			}

			const int Index = FromAlphabet.GetIndexOfCharacter(Character);
			return ToAlphabet.Characters[Index].GetCase(bUseLowercase);
		}
	}
	else
	{
		L_WARN(MEOHelpers, "Alphabet lengths mismatched, FromAlphabet: '{0}' size {1}, ToAlphabet: '{2}' size {3}",
			FromAlphabet.ToString(),
			FromAlphabet.ToString().Len(),
			ToAlphabet.ToString(),
			ToAlphabet.ToString().Len()
		);
	}


	return Character;
}


FString UTextTypes::TransliterateString(
	FAlphabet const& FromAlphabet,
	FAlphabet const& ToAlphabet,
	FString const& String
)
{
	if (ToAlphabet.IsEmpty())
	{
		L_ERR(MEOHelpers, "ToAlphabet is empty");
		return String;
	}
	if (FromAlphabet.IsEmpty())
	{
		L_ERR(MEOHelpers, "FromAlphabet is empty");
		return String;
	}

	const bool bToAlphabetHasUppercase = ToAlphabet.HasUppercase();
	const bool bToAlphabetHasLowercase = ToAlphabet.HasLowercase();
	bool bOnlyUseOneCase = false;
	bool bOnlyUseLowerCase = true;
	if (bToAlphabetHasLowercase && !bToAlphabetHasUppercase)
	{
		bOnlyUseOneCase = true;
		bOnlyUseLowerCase = true;
	}
	else if (bToAlphabetHasUppercase && !bToAlphabetHasLowercase)
	{
		bOnlyUseOneCase = true;
		bOnlyUseLowerCase = false;
	}

	FString Out;
	FOR_FORWARD(i, String.Len())
	{
		const FString CharacterIterator = UTextTypes::GetStringifiedCharacter(String, i);
		Out += UTextTypes::TransliterateCharacter(FromAlphabet, ToAlphabet, CharacterIterator, bOnlyUseOneCase, bOnlyUseLowerCase);
	}


	return Out;
}


FString UTextTypes::GothifyCharacter(FString const& Character)
{
	if (UTextTypes::c_Alphabet_English.ContainsCharacter(Character))
	{
		const bool bUseLowercase = UTextTypes::c_Alphabet_English.IsContainedCharacterLowercase(Character);
		const int Index = UTextTypes::c_Alphabet_English.GetIndexOfCharacter(Character);
		return UTextTypes::c_Alphabet_Gothic.Characters[Index].GetCase(bUseLowercase);
	}


	return Character;
}


FString UTextTypes::GothifyString(FString const& String)
{
	const FString LowercaseString = String.ToLower();
	FString Out;
	FOR_FORWARD(i, LowercaseString.Len())
	{
		const FString CharacterIterator = UTextTypes::GetStringifiedCharacter(LowercaseString, i);
		if (UTextTypes::c_Alphabet_English.ContainsCharacter(CharacterIterator))
		{
			Out += UTextTypes::GothifyCharacter(CharacterIterator);
		}
		else
		{
			Out += CharacterIterator;
		}
	}


	return Out;
}