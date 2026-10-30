// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "TimeTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"


const int UTimeTypes::c_HoursPerHalfDay = (static_cast<int>(UTimeTypes::c_HoursPerDayRatio)) / 2;

const int UTimeTypes::c_ClockHoursMin_Military = 0;
const int UTimeTypes::c_ClockHoursMax_Military = 23;

const int UTimeTypes::c_ClockHoursMax_Meridiem = 12;
const int UTimeTypes::c_ClockHoursMin_Meridiem = 1;

const int UTimeTypes::c_ClockMinutesMax = 59;
const int UTimeTypes::c_ClockMinutesMin = 0;

const int UTimeTypes::c_ClockSecondsMax = UTimeTypes::c_ClockMinutesMax;
const int UTimeTypes::c_ClockSecondsMin = UTimeTypes::c_ClockMinutesMin;

const int UTimeTypes::c_TimeOfDayMeridiemHour_Midnight = 12;
const int UTimeTypes::c_TimeOfDayMeridiemHour_LateNight = 1;
const int UTimeTypes::c_TimeOfDayMeridiemHour_VeryEarlyMorning = 3;
const int UTimeTypes::c_TimeOfDayMeridiemHour_EarlyMorning = 5;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Dawn = 7;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Morning = 8;
const int UTimeTypes::c_TimeOfDayMeridiemHour_LateMorning = 10;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Noon = 12;
const int UTimeTypes::c_TimeOfDayMeridiemHour_EarlyAfternoon = 1;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Afternoon = 3;
const int UTimeTypes::c_TimeOfDayMeridiemHour_LateAfternoon = 5;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Dusk = 7;
const int UTimeTypes::c_TimeOfDayMeridiemHour_Evening = 8;
const int UTimeTypes::c_TimeOfDayMeridiemHour_LateEvening = 10;

const float UTimeTypes::c_SecondsPerMinuteRatio = 60.f;
const float UTimeTypes::c_SecondsPerHourRatio = 3600.f;
const float UTimeTypes::c_SecondsPerDayRatio = 86400.f;
const float UTimeTypes::c_SecondsPerWeekRatio = 604800.f;
const float UTimeTypes::c_SecondsPerMonthRatio = 604800.f;
const float UTimeTypes::c_SecondsPerSeasonRatio = 7884000.f;
const float UTimeTypes::c_SecondsPerYearRatio = 31536000.f;

const float UTimeTypes::c_MinutesPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerMinuteRatio;
const float UTimeTypes::c_MinutesPerHourRatio = 60.f;
const float UTimeTypes::c_MinutesPerDayRatio = 1440.f;
const float UTimeTypes::c_MinutesPerWeekRatio = 10080.f;
const float UTimeTypes::c_MinutesPerMonthRatio = 43800.f;
const float UTimeTypes::c_MinutesPerSeasonRatio = 131400.f;
const float UTimeTypes::c_MinutesPerYearRatio = 525600.f;

const float UTimeTypes::c_HoursPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerHourRatio;
const float UTimeTypes::c_HoursPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerHourRatio;
const float UTimeTypes::c_HoursPerDayRatio = 24.f;
const float UTimeTypes::c_HoursPerWeekRatio = 168.f;
const float UTimeTypes::c_HoursPerMonthRatio = 730.001f;
const float UTimeTypes::c_HoursPerSeasonRatio = 2190.f;
const float UTimeTypes::c_HoursPerYearRatio = 8760.f;

const float UTimeTypes::c_DaysPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerDayRatio;
const float UTimeTypes::c_DaysPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerDayRatio;
const float UTimeTypes::c_DaysPerHourRatio = 1.f / UTimeTypes::c_HoursPerDayRatio;
const float UTimeTypes::c_DaysPerWeekRatio = 7.f;
const float UTimeTypes::c_DaysPerMonthRatio = 30.4167f;
const float UTimeTypes::c_DaysPerSeasonRatio = 91.25f;
const float UTimeTypes::c_DaysPerYearRatio = 365.f;

