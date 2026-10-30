// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "WorldDiplomacySubsystem.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "PlayerTaggedActorInterface.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "AIController.h"


UWorldDiplomacySubsystem::UWorldDiplomacySubsystem()
	: Super()
	, _GameDiplomacies()
	, _TaggedPlayerIterator(0)
	, _TaggedComputerIterator(0)
{
}


void UWorldDiplomacySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	return;
}


void UWorldDiplomacySubsystem::Deinitialize()
{
	Super::Deinitialize();


	return;
}


UWorldDiplomacySubsystem* UWorldDiplomacySubsystem::Get(const UObject* WorldContextObject)
{
	return UMCommon::GetWorldSubsystem<UWorldDiplomacySubsystem>(WorldContextObject);
}


EDiplomacyTypes UWorldDiplomacySubsystem::GetActorStandingTowardsActor(AActor* PerspectiveActor, AActor* TargetActor)
{
	if (UMCommon::IsUObjectPointerSafe(PerspectiveActor) == false)
	{
		L_ERR(MEOHelpers, "target actor is null");
		return EDiplomacyTypes::neutral;
	}

	const FPlayerTag PerspectivePlayer = IPlayerTaggedActorInterface::GetActorPlayerTag(PerspectiveActor);


	return UWorldDiplomacySubsystem::GetPlayerStandingTowardsActor(PerspectivePlayer, TargetActor);
}


EDiplomacyTypes UWorldDiplomacySubsystem::GetPlayerStandingTowardsActor(FPlayerTag const& PerspectivePlayer, AActor* TargetActor)
{
	if (UMCommon::IsUObjectPointerSafe(TargetActor) == false)
	{
		L_ERR(MEOHelpers, "target actor is null");
		return EDiplomacyTypes::neutral;
	}

	

	const FPlayerTag TargetPlayer = IPlayerTaggedActorInterface::GetActorPlayerTag(TargetActor);


	return UWorldDiplomacySubsystem::GetPlayerStandingTowardsPlayer(PerspectivePlayer, TargetPlayer, TargetActor);
}


EDiplomacyTypes UWorldDiplomacySubsystem::GetActorStandingTowardsPlayer(AActor* PerspectiveActor, FPlayerTag const& TargetPlayer)
{
	if (UMCommon::IsUObjectPointerSafe(PerspectiveActor) == false)
	{
		L_ERR(MEOHelpers, "target actor is null");
		return EDiplomacyTypes::neutral;
	}

	const FPlayerTag PerspectivePlayer = IPlayerTaggedActorInterface::GetActorPlayerTag(PerspectiveActor);


	return UWorldDiplomacySubsystem::GetPlayerStandingTowardsPlayer(PerspectivePlayer, TargetPlayer, PerspectiveActor);
}



EDiplomacyTypes UWorldDiplomacySubsystem::GetPlayerStandingTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const UObject* WorldContextObject)
{
	UWorldDiplomacySubsystem* DiplomacySubsystem = UWorldDiplomacySubsystem::Get(WorldContextObject);
	if (UMCommon::IsUObjectPointerSafe(DiplomacySubsystem) == false)
	{
		L_ERR(MEOHelpers, "could not get world diplomacy subsystem");
		return EDiplomacyTypes::neutral;
	}


	return 	DiplomacySubsystem->_GameDiplomacies.GetPlayerAttitudeTowardsPlayer(PerspectivePlayer, TargetPlayer);
}


TArray<FPlayerTag> UWorldDiplomacySubsystem::GetAllPlayersWithStandingTowardsPlayer(
	const UObject* WorldContextObject,
	const EDiplomacyTypes Standing,
	FPlayerTag const& TowardsPlayer,
	bool bExcludeHumanPlayers,
	bool bExcludeComputerPlayers,
	bool bExcludeNeutralPlayers
)
{
	const FGameDiplomacies GameDiplomacies = UWorldDiplomacySubsystem::GetGameDiplomacies(WorldContextObject);
	TArray<FPlayerTag> Out;

	TArray<FPlayerTag> AllActive = GameDiplomacies.GetActivePlayerTags();

	for (const FPlayerTag& ActiveIdentity : AllActive)
	{
		if (ActiveIdentity.IsValidId() == false) continue;
		if (bExcludeHumanPlayers && ActiveIdentity.IsUserPlayer()) continue;
		if (bExcludeComputerPlayers && ActiveIdentity.IsComputerPlayer()) continue;
		if (bExcludeNeutralPlayers && ActiveIdentity.IsNeutralPlayer()) continue;

		if (GameDiplomacies.GetPlayerAttitudeTowardsPlayer(ActiveIdentity, TowardsPlayer) == Standing)
		{
			Out.Add(ActiveIdentity);
		}
	}


	return Out;
}


