// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScoredListTypes.generated.h"



// a weak actor reference associated with a score
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FScoredActor
{
	GENERATED_BODY()

public:

	FScoredActor()
		: Actor(nullptr)
		, Score(0.f)
	{
	}

	FScoredActor(AActor* InActor)
		: Actor(InActor)
		, Score(0.f)
	{
	}

	FScoredActor(const float InScore)
		: Actor(nullptr)
		, Score(InScore)
	{
	}

	FScoredActor(AActor* InActor, const float InScore)
		: Actor(InActor)
		, Score(InScore)
	{
	}

	// reference to the actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TWeakObjectPtr<AActor> Actor;

	AActor* GetActor() const;

	template <class T>
	T* GetActor() const
	{
		return Cast<T>(GetActor());
	}

	// the actor's score
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Score;

};


// a list of scored actors
// can be sorted by score, or used to fetch the actor with the highest / lowest
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FScoredActorList
{
	GENERATED_BODY()

public:

	FScoredActorList()
		: List()
	{
	}

	FScoredActorList(const float InScore)
		: FScoredActorList()
	{
		List.Add(FScoredActor(InScore));
	}

	FScoredActorList(AActor* InActor)
		: FScoredActorList()
	{
		List.Add(FScoredActor(InActor));
	}

	FScoredActorList(AActor* InActor, const float InScore)
		: FScoredActorList()
	{
		List.Add(FScoredActor(InActor, InScore));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScoredActor> List;

	int GetHighestScoringActorIndex() const;
	AActor* GetHighestScoringActor() const;
	float GetHighestScore() const;

	int GetLowestScoringActorIndex() const;
	AActor* GetLowestScoringActor() const;
	float GetLowestScore() const;

	// creates a copy of the list that is sorted by the score of each entry
	FScoredActorList Sort(bool bLowestToHighest = true) const;

	int GetActorIndex(const AActor* Actor) const;
	bool IsIndexValid(const int Index) const;
	bool ContainsActor(const AActor* Actor) const;
	float GetActorScore(const AActor* Actor) const;

};


// a weak actor reference associated with a score
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FScoredIndex
{
	GENERATED_BODY()

public:

	FScoredIndex()
		: Index(-1)
		, Score(0.f)
	{
	}

	FScoredIndex(const int InIndex)
		: Index(InIndex)
		, Score(0.f)
	{
	}

	FScoredIndex(const int InIndex, const float InScore)
		: Index(InIndex)
		, Score(InScore)
	{
	}

	// index value associated with a score
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Index;

	// the index's score
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Score;

};


// a list of scored indices
// can be sorted by score
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FScoredIndexList
{
	GENERATED_BODY()

public:

	FScoredIndexList()
		: List()
	{
	}

	FScoredIndexList(const int InIndex)
		: FScoredIndexList()
	{
		List.Add(FScoredIndex(InIndex));
	}

	FScoredIndexList(const int InIndex, const float InScore)
		: FScoredIndexList()
	{
		List.Add(FScoredIndex(InIndex, InScore));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScoredIndex> List;

	// creates a copy of the list that is sorted by the score of each entry
	FScoredIndexList Sort(bool bLowestToHighest = true) const;

	// gets the index of the list entry that has the highest score
	// does NOT fetch the index property from FScoredIndex
	int GetHighestScoringListIndex() const;

	// gets the index of the list entry that has the lowest score
	// does NOT fetch the index property from FScoredIndex
	int GetLowestScoringListIndex() const;

};