const float UTimeTypes::c_WeeksPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerWeekRatio;
const float UTimeTypes::c_WeeksPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerWeekRatio;
const float UTimeTypes::c_WeeksPerHourRatio = 1.f / UTimeTypes::c_HoursPerWeekRatio;
const float UTimeTypes::c_WeeksPerDayRatio = 1.f / UTimeTypes::c_DaysPerWeekRatio;
const float UTimeTypes::c_WeeksPerMonthRatio = 4.34524f;
const float UTimeTypes::c_WeeksPerSeasonRatio = 13.035725f;
const float UTimeTypes::c_WeeksPerYearRatio = 52.1429f;

const float UTimeTypes::c_MonthsPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerMonthRatio;
const float UTimeTypes::c_MonthsPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerMonthRatio;
const float UTimeTypes::c_MonthsPerHourRatio = 1.f / UTimeTypes::c_HoursPerMonthRatio;
const float UTimeTypes::c_MonthsPerDayRatio = 1.f / UTimeTypes::c_DaysPerMonthRatio;
const float UTimeTypes::c_MonthsPerWeekRatio = 1.f / UTimeTypes::c_WeeksPerMonthRatio;
const float UTimeTypes::c_MonthsPerSeasonRatio = 3.f;
const float UTimeTypes::c_MonthsPerYearRatio = 12.f;

const float UTimeTypes::c_SeasonsPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerHourRatio = 1.f / UTimeTypes::c_HoursPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerDayRatio = 1.f / UTimeTypes::c_DaysPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerWeekRatio = 1.f / UTimeTypes::c_WeeksPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerMonthRatio = 1.f / UTimeTypes::c_MonthsPerSeasonRatio;
const float UTimeTypes::c_SeasonsPerYearRatio = 4.f;

const float UTimeTypes::c_YearsPerSecondRatio = 1.f / UTimeTypes::c_SecondsPerYearRatio;
const float UTimeTypes::c_YearsPerMinuteRatio = 1.f / UTimeTypes::c_MinutesPerYearRatio;
const float UTimeTypes::c_YearsPerHourRatio = 1.f / UTimeTypes::c_HoursPerYearRatio;
const float UTimeTypes::c_YearsPerDayRatio = 1.f / UTimeTypes::c_DaysPerYearRatio;
const float UTimeTypes::c_YearsPerWeekRatio = 1.f / UTimeTypes::c_WeeksPerYearRatio;
const float UTimeTypes::c_YearsPerMonthRatio = 1.f / UTimeTypes::c_MonthsPerYearRatio;
const float UTimeTypes::c_YearsPerSeasonRatio = 1.f / UTimeTypes::c_SeasonsPerYearRatio;


FTimeOfDay::FTimeOfDay(const int InTotalSeconds)
	: FTimeOfDay()
{
	TotalSeconds = InTotalSeconds;

	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		TotalSeconds = 0;
	}
}


FTimeOfDay FTimeOfDay::FromTimeOfDayType(const ETimeOfDayType TimeOfDayType)
{
	switch (TimeOfDayType)
	{

	case ETimeOfDayType::midnight:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_Midnight);

	case ETimeOfDayType::late_night:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_LateNight);

	case ETimeOfDayType::very_early_morning:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_VeryEarlyMorning);

	case ETimeOfDayType::early_morning:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_EarlyMorning);

	case ETimeOfDayType::dawn:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_Dawn);

	case ETimeOfDayType::morning:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_Morning);

	case ETimeOfDayType::late_morning:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::am, UTimeTypes::c_TimeOfDayMeridiemHour_LateMorning);

	case ETimeOfDayType::noon:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_Noon);

	case ETimeOfDayType::early_afternoon:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_EarlyAfternoon);

	case ETimeOfDayType::afternoon:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_Afternoon);

	case ETimeOfDayType::late_afternoon:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_LateAfternoon);

	case ETimeOfDayType::dusk:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_Dusk);

	case ETimeOfDayType::evening:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_Evening);

	case ETimeOfDayType::late_evening:
		return FTimeOfDay::FromMeridiemClockTime(EMeridiemType::pm, UTimeTypes::c_TimeOfDayMeridiemHour_LateEvening);


	default:
		L_ERR(MEOHelpers, "default on time of day type '{0}'", TimeOfDayType);
		break;
	}


	return FTimeOfDay();
}


