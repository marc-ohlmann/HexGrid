// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimeTypes.generated.h"


UENUM(BlueprintType)
enum class ETimeOfDayType : uint8
{
	// 12am to 1am
	midnight,

	// 1am to 3am
	late_night,

	// 3am to 5am 
	very_early_morning,

	// 5am to 7am
	early_morning,

	// sun rise
	// 7am to 8am
	dawn,

	// 8am to 10pm
	morning,

	// 10am to 12pm
	late_morning,

	// 12pm to 1pm
	noon,

	// 1pm to 3pm
	early_afternoon,

	// 3pm to 5pm
	afternoon,

	// 5pm to 7pm
	late_afternoon,

	// sunset
	// 7pm to 8pm
	dusk,
	
	// 8pm to 10pm
	evening,

	// 10pm to 12am
	late_evening,
};


// formats to display the time
UENUM(BlueprintType)
enum class EClockType : uint8
{
	// 12 hour clock
	meridiem,

	// 24 hour clock
	military,
};


// meridiem tags used for 12hr clocks
UENUM(BlueprintType)
enum class EMeridiemType : uint8
{
	// ante meridiem
	am,

	// post meridiem
	pm,
};


// display types that are used by standard digital clocks
UENUM(BlueprintType)
enum class EClockDisplayType : uint8
{
	// full time display
	hours_minutes_seconds,

	// large time display
	hours_minutes,

	// small time display
	minutes_seconds,
	
	// seconds time display
	seconds,
	
	// minutes time display
	minutes,
	
	// hours time display
	hours,
};


// struct representing the current time of day
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FTimeOfDay
{
	GENERATED_BODY()

public:

	FTimeOfDay()
		: TotalSeconds(0)
	{
	}

	FTimeOfDay(const int InTotalSeconds);
	
	static FTimeOfDay FromTimeOfDayType(const ETimeOfDayType TimeOfDayType);
	static FTimeOfDay FromMeridiemClockTime(const EMeridiemType MeridiemType, int MeridiemClockHours, int ClockMinutes = 0, int ClockSeconds = 0);
	static FTimeOfDay FromMilitaryClockTime(int MilitaryClockHours, int ClockMinutes = 0, int ClockSeconds = 0);

	static FTimeOfDay FromDateTime(FDateTime const& InDateTime);
	static FTimeOfDay Now();
	static FTimeOfDay UtcNow();

	// total seconds that have passed within the current time of day
	UPROPERTY()
		int TotalSeconds;

	// check if the total seconds is within range of a single day
	bool IsValid() const;

	// get the total seconds that have passed in the current time of day
	int GetTotalSeconds() const { return TotalSeconds; }

	// check if the current time of day is equal to the given time
	bool EqualTo(FTimeOfDay const& Other) const
	{
		return (*this).GetTotalSeconds() == Other.GetTotalSeconds();
	}

	bool operator==(FTimeOfDay const& Other) const
	{
		return (*this).EqualTo(Other);
	}

	bool operator!=(FTimeOfDay const& Other) const
	{
		return !(*this).EqualTo(Other);
	}

	// check if the current time of day is later in the day than the given time
	bool LaterThan(FTimeOfDay const& Other) const
	{
		return (*this).GetTotalSeconds() > Other.GetTotalSeconds();
	}

	bool operator>(FTimeOfDay const& Other) const
	{
		return (*this).LaterThan(Other);
	}

	bool operator>=(FTimeOfDay const& Other) const
	{
		return (*this).LaterThan(Other) || (*this).EqualTo(Other);
	}

	// check if the current time of day is earlier in the day than the given time
	bool EarlierThan(FTimeOfDay const& Other) const
	{
		return (*this).GetTotalSeconds() < Other.GetTotalSeconds();
	}

	bool operator<(FTimeOfDay const& Other) const
	{
		return (*this).EarlierThan(Other);
	}

	bool operator<=(FTimeOfDay const& Other) const
	{
		return (*this).EarlierThan(Other) || (*this).EqualTo(Other);
	}

	// get the total number of minutes that have occured for this time of day
	int GetTotalMinutes() const;

	// get the total number of hours that have occured for this time of day
	// equivalent to clock hours in military time
	int GetTotalHours() const;

	// get the current second for the current minute within the current hour on the clock
	// always returns a value between 0 and 59
	int GetClockSeconds() const;

	// get the current minute for the current hour on the clock
	// always returns a value between 0 and 59
	int GetClockMinutes() const;

	// get the current hour for the current time of day
	// for meridiem time: always returns a value between 1 and 12
	// for military time: always returns a value between 0 and 23
	int GetClockHours(const EClockType ClockType = EClockType::meridiem) const;

	// get the meridiem type of the current time of day (am / pm)
	EMeridiemType GetMeridiemType() const;

	// get the type for the current time of day
	ETimeOfDayType GetTimeOfDayType() const;

	// get a value between zero and one representing the fraction of the day that has progressed
	float GetProgressThroughDay() const;

	// convert to neatly formatted text
	FText ToText(const EClockType ClockType = EClockType::meridiem, bool bHideSeconds = true) const;

	// convert to a neatly formatted string
	FString ToString(const EClockType ClockType = EClockType::meridiem, bool bHideSeconds = true) const
	{
		return ToText(ClockType, bHideSeconds).ToString();
	}

	// adds seconds to increment the time of day
	FTimeOfDay& CycleTimeOfDay(const int InCycleSeconds);

	// adds seconds to increment the time of day
	FTimeOfDay& operator+=(const int InCycleSeconds)
	{
		return CycleTimeOfDay(InCycleSeconds);
	}

	// adds seconds to increment the time of day
	FTimeOfDay& operator-=(const int InCycleSeconds)
	{
		return CycleTimeOfDay(-1 * InCycleSeconds);
	}

	// change to a new time of day that is exactly half a day away from this
	FTimeOfDay& MakeOppositeTime();

	// make a new time of day that is exactly half a day away from this
	static FTimeOfDay OppositeToTimeOfDay(FTimeOfDay const& InTimeOfDay)
	{
		FTimeOfDay Out = FTimeOfDay(InTimeOfDay);
		return Out.MakeOppositeTime();
	}

	// make a new time of day that is exactly half a day away from this
	static FTimeOfDay OppositeToTimeOfDay(const ETimeOfDayType InTimeOfDayType)
	{
		FTimeOfDay Out = FTimeOfDay::FromTimeOfDayType(InTimeOfDayType);
		return Out.MakeOppositeTime();
	}
	 
	// get the time of day type that corresponds with the opposite of this time of day's type
	ETimeOfDayType GetOppositeTimeOfDayType(ETimeOfDayType const InTimeOfDayType) const
	{
		FTimeOfDay Now = OppositeToTimeOfDay(*this);
		return Now.GetTimeOfDayType();
	}

};