TArray<FPlayerTag> UWorldDiplomacySubsystem::GetAllPlayersBeingTreatedByPlayer(
	const UObject* WorldContextObject,
	const EDiplomacyTypes TreatmentType,
	FPlayerTag const& PerspectivePlayer,
	bool bExcludeHumanPlayers,
	bool bExcludeComputerPlayers,
	bool bExcludeNeutralPlayers
)
{
	const FGameDiplomacies GameDiplomacies = UWorldDiplomacySubsystem::GetGameDiplomacies(WorldContextObject);
	TArray<FPlayerTag> Out;

	TArray<FPlayerTag> AllActive = GameDiplomacies.GetActivePlayerTags();

	for (const FPlayerTag& ActiveIdentity : AllActive)
	{
		if (ActiveIdentity.IsValidId() == false) continue;
		if (bExcludeHumanPlayers && ActiveIdentity.IsUserPlayer()) continue;
		if (bExcludeComputerPlayers && ActiveIdentity.IsComputerPlayer()) continue;
		if (bExcludeNeutralPlayers && ActiveIdentity.IsNeutralPlayer()) continue;

		if (GameDiplomacies.GetPlayerAttitudeTowardsPlayer(PerspectivePlayer, ActiveIdentity) == TreatmentType)
		{
			Out.Add(ActiveIdentity);
		}
	}


	return Out;
}


FGameDiplomacies UWorldDiplomacySubsystem::GetGameDiplomacies(const UObject* WorldContextObject)
{
	UWorldDiplomacySubsystem* WorldDiplomacySubsystem = UWorldDiplomacySubsystem::Get(WorldContextObject);
	if (UMCommon::IsUObjectPointerSafe(WorldDiplomacySubsystem) == false)
	{
		L_ERR(MEOHelpers, "world diplomacy subsystem is null");
		return FGameDiplomacies();
	}


	return WorldDiplomacySubsystem->_GameDiplomacies;
}


void UWorldDiplomacySubsystem::SetPlayerStandingTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes Standing)
{
	_GameDiplomacies.SetPlayerAttitudeTowardsPlayer(PerspectivePlayer, TargetPlayer, Standing);


	return;
}


void UWorldDiplomacySubsystem::SetPlayerStandingTowardsPlayer(const UObject* WorldContextObject, FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes Standing)
{
	UWorldDiplomacySubsystem* DiplomacySubsystem = UWorldDiplomacySubsystem::Get(WorldContextObject);
	if (UMCommon::IsUObjectPointerSafe(DiplomacySubsystem) == false)
	{
		L_ERR(MEOHelpers, "could not get world diplomacy subsystem");
		return;
	}

	DiplomacySubsystem->SetPlayerStandingTowardsPlayer(PerspectivePlayer, TargetPlayer, Standing);


	return;
}