FTimeOfDay FTimeOfDay::FromMeridiemClockTime(const EMeridiemType MeridiemType, int MeridiemClockHours, int ClockMinutes, int ClockSeconds)
{
	if (UTimeTypes::IsValidClockHour(EClockType::meridiem, MeridiemClockHours) == false)
	{
		L_ERR(MEOHelpers, "invalid meridiem clock hour {0}", MeridiemClockHours);
		return FTimeOfDay();
	}

	if (UTimeTypes::IsValidClockMinute(ClockMinutes) == false)
	{
		L_ERR(MEOHelpers, "invalid clock minute {0}", ClockMinutes);
		return FTimeOfDay();
	}

	if (UTimeTypes::IsValidClockSecond(ClockSeconds) == false)
	{
		L_ERR(MEOHelpers, "invalid clock second {0}", ClockSeconds);
		return FTimeOfDay();
	}

	// convert the hour to military time
	MeridiemClockHours = UTimeTypes::ConvertMeridiemToMilitaryHours(MeridiemType, MeridiemClockHours);


	return FTimeOfDay::FromMilitaryClockTime(MeridiemClockHours, ClockMinutes, ClockSeconds);
}


FTimeOfDay FTimeOfDay::FromMilitaryClockTime(int MilitaryClockHours, int ClockMinutes, int ClockSeconds)
{
	if (UTimeTypes::IsValidClockHour(EClockType::military, MilitaryClockHours) == false)
	{
		L_ERR(MEOHelpers, "invalid military clock hour {0}", MilitaryClockHours);
		return FTimeOfDay();
	}

	if (UTimeTypes::IsValidClockMinute(ClockMinutes) == false)
	{
		L_ERR(MEOHelpers, "invalid clock minute {0}", ClockMinutes);
		return FTimeOfDay();
	}

	if (UTimeTypes::IsValidClockSecond(ClockSeconds) == false)
	{
		L_ERR(MEOHelpers, "invalid clock second {0}", ClockSeconds);
		return FTimeOfDay();
	}


	return FTimeOfDay(
		ClockSeconds +
		(((static_cast<int>(UTimeTypes::c_SecondsPerMinuteRatio))) * ClockMinutes) +
		(((static_cast<int>(UTimeTypes::c_SecondsPerHourRatio))) * MilitaryClockHours)
	);
}


FTimeOfDay FTimeOfDay::FromDateTime(FDateTime const& InDateTime)
{
	return FTimeOfDay::FromMilitaryClockTime(InDateTime.GetHour(), InDateTime.GetMinute(), InDateTime.GetSecond());
}


FTimeOfDay FTimeOfDay::Now()
{
	return FTimeOfDay::FromDateTime(FDateTime::Now());
}


FTimeOfDay FTimeOfDay::UtcNow()
{
	return FTimeOfDay::FromDateTime(FDateTime::UtcNow());
}


bool FTimeOfDay::IsValid() const
{
	const int Seconds = GetTotalSeconds();


	return Seconds >= 0 && Seconds < static_cast<int>(UTimeTypes::c_SecondsPerDayRatio);
}


int FTimeOfDay::GetTotalMinutes() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0;
	}


	return GetTotalSeconds() / (static_cast<int>(UTimeTypes::c_SecondsPerMinuteRatio));
}


int FTimeOfDay::GetTotalHours() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0;
	}


	return GetTotalSeconds() / (static_cast<int>(UTimeTypes::c_SecondsPerHourRatio));
}