// for precise measuring of time between function calls
struct FPrecisionStopwatch
{
	void Start()
	{
		StartTime = FPlatformTime::Cycles64();
	}
	void Stop()
	{
		EndTime = FPlatformTime::Cycles64();
	}
	double GetElapsedTimeSeconds() const
	{
		return FPlatformTime::ToSeconds64(EndTime - StartTime);
	}
	double GetElapsedTimeMilliseconds() const
	{
		return FPlatformTime::ToMilliseconds64(EndTime - StartTime);
	}
	FString GetPrettyElapsedTimeSeconds() const
	{
		return FPlatformTime::PrettyTime(GetElapsedTimeSeconds());
	}
private:
	uint64 StartTime;
	uint64 EndTime;
};


/**
 * Functions and constants for time types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UTimeTypes : public UObject
{
	GENERATED_BODY()

public:

	static const int c_HoursPerHalfDay;

	static const int c_ClockHoursMin_Military;
	static const int c_ClockHoursMax_Military;

	static const int c_ClockHoursMax_Meridiem;
	static const int c_ClockHoursMin_Meridiem;

	static const int c_ClockMinutesMax;
	static const int c_ClockMinutesMin;
	static const int c_ClockSecondsMax;
	static const int c_ClockSecondsMin;

	static const int c_TimeOfDayMeridiemHour_Midnight;
	static const int c_TimeOfDayMeridiemHour_LateNight;
	static const int c_TimeOfDayMeridiemHour_VeryEarlyMorning;
	static const int c_TimeOfDayMeridiemHour_EarlyMorning;
	static const int c_TimeOfDayMeridiemHour_Dawn;
	static const int c_TimeOfDayMeridiemHour_Morning;
	static const int c_TimeOfDayMeridiemHour_LateMorning;
	static const int c_TimeOfDayMeridiemHour_Noon;
	static const int c_TimeOfDayMeridiemHour_EarlyAfternoon;
	static const int c_TimeOfDayMeridiemHour_Afternoon;
	static const int c_TimeOfDayMeridiemHour_LateAfternoon;
	static const int c_TimeOfDayMeridiemHour_Dusk;
	static const int c_TimeOfDayMeridiemHour_Evening;
	static const int c_TimeOfDayMeridiemHour_LateEvening;

	static const float c_SecondsPerMinuteRatio;
	static const float c_SecondsPerHourRatio;
	static const float c_SecondsPerDayRatio;
	static const float c_SecondsPerWeekRatio;
	static const float c_SecondsPerMonthRatio;
	static const float c_SecondsPerSeasonRatio;
	static const float c_SecondsPerYearRatio;

	static const float c_MinutesPerSecondRatio;
	static const float c_MinutesPerHourRatio;
	static const float c_MinutesPerDayRatio;
	static const float c_MinutesPerWeekRatio;
	static const float c_MinutesPerMonthRatio;
	static const float c_MinutesPerSeasonRatio;
	static const float c_MinutesPerYearRatio;

	static const float c_HoursPerSecondRatio;
	static const float c_HoursPerMinuteRatio;
	static const float c_HoursPerDayRatio;
	static const float c_HoursPerWeekRatio;
	static const float c_HoursPerMonthRatio;
	static const float c_HoursPerSeasonRatio;
	static const float c_HoursPerYearRatio;

	static const float c_DaysPerSecondRatio;
	static const float c_DaysPerMinuteRatio;
	static const float c_DaysPerHourRatio;
	static const float c_DaysPerWeekRatio;
	static const float c_DaysPerMonthRatio;
	static const float c_DaysPerSeasonRatio;
	static const float c_DaysPerYearRatio;

	static const float c_WeeksPerSecondRatio;
	static const float c_WeeksPerMinuteRatio;
	static const float c_WeeksPerHourRatio;
	static const float c_WeeksPerDayRatio;
	static const float c_WeeksPerMonthRatio;
	static const float c_WeeksPerSeasonRatio;
	static const float c_WeeksPerYearRatio;

	static const float c_MonthsPerSecondRatio;
	static const float c_MonthsPerMinuteRatio;
	static const float c_MonthsPerHourRatio;
	static const float c_MonthsPerDayRatio;
	static const float c_MonthsPerWeekRatio;
	static const float c_MonthsPerYearRatio;
	static const float c_MonthsPerSeasonRatio;

	static const float c_SeasonsPerSecondRatio;
	static const float c_SeasonsPerMinuteRatio;
	static const float c_SeasonsPerHourRatio;
	static const float c_SeasonsPerDayRatio;
	static const float c_SeasonsPerWeekRatio;
	static const float c_SeasonsPerMonthRatio;
	static const float c_SeasonsPerYearRatio;

	static const float c_YearsPerSecondRatio;
	static const float c_YearsPerMinuteRatio;
	static const float c_YearsPerHourRatio;
	static const float c_YearsPerDayRatio;
	static const float c_YearsPerWeekRatio;
	static const float c_YearsPerMonthRatio;
	static const float c_YearsPerSeasonRatio;

	// get meridiem type as text
	static FText GetMeridianTypeText(const EMeridiemType Type);

	// get number formatting options used for standard 2-digit sectioned digital clocks
	static const FNumberFormattingOptions GetNumberFormattingOptions_ClockTime();

	// check if the given second index is valid
	static bool IsValidClockSecond(const int ClockSeconds);

	// check if the given minute index is valid
	static bool IsValidClockMinute(const int ClockMinutes);

	// check if the given hour index is valid for the given clock type
	static bool IsValidClockHour(const EClockType ClockType, const int ClockHours);

	// convert hour for meridiem time to a military time hour
	static int ConvertMeridiemToMilitaryHours(const EMeridiemType MeridiemType, const int MeridiemHours);

	// convert hour for military time to a meridiem time hour (does not provide meridiem type)
	static int ConvertMilitaryToMeridiemHours(const int MilitaryHours);

	// get the meridiem hour type for the given hour in military time
	static EMeridiemType GetMeridiemHourTypeFromMilitaryHours(const int MilitaryHours);
	
	// format the given seconds value as text similar to a standard digital clock
	// optionally clamping the minutes and hours values to clock maximums
	static FText FormatSecondsAsClockTimeText(
		const EClockDisplayType DisplayType,
		const float Seconds, 
		bool bClampMinutes = true,
		bool bClampHours = true,
		const EClockType ClockType = EClockType::meridiem
	);

	// check if the given clock display type includes seconds
	static bool DoesClockDisplayTypeIncludeSeconds(const EClockDisplayType DisplayType);

	// check if the given clock display type includes minutes
	static bool DoesClockDisplayTypeIncludeMinutes(const EClockDisplayType DisplayType);

	// check if the given clock display type includes hours
	static bool DoesClockDisplayTypeIncludeHours(const EClockDisplayType DisplayType);

};
