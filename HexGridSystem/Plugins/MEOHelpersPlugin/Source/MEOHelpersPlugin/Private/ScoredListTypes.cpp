// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "ScoredListTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"


AActor* FScoredActor::GetActor() const
{
	return UMCommon::SafelyDereferenceWeakObjectPtr<AActor>(Actor);
}


int FScoredActorList::GetLowestScoringActorIndex() const
{
	int LowestScoringIndex = -1;
	float LowestScore = TNumericLimits<float>::Max();
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Score < LowestScore)
		{
			LowestScore = List[i].Score;
			LowestScoringIndex = i;
		}
	}


	return LowestScoringIndex;
}


int FScoredActorList::GetHighestScoringActorIndex() const
{
	int HighestScoringIndex = -1;
	float HighestScore = TNumericLimits<float>::Lowest();
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Score > HighestScore)
		{
			HighestScore = List[i].Score;
			HighestScoringIndex = i;
		}
	}


	return HighestScoringIndex;
}


AActor* FScoredActorList::GetLowestScoringActor() const
{
	const int LowestScoringIndex = GetLowestScoringActorIndex();
	if (LowestScoringIndex < 0 || LowestScoringIndex >= List.Num())
	{
		return nullptr;
	}


	return List[LowestScoringIndex].GetActor();
}


AActor* FScoredActorList::GetHighestScoringActor() const
{
	const int HighestScoringIndex = GetHighestScoringActorIndex();
	if (HighestScoringIndex < 0 || HighestScoringIndex >= List.Num())
	{
		return nullptr;
	}


	return List[HighestScoringIndex].GetActor();
}


float FScoredActorList::GetLowestScore() const
{
	const int LowestScoringIndex = GetLowestScoringActorIndex();
	if (LowestScoringIndex < 0 || LowestScoringIndex >= List.Num())
	{
		return 0.f;
	}


	return List[LowestScoringIndex].Score;
}


float FScoredActorList::GetHighestScore() const
{
	const int HighestScoringIndex = GetHighestScoringActorIndex();
	if (HighestScoringIndex < 0 || HighestScoringIndex >= List.Num())
	{
		return 0.f;
	}


	return List[HighestScoringIndex].Score;
}


float FScoredActorList::GetActorScore(const AActor* InActor) const
{
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Actor == InActor)
		{
			return List[i].Score;
		}
	}


	return 0.f;
}


int FScoredActorList::GetActorIndex(const AActor* InActor) const
{
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Actor == InActor)
		{
			return i;
		}
	}


	return -1;
}


bool FScoredActorList::IsIndexValid(const int Index) const
{
	return UMCommon::IsValidIndex(List, Index);
}


bool FScoredActorList::ContainsActor(const AActor* InActor) const
{
	return IsIndexValid(GetActorIndex(InActor));
}

FScoredActorList FScoredActorList::Sort(bool bLowestToHighest) const
{
	if (List.Num() <= 1)
	{
		return (*this);
	}

	FScoredActorList Out = (*this);
	if (bLowestToHighest)
	{
		Out.List.Sort([](const auto& A, const auto& B)
			{
				// (Change '.Score' to whatever the actual float/int variable is named inside your list items!)
				return A.Score < B.Score;
			});
	}
	else
	{
		Out.List.Sort([](const auto& A, const auto& B)
			{
				return A.Score > B.Score;
			});
	}


	return Out;
}


FScoredIndexList FScoredIndexList::Sort(bool bLowestToHighest) const
{
	if (List.Num() <= 1)
	{
		return (*this);
	}

	FScoredIndexList Out = (*this);
	if (bLowestToHighest)
	{
		Out.List.Sort([](const auto& A, const auto& B)
			{
				return A.Score < B.Score;
			});
	}
	else
	{
		Out.List.Sort([](const auto& A, const auto& B)
			{
				return A.Score > B.Score;
			});
	}


	return Out;
}


int FScoredIndexList::GetLowestScoringListIndex() const
{
	int LowestScoringIndex = -1;
	float LowestScore = TNumericLimits<float>::Max();
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Score < LowestScore)
		{
			LowestScore = List[i].Score;
			LowestScoringIndex = i;
		}
	}


	return LowestScoringIndex;
}


int FScoredIndexList::GetHighestScoringListIndex() const
{
	int HighestScoringIndex = -1;
	float HighestScore = TNumericLimits<float>::Lowest();
	FOR_FORWARD(i, List.Num())
	{
		if (List[i].Score > HighestScore)
		{
			HighestScore = List[i].Score;
			HighestScoringIndex = i;
		}
	}


	return HighestScoringIndex;
}