int FTimeOfDay::GetClockSeconds() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0;
	}


	return GetTotalSeconds() % (static_cast<int>(UTimeTypes::c_SecondsPerMinuteRatio));
}


int FTimeOfDay::GetClockMinutes() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0;
	}


	return GetTotalMinutes() % (static_cast<int>(UTimeTypes::c_MinutesPerHourRatio));
}


int FTimeOfDay::GetClockHours(const EClockType ClockType) const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0;
	}

	switch (ClockType)
	{

	case EClockType::military:
		return GetTotalHours();

	case EClockType::meridiem:
		return UTimeTypes::ConvertMilitaryToMeridiemHours(GetTotalHours());

	default:
		L_ERR(MEOHelpers, "default on clock type '{0}'", ClockType);
		break;
	}


	return 0;
}


EMeridiemType FTimeOfDay::GetMeridiemType() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return EMeridiemType::am;
	}


	return UTimeTypes::GetMeridiemHourTypeFromMilitaryHours(GetTotalHours());
}


ETimeOfDayType FTimeOfDay::GetTimeOfDayType() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return ETimeOfDayType::midnight;
	}

	// find the time of day type in which the current time falls within range
	TArray<ETimeOfDayType> TimeOfDayTypes = UMCommon::GetAllValuesForEnum<ETimeOfDayType>();
	FOR_FORWARD(i, TimeOfDayTypes.Num() - 1)
	{
		if ((*this) >= FTimeOfDay::FromTimeOfDayType(TimeOfDayTypes[i]) && 
			(*this) < FTimeOfDay::FromTimeOfDayType(TimeOfDayTypes[i + 1]))
		{
			return TimeOfDayTypes[i];
		}
	}


	// current time doesnt fall within any range
	// must be the last part of the day
	return TimeOfDayTypes[TimeOfDayTypes.Num() - 1];
}


float FTimeOfDay::GetProgressThroughDay() const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return 0.f;
	}


	return (static_cast<float>(GetTotalSeconds())) / UTimeTypes::c_SecondsPerDayRatio;
}


FText FTimeOfDay::ToText(const EClockType ClockType, bool bHideSeconds) const
{
	if (IsValid() == false)
	{
		L_ERR(MEOHelpers, "invalid time of day {seconds: '{0}'}", GetTotalSeconds());
		return FText();
	}

	const FNumberFormattingOptions ClockNumberFormatting = UTimeTypes::GetNumberFormattingOptions_ClockTime();
	const EMeridiemType MeridiemType = GetMeridiemType();
	const FText ClockTypeText = UTimeTypes::GetMeridianTypeText(MeridiemType);

	FFormatNamedArguments LocalArgs;
	LocalArgs.Add("Hours", FText::AsNumber(GetClockHours(ClockType), &ClockNumberFormatting));
	LocalArgs.Add("Minutes", FText::AsNumber(GetClockMinutes(), &ClockNumberFormatting));
	LocalArgs.Add("Type", ClockTypeText);

	switch (ClockType)
	{

	case EClockType::meridiem:
		
		if (bHideSeconds)
			return FText::Format(NSLOCTEXT("MEOHelpersPlugin", "FTimeOfDay::ToText(meridiem)", "{Hours}:{Minutes} {Type}"), LocalArgs);
		
		LocalArgs.Add("Seconds", FText::AsNumber(GetClockSeconds(), &ClockNumberFormatting));
		return FText::Format(NSLOCTEXT("MEOHelpersPlugin", "FTimeOfDay::ToText(meridiem)", "{Hours}:{Minutes}:{Seconds} {Type}"), LocalArgs);

	case EClockType::military:
		if (bHideSeconds)
			return FText::Format(NSLOCTEXT("MEOHelpersPlugin", "FTimeOfDay::ToText(military)", "{Hours}:{Minutes}"), LocalArgs);
		
		LocalArgs.Add("Seconds", FText::AsNumber(GetClockSeconds(), &ClockNumberFormatting));
		return FText::Format(NSLOCTEXT("MEOHelpersPlugin", "FTimeOfDay::ToText(military)", "{Hours}:{Minutes}:{Seconds}"), LocalArgs);

	default:
		L_ERR(MEOHelpers, "default on clock type '{0}'", ClockType);
		break;
	}


	return FText();
}