bool UWorldDiplomacySubsystem::AssignNextPlayerTag(APlayerController* Player)
{
	if (UMCommon::IsUObjectPointerSafe(Player) == false)
	{
		L_ERR(MEOHelpers, "player is null");
		return false;
	}

	UWorldDiplomacySubsystem* DiplomacySubsystem = UWorldDiplomacySubsystem::Get(Player);
	if (UMCommon::IsUObjectPointerSafe(DiplomacySubsystem) == false)
	{
		L_ERR(MEOHelpers, "diplomacy subsystem is null");
		return false;
	}

	const FPlayerTag NextPlayerTag = DiplomacySubsystem->GetNextAvailablePlayerTag();
	if (NextPlayerTag.IsValidId() == false)
	{
		L_WARN(MEOHelpers, "failed to get new player tag for player '{0}'", Player);
		return false;
	}

	if (IPlayerTaggedActorInterface::SetActorPlayerTag(Player, NextPlayerTag) == false)
	{
		L_WARN(MEOHelpers, "failed to assign player tag '{0}' to player '{1}'", NextPlayerTag, Player);
		return false;
	}

	DiplomacySubsystem->RegisterPlayer(NextPlayerTag);

	// try setting the player state
	APlayerState* PlayerState = Player->GetPlayerState<APlayerState>();
	if (UMCommon::IsUObjectPointerSafe(PlayerState) == true)
	{
		IPlayerTaggedActorInterface::SetActorPlayerTag(PlayerState, NextPlayerTag);
	}

	// try setting the pawn
	APawn* PlayerPawn = Player->GetPawn();
	if (UMCommon::IsUObjectPointerSafe(PlayerPawn) == true)
	{
		IPlayerTaggedActorInterface::SetActorPlayerTag(PlayerPawn, NextPlayerTag);
	}

	DiplomacySubsystem->IterateNextAvailablePlayerTag();


	return true;
}


bool UWorldDiplomacySubsystem::AssignNextComputerTag(AAIController* Computer)
{
	if (UMCommon::IsUObjectPointerSafe(Computer) == false)
	{
		L_ERR(MEOHelpers, "computer is null");
		return false;
	}

	UWorldDiplomacySubsystem* DiplomacySubsystem = UWorldDiplomacySubsystem::Get(Computer);
	if (UMCommon::IsUObjectPointerSafe(DiplomacySubsystem) == false)
	{
		L_ERR(MEOHelpers, "diplomacy subsystem is null");
		return false;
	}

	const FPlayerTag NextComputerTag = DiplomacySubsystem->GetNextAvailableComputerTag();
	if (NextComputerTag.IsValidId() == false)
	{
		L_WARN(MEOHelpers, "failed to get new player tag for computer '{0}'", Computer);
		return false;
	}

	DiplomacySubsystem->RegisterPlayer(NextComputerTag);

	if (IPlayerTaggedActorInterface::SetActorPlayerTag(Computer, NextComputerTag) == false)
	{
		L_WARN(MEOHelpers, "failed to assign computer tag '{0}' to computer '{1}'", NextComputerTag, Computer);
		return false;
	}


	// try setting the player state
	APlayerState* PlayerState = Computer->GetPlayerState<APlayerState>();
	if (UMCommon::IsUObjectPointerSafe(PlayerState) == true)
	{
		IPlayerTaggedActorInterface::SetActorPlayerTag(PlayerState, NextComputerTag);
	}

	// try setting the pawn
	APawn* PlayerPawn = Computer->GetPawn();
	if (UMCommon::IsUObjectPointerSafe(PlayerPawn) == true)
	{
		IPlayerTaggedActorInterface::SetActorPlayerTag(PlayerPawn, NextComputerTag);
	}

	DiplomacySubsystem->IterateNextAvailableComputerTag();


	return true;
}


FPlayerTag UWorldDiplomacySubsystem::GetNextAvailablePlayerTag() const
{
	return FPlayerTag(EPlayerTypes::user, _TaggedPlayerIterator);
}


void UWorldDiplomacySubsystem::IterateNextAvailablePlayerTag()
{ 
	_TaggedPlayerIterator += 1; 


	return;
}


FPlayerTag UWorldDiplomacySubsystem::GetNextAvailableComputerTag() const 
{
	return FPlayerTag(EPlayerTypes::computer, _TaggedComputerIterator);
}


void UWorldDiplomacySubsystem::IterateNextAvailableComputerTag()
{
	_TaggedComputerIterator += 1; 


	return;
}


void UWorldDiplomacySubsystem::RegisterPlayer(FPlayerTag const& NewPlayer)
{
	_GameDiplomacies.RegisterPlayerTag(NewPlayer);

	L_DBG(MEOHelper, "{0}", NewPlayer);


	return;
}


TArray<FPlayerTag> UWorldDiplomacySubsystem::GetActivePlayerTags() const
{
	return _GameDiplomacies.GetActivePlayerTags();
}