FTimeOfDay& FTimeOfDay::CycleTimeOfDay(const int InCycleSeconds)
{
	TotalSeconds += InCycleSeconds;

	// add days worth of seconds to bring the seconds back to a positive value
	if (TotalSeconds < 0)
	{
		const int DaysBelowZero = FMath::Abs(TotalSeconds / (static_cast<int>(UTimeTypes::c_SecondsPerDayRatio))) + 1;
		TotalSeconds += DaysBelowZero * (static_cast<int>(UTimeTypes::c_SecondsPerDayRatio));
	}

	// shrink to within range of one day's worth of seconds
	TotalSeconds = GetTotalSeconds() % (static_cast<int>(UTimeTypes::c_SecondsPerDayRatio));


	return (*this);
}


FText UTimeTypes::GetMeridianTypeText(const EMeridiemType Type)
{
	switch (Type)
	{
	case EMeridiemType::am:
		return NSLOCTEXT("MEOHelpersPlugin", "UTimeTypes::GetMeridianTypeText(am)", "AM");

	case EMeridiemType::pm:
		return NSLOCTEXT("MEOHelpersPlugin", "UTimeTypes::GetMeridianTypeText(military)", "PM");

	default:
		L_ERR(MEOHelpers, "default on meridiem type '{0}'", Type);
		break;
	}

	return FText();
}



const FNumberFormattingOptions UTimeTypes::GetNumberFormattingOptions_ClockTime()
{
	FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions();

	NumberFormattingOptions.SetRoundingMode(ERoundingMode::ToZero);
	NumberFormattingOptions.SetUseGrouping(false);
	NumberFormattingOptions.SetAlwaysSign(false);
	NumberFormattingOptions.SetMaximumIntegralDigits(2);
	NumberFormattingOptions.SetMinimumIntegralDigits(2);
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	NumberFormattingOptions.SetMinimumFractionalDigits(0);


	return NumberFormattingOptions;
}


int UTimeTypes::ConvertMeridiemToMilitaryHours(const EMeridiemType MeridiemType, const int MeridiemHours)
{
	if (IsValidClockHour(EClockType::meridiem, MeridiemHours) == false)
	{
		L_ERR(MEOHelpers, "invalid meridiem clock hour {0}", MeridiemHours);
		return 0;
	}

	int Hours = MeridiemHours;

	switch (MeridiemType)
	{

	case EMeridiemType::am:
		if (MeridiemHours == UTimeTypes::c_HoursPerHalfDay)
		{
			Hours = 0;
		}
		break;

	case EMeridiemType::pm:
		if (MeridiemHours != UTimeTypes::c_HoursPerHalfDay)
		{
			Hours += UTimeTypes::c_HoursPerHalfDay;
		}
		break;

	default:
		L_ERR(MEOHelpers, "default on meridiem type '{0}'", MeridiemType);
		break;
	}

	return Hours;
}


int UTimeTypes::ConvertMilitaryToMeridiemHours(const int MilitaryHours)
{
	if (IsValidClockHour(EClockType::military, MilitaryHours) == false)
	{
		L_ERR(MEOHelpers, "invalid hours '{0}'", MilitaryHours);
		return 0;
	}
	else if (MilitaryHours == 0) 
		return UTimeTypes::c_HoursPerHalfDay;
	else if (MilitaryHours > UTimeTypes::c_HoursPerHalfDay) 
		return MilitaryHours - UTimeTypes::c_HoursPerHalfDay;
	else 
		return MilitaryHours;
}


EMeridiemType UTimeTypes::GetMeridiemHourTypeFromMilitaryHours(const int MilitaryHours)
{
	return MilitaryHours >= UTimeTypes::c_HoursPerHalfDay ? EMeridiemType::pm : EMeridiemType::am;
}


bool UTimeTypes::IsValidClockSecond(const int ClockSeconds)
{
	return ClockSeconds <= UTimeTypes::c_ClockSecondsMax && ClockSeconds >= UTimeTypes::c_ClockSecondsMin;
}


bool UTimeTypes::IsValidClockMinute(const int ClockMinutes)
{
	return ClockMinutes <= UTimeTypes::c_ClockMinutesMax && ClockMinutes >= UTimeTypes::c_ClockMinutesMin;
}


bool UTimeTypes::IsValidClockHour(const EClockType ClockType, const int ClockHours)
{
	switch (ClockType)
	{

	case EClockType::meridiem:
		return ClockHours <= UTimeTypes::c_ClockHoursMax_Meridiem && ClockHours >= UTimeTypes::c_ClockHoursMin_Meridiem;

	case EClockType::military:
		return ClockHours <= UTimeTypes::c_ClockHoursMax_Military && ClockHours >= UTimeTypes::c_ClockHoursMin_Military;

	default:
		L_ERR(MEOHelpers, "default on clock type '{0}'", ClockType);
		break;
	}


	return false;
}


bool UTimeTypes::DoesClockDisplayTypeIncludeSeconds(const EClockDisplayType DisplayType)
{
	switch (DisplayType)
	{

	case EClockDisplayType::hours_minutes_seconds:
	case EClockDisplayType::minutes_seconds:
	case EClockDisplayType::seconds:
		return true;

	case EClockDisplayType::hours_minutes:
		return false;

	default:
		L_ERR(MEOHelpers, "default on clock display type '{0}'", DisplayType);
		break;
	}

	return false;
}


bool UTimeTypes::DoesClockDisplayTypeIncludeMinutes(const EClockDisplayType DisplayType)
{
	switch (DisplayType)
	{

	case EClockDisplayType::hours_minutes_seconds:
	case EClockDisplayType::minutes_seconds:
	case EClockDisplayType::hours_minutes:
		return true;

	case EClockDisplayType::seconds:
		return false;

	default:
		L_ERR(MEOHelpers, "default on clock display type '{0}'", DisplayType);
		break;
	}

	return false;
}


bool UTimeTypes::DoesClockDisplayTypeIncludeHours(const EClockDisplayType DisplayType)
{
	switch (DisplayType)
	{

	case EClockDisplayType::hours_minutes_seconds:
	case EClockDisplayType::hours_minutes:
		return true;

	case EClockDisplayType::minutes_seconds:
	case EClockDisplayType::seconds:
		return false;

	default:
		L_ERR(MEOHelpers, "default on clock display type '{0}'", DisplayType);
		break;
	}

	return false;
}


FTimeOfDay& FTimeOfDay::MakeOppositeTime()
{
	return CycleTimeOfDay(UTimeTypes::c_SecondsPerDayRatio * UMathTypes::c_Half);
}


FText UTimeTypes::FormatSecondsAsClockTimeText(
	const EClockDisplayType DisplayType,
	const float Seconds,
	bool bClampMinutes,
	bool bClampHours,
	const EClockType ClockType
)
{
	const FNumberFormattingOptions NumberFormatting = UTimeTypes::GetNumberFormattingOptions_ClockTime();
	FFormatNamedArguments LocalArgs;


	// seconds
	const int TotalSeconds = FMath::FloorToInt(Seconds);
	int DisplaySeconds = 0;
	if(UTimeTypes::DoesClockDisplayTypeIncludeSeconds(DisplayType))
	{
		DisplaySeconds = TotalSeconds % static_cast<int>(UTimeTypes::c_SecondsPerMinuteRatio);
	}

	const FText SecondsText = FText::AsNumber(DisplaySeconds, &NumberFormatting);
	LocalArgs.Add("Seconds", SecondsText);

	if (UTimeTypes::DoesClockDisplayTypeIncludeMinutes(DisplayType) == false &&
		UTimeTypes::DoesClockDisplayTypeIncludeHours(DisplayType) == false)
	{
		// can return early
		return SecondsText;
	}


	// minutes
	int DisplayMinutes = 0;
	if (UTimeTypes::DoesClockDisplayTypeIncludeMinutes(DisplayType))
	{
		const int TotalMinutes = TotalSeconds / static_cast<int>(UTimeTypes::c_SecondsPerMinuteRatio);
		if (bClampMinutes)
		{
			DisplayMinutes = TotalMinutes % static_cast<int>(UTimeTypes::c_MinutesPerHourRatio);
		}
		else
		{
			DisplayMinutes = TotalMinutes;
		}
	}

	const FText MinutesText = FText::AsNumber(DisplayMinutes, &NumberFormatting);
	LocalArgs.Add("Minutes", MinutesText);

	if (UTimeTypes::DoesClockDisplayTypeIncludeSeconds(DisplayType) == false &&
		UTimeTypes::DoesClockDisplayTypeIncludeHours(DisplayType) == false)
	{
		// can return early
		return MinutesText;
	}


	// hours
	int DisplayHours = 0;
	if (UTimeTypes::DoesClockDisplayTypeIncludeHours(DisplayType))
	{
		const int TotalHours = TotalSeconds / static_cast<int>(UTimeTypes::c_SecondsPerHourRatio);
		if (bClampHours)
		{
			switch (ClockType)
			{

			case EClockType::meridiem:
				DisplayHours = TotalHours % (UTimeTypes::c_ClockHoursMax_Meridiem + 1);
				break;
			case EClockType::military:
				DisplayHours = TotalHours % (UTimeTypes::c_ClockHoursMax_Military + 1);
				break;

			default:
				L_ERR(MEOHelpers, "default on clock type '{0}'", ClockType);
				break;
			}
		}
		else
		{
			DisplayHours = TotalHours;
		}
	}

	const FText HoursText = FText::AsNumber(DisplayHours, &NumberFormatting);
	LocalArgs.Add("Hours", HoursText);

	if (UTimeTypes::DoesClockDisplayTypeIncludeSeconds(DisplayType) == false &&
		UTimeTypes::DoesClockDisplayTypeIncludeMinutes(DisplayType) == false)
	{
		// can return early
		return HoursText;
	}


	// format text
	FText ClockText;
	switch (DisplayType)
	{

	case EClockDisplayType::hours_minutes_seconds:
		ClockText = FText::Format(NSLOCTEXT("MEOHelpersPlugin", "UTimeTypes::FormatSecondsAsClockTimeText(hours_minutes_seconds)", "{Hours}:{Minutes}:{Seconds}"), LocalArgs);
		break;

	case EClockDisplayType::hours_minutes:
		ClockText = FText::Format(NSLOCTEXT("MEOHelpersPlugin", "UTimeTypes::FormatSecondsAsClockTimeText(hours_minutes)", "{Hours}:{Minutes}"), LocalArgs);
		break;

	case EClockDisplayType::minutes_seconds:
		ClockText = FText::Format(NSLOCTEXT("MEOHelpersPlugin", "UTimeTypes::FormatSecondsAsClockTimeText(minutes_seconds)", "{Minutes}:{Seconds}"), LocalArgs);
		break;

	case EClockDisplayType::seconds:
	case EClockDisplayType::minutes:
	case EClockDisplayType::hours:
		L_ERR(MEOHelpers, "unreachable");
		break;

	default:
		L_ERR(MEOHelpers, "default on clock display type '{0}'", DisplayType);
		break;
	}


	return ClockText;
}