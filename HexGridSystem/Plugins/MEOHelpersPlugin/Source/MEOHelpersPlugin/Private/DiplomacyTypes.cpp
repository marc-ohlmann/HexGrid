// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "DiplomacyTypes.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"
#include "PlayerTaggedActorInterface.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "AIController.h"

const FPlayerTag FPlayerTag::NeutralPassive = FPlayerTag(ENeutralFactionTypes::passive);
const FPlayerTag FPlayerTag::NeutralHostile = FPlayerTag(ENeutralFactionTypes::hostile);
const FPlayerTag FPlayerTag::NeutralFriendly = FPlayerTag(ENeutralFactionTypes::friendly);
const FPlayerTag FPlayerTag::NeutralFrenemy = FPlayerTag(ENeutralFactionTypes::frenemy);
const FPlayerTag FPlayerTag::NeutralUnpopular = FPlayerTag(ENeutralFactionTypes::unpopular);
const FPlayerTag FPlayerTag::NeutralVictim = FPlayerTag(ENeutralFactionTypes::victim);


FString FPlayerTag::ToString() const
{
	if (IsValidId() == false)
	{
		return FString(TEXT("invalid_player_id"));
	}

	if (IsNeutralPlayer())
	{
		return FString::Printf(TEXT("%s::%s"), 
			*UMCommon::EnumToString<EPlayerTypes>(PlayerType),
			*UMCommon::EnumToString<ENeutralFactionTypes>(GetNeutralType())
		);
	}


	return FString::Printf(TEXT("%s::%d"), 
			*UMCommon::EnumToString<EPlayerTypes>(PlayerType),
			Id
	);
}


FText FPlayerTag::GetDisplayName() const
{
	if (IsValidId() == false)
	{
		return NSLOCTEXT("MEOHelpers", "FPlayerTag::GetDisplayName(IsInvalid)", "Invalid Player Id");
	}

	const FText TypeText = UDiplomacyTypes::GetPlayerTypeDisplayName(PlayerType);
	FText SubTypeText;
	if (IsNeutralPlayer())
	{
		SubTypeText = UDiplomacyTypes::GetNeutralTypeDisplayName(NeutralType);
	}
	else
	{
		SubTypeText = FText::AsNumber(Id);
	}


	return FText::Format(NSLOCTEXT("MEOHelpers", "FPlayerTag::GetDisplayName", "{0} {1}"), TypeText, SubTypeText);
}


void FGameDiplomacies::RegisterPlayerTag(FPlayerTag const& NewPlayerTag)
{
	if (NewPlayerTag.IsValidId() == false)
	{
		L_ERR(MEOHelpers, "invalid player tag '{0}'", NewPlayerTag);
		return;
	}

	if (PlayerStandings.Contains(NewPlayerTag))
	{
		return;
	}

	// Initialize the struct for this new identity in the map
	PlayerStandings.Add(NewPlayerTag, FPlayerDiplomacies());

	// Register family standing with self
	PlayerStandings[NewPlayerTag].StandingTowards.Add(NewPlayerTag, EDiplomacyTypes::family);

	// Evaluate and assign mutual standings against all existing players in the game
	FOR_MAP(Pair, PlayerStandings)
	{
		const FPlayerTag& ExistingIdentity = Pair.Key;

		// Skip self, since we just handled it above
		if (ExistingIdentity == NewPlayerTag)
		{
			continue;
		}

		// Resolve how the newly added player feels about the existing player
		const EDiplomacyTypes StandingTowardsExisting = GetDefaultStanding(NewPlayerTag, ExistingIdentity);
		PlayerStandings[NewPlayerTag].StandingTowards.Add(ExistingIdentity, StandingTowardsExisting);

		// Resolve how the existing player feels about the newly added player
		const EDiplomacyTypes ExistingStandingTowardsNew = GetDefaultStanding(ExistingIdentity, NewPlayerTag);
		PlayerStandings[ExistingIdentity].StandingTowards.Add(NewPlayerTag, ExistingStandingTowardsNew);
	}


	return;
}


bool FGameDiplomacies::IsPlayerTagRegisteredProperly(FPlayerTag const& PlayerTag) const
{
	// the tag must actually exist in the outer map
	const FPlayerDiplomacies* FoundPlayerDiplomacies = PlayerStandings.Find(PlayerTag);
	if (FoundPlayerDiplomacies == nullptr)
	{
		return false;
	}

	// verify mutual cross-references with every registered player (including itself)
	FOR_MAP_CONST(Pair, PlayerStandings)
	{
		const FPlayerTag& OtherTag = Pair.Key;

		// check if our target tag has a recorded standing towards the other tag
		if (PlayerStandings[PlayerTag].StandingTowards.Contains(OtherTag) == false)
		{
			return false;
		}

		// check if the other tag has a recorded standing towards our target tag
		if (PlayerStandings[OtherTag].StandingTowards.Contains(PlayerTag) == false)
		{
			return false;
		}
	}


	return true;
}


void FGameDiplomacies::ResetPlayerTag(FPlayerTag const& PlayerTag)
{
	// ensure the tag exists in the main map, creating a blank slate if it doesn't
	const FPlayerDiplomacies* FoundPlayerDiplomacies = PlayerStandings.Find(PlayerTag);
	if (FoundPlayerDiplomacies == nullptr)
	{
		PlayerStandings.Add(PlayerTag, FPlayerDiplomacies());
	}

	// iterate through all currently registered players (including itself) to forcefully reset mutual standings
	FOR_MAP(Pair, PlayerStandings)
	{
		const FPlayerTag& OtherTag = Pair.Key;

		// calculate the defaults
		const EDiplomacyTypes DefaultStandingTowardsOther = GetDefaultStanding(PlayerTag, OtherTag);
		const EDiplomacyTypes DefaultOtherStandingTowardsPlayer = GetDefaultStanding(OtherTag, PlayerTag);

		// forcefully set/overwrite how the repaired player feels about the other player
		// (TMap::Add will replace the value if the key already exists)
		PlayerStandings[PlayerTag].StandingTowards.Add(OtherTag, DefaultStandingTowardsOther);

		// forcefully set/overwrite how the other player feels about the repaired player
		PlayerStandings[OtherTag].StandingTowards.Add(PlayerTag, DefaultOtherStandingTowardsPlayer);
	}


	return;
}


void FGameDiplomacies::Print() const
{
	FOR_MAP_CONST(PerspectivePlayer, PlayerStandings)
	{
		FOR_MAP_CONST(TargetPlayer, PerspectivePlayer.Value.StandingTowards)
		{
			L_DBG(MEOHelpers, "{0} standing towards {1}: '{2}'", PerspectivePlayer.Key, TargetPlayer.Key, TargetPlayer.Value);
		}
	}


	return;
}


EDiplomacyTypes FGameDiplomacies::GetDefaultStanding(FPlayerTag const& Perspective, FPlayerTag const& Target)
{
	if (Perspective == Target) return EDiplomacyTypes::family;

	EDiplomacyTypes Standing = EDiplomacyTypes::neutral;

	// pass 1: basic neutral default states
	if (Perspective.IsNeutralFriendly()) Standing = EDiplomacyTypes::ally;
	if (Perspective.IsNeutralHostile()) Standing = EDiplomacyTypes::enemy;
	if (Perspective.IsNeutralPassive()) Standing = EDiplomacyTypes::neutral;

	// pass 2: friendly and hostile overrides
	if (Perspective.IsNeutralFriendly() && Target.IsNeutralHostile()) Standing = EDiplomacyTypes::enemy;
	if (Perspective.IsNeutralFriendly() && Target.IsNeutralPassive()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralHostile() && Target.IsNeutralPassive()) Standing = EDiplomacyTypes::neutral;

	// pass 3: advanced neutrals
	if (Target.IsNeutralVictim()) Standing = EDiplomacyTypes::enemy;
	if (Perspective.IsNeutralVictim()) Standing = EDiplomacyTypes::neutral;

	if (Target.IsNeutralUnpopular()) Standing = EDiplomacyTypes::enemy;
	if (Perspective.IsNeutralUnpopular()) Standing = EDiplomacyTypes::neutral;

	if (Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::ally;
	if (Perspective.IsNeutralFrenemy()) Standing = EDiplomacyTypes::enemy;

	// pass 4: advanced overrides between neutrals
	if (Perspective.IsNeutralUnpopular() && Target.IsNeutralVictim()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralPassive() && Target.IsNeutralVictim()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralPassive() && Target.IsNeutralUnpopular()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralVictim() && Target.IsNeutralUnpopular()) Standing = EDiplomacyTypes::neutral;

	if (Perspective.IsNeutralFrenemy() && Target.IsNeutralPassive()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralFrenemy() && Target.IsNeutralFriendly()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralFrenemy() && Target.IsNeutralHostile()) Standing = EDiplomacyTypes::neutral;

	if (Perspective.IsNeutralHostile() && Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralPassive() && Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::neutral;
	if (Perspective.IsNeutralFriendly() && Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::neutral;

	// pass 5: players and computers judging neutrals
	if (Perspective.IsUserPlayer() || Perspective.IsComputerPlayer())
	{
		if (Target.IsNeutralFriendly()) Standing = EDiplomacyTypes::ally;
		if (Target.IsNeutralHostile()) Standing = EDiplomacyTypes::enemy;
		if (Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::enemy;
		if (Target.IsNeutralUnpopular()) Standing = EDiplomacyTypes::enemy;
		if (Target.IsNeutralPassive()) Standing = EDiplomacyTypes::neutral;
		if (Target.IsNeutralVictim()) Standing = EDiplomacyTypes::neutral;
	}

	// pass 6: players judging players
	if (Perspective.IsUserPlayer() && Target.IsUserPlayer())
	{
		Standing = EDiplomacyTypes::ally;
	}

	// pass 7: custom computer overrides
	if (Perspective.IsComputerPlayer())
	{
		if (Target.IsNeutralVictim()) Standing = EDiplomacyTypes::enemy;
		if (Target.IsNeutralFrenemy()) Standing = EDiplomacyTypes::neutral;
	}
	if (Target.IsComputerPlayer() && Perspective.IsNeutralFrenemy())
	{
		Standing = EDiplomacyTypes::neutral;
	}

	return Standing;
}


EDiplomacyTypes FGameDiplomacies::GetPlayerAttitudeTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer) const
{
	if (PerspectivePlayer.IsValidId() == false)
	{
		L_ERR(MEOHelpers, "invalid perspective player id '{0}'", PerspectivePlayer);
		return EDiplomacyTypes::neutral;
	}

	if (TargetPlayer.IsValidId() == false)
	{
		L_ERR(MEOHelpers, "invalid target player id '{0}'", TargetPlayer);
		return EDiplomacyTypes::neutral;
	}

	const FPlayerDiplomacies* PerspectivePlayerDiplomacies = PlayerStandings.Find(PerspectivePlayer);
	if (PerspectivePlayerDiplomacies == nullptr)
	{
		L_ERR(MEOHelpers, "failed to find perspective player id '{0}'", PerspectivePlayer);
		return EDiplomacyTypes::neutral;
	}

	const EDiplomacyTypes* TargetPlayerDiplomacy = PerspectivePlayerDiplomacies->StandingTowards.Find(TargetPlayer);
	if (TargetPlayerDiplomacy == nullptr)
	{
		L_ERR(MEOHelpers, "failed to find target player '{0}' diplomacy from perspective player '{1}'", TargetPlayer, PerspectivePlayer);
		return EDiplomacyTypes::neutral;
	}


	return *TargetPlayerDiplomacy;
}


void FGameDiplomacies::SetPlayerAttitudeTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes NewAttitude)
{
	if (PerspectivePlayer.IsValidId() == false)
	{
		L_ERR(MEOHelpers, "invalid perspective player id '{0}'", PerspectivePlayer);
		return;
	}

	if (TargetPlayer.IsValidId() == false)
	{
		L_ERR(MEOHelpers, "invalid target player id '{0}'", TargetPlayer);
		return;
	}

	FPlayerDiplomacies* PerspectivePlayerDiplomacies = PlayerStandings.Find(PerspectivePlayer);
	if (PerspectivePlayerDiplomacies == nullptr)
	{
		L_ERR(MEOHelpers, "failed to find perspective player id '{0}'", PerspectivePlayer);
		return;
	}

	PerspectivePlayerDiplomacies->StandingTowards.Add(TargetPlayer, NewAttitude);


	return;
}


TArray<FPlayerTag> FGameDiplomacies::GetActivePlayerTags() const
{
	TArray<FPlayerTag> Out;
	PlayerStandings.GenerateKeyArray(Out);


	return Out;
}


ETeamAttitude::Type UDiplomacyTypes::ConvertDiplomacyToTeamAttitude(const EDiplomacyTypes DiplomacyType)
{
	switch (DiplomacyType)
	{


	case EDiplomacyTypes::family:
	case EDiplomacyTypes::ally:
		return ETeamAttitude::Friendly;


	case EDiplomacyTypes::enemy:
		return ETeamAttitude::Hostile;


	default:
		L_ERR(MEOHelpers, "default on diplomacy type '{0}'", DiplomacyType);
	case EDiplomacyTypes::neutral:
		return ETeamAttitude::Neutral;


	}
}


FText UDiplomacyTypes::GetDiplomacyTypeDisplayName(const EDiplomacyTypes DiplomacyType)
{
	switch (DiplomacyType)
	{

	case EDiplomacyTypes::ally:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetDiplomacyTypeDisplayName(EDiplomacyTypes::ally)", "Ally");

	case EDiplomacyTypes::enemy:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetDiplomacyTypeDisplayName(EDiplomacyTypes::enemy)", "Enemy");

	case EDiplomacyTypes::family:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetDiplomacyTypeDisplayName(EDiplomacyTypes::family)", "Family");

	case EDiplomacyTypes::neutral:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetDiplomacyTypeDisplayName(EDiplomacyTypes::neutral)", "Neutral");

	default:
		L_ERR(MEOHelpers, "default on diplomacy '{0}'", DiplomacyType);
		return FText();
	}
}


FText UDiplomacyTypes::GetPlayerTypeDisplayName(const EPlayerTypes PlayerType)
{
	switch (PlayerType)
	{

	case EPlayerTypes::user:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetPlayerTypeDisplayName(EPlayerTypes::user)", "User");

	case EPlayerTypes::computer:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetPlayerTypeDisplayName(EPlayerTypes::computer)", "Computer");

	case EPlayerTypes::neutral:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetPlayerTypeDisplayName(EPlayerTypes::neutral)", "Neutral");

	default:
		L_ERR(MEOHelpers, "default on player type '{0}'", PlayerType);
		return FText();
	}
}


FText UDiplomacyTypes::GetNeutralTypeDisplayName(const ENeutralFactionTypes NeutralType)
{
	switch (NeutralType)
	{

	case ENeutralFactionTypes::passive:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::passive)", "Passive");

	case ENeutralFactionTypes::hostile:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::hostile)", "Hostile");

	case ENeutralFactionTypes::friendly:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::friendly)", "Friendly");

	case ENeutralFactionTypes::frenemy:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::frenemy)", "Frenemy");

	case ENeutralFactionTypes::unpopular:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::unpopular)", "Unpopular");

	case ENeutralFactionTypes::victim:
		return NSLOCTEXT("MEOHelpers", "UDiplomacyTypes::GetNeutralTypeDisplayName(ENeutralFactionTypes::victim)", "Victim");

	default:
		L_ERR(MEOHelpers, "default on neutral type '{0}'", NeutralType);
		return FText();
	}
}


APlayerController* UDiplomacyTypes::GetPlayerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
{
	if (Tag.IsUserPlayer() == false)
	{
		L_ERR(MEOHelpers, "invalid player tag '{0}'", Tag);
		return nullptr;
	}

	if (UMCommon::IsUObjectPointerSafe(WorldContextObject) == false)
	{
		L_ERR(MEOHelpers, "world context is null");
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World) == false)
	{
		L_ERR(MEOHelpers, "world is null");
		return nullptr;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerControllerIterator = Iterator->Get();
		if (UMCommon::IsUObjectPointerSafe(PlayerControllerIterator) == true &&
			IPlayerTaggedActorInterface::GetActorPlayerTag(PlayerControllerIterator) == Tag)
		{
			return PlayerControllerIterator;
		}
	}
	if (World->IsNetMode(ENetMode::NM_Client))
	{
		L_WARN(MEOHelpers, "failed to find local player controller on client with tag '{0}', maybe this was supposed to be ran on the server?", Tag);
	}
	else
	{
		L_WARN(MEOHelpers, "failed to find player controller on server with tag '{0}'", Tag);
	}


	return nullptr;
}


AController* UDiplomacyTypes::GetControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
{
	if (UMCommon::IsUObjectPointerSafe(WorldContextObject) == false)
	{
		L_ERR(MEOHelpers, "world context is null");
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World) == false)
	{
		L_ERR(MEOHelpers, "world is null");
		return nullptr;
	}

	for (FConstControllerIterator Iterator = World->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* ControllerIterator = Iterator->Get();
		if (UMCommon::IsUObjectPointerSafe(ControllerIterator) == true &&
			IPlayerTaggedActorInterface::GetActorPlayerTag(ControllerIterator) == Tag)
		{
			return ControllerIterator;
		}
	}

	L_WARN(MEOHelpers, "failed to find controller with tag '{0}'", Tag);


	return nullptr;
}


AAIController* UDiplomacyTypes::GetComputerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
{
	if (Tag.IsComputerPlayer() == false)
	{
		L_ERR(MEOHelpers, "invalid computer tag '{0}'", Tag);
		return nullptr;
	}

	if (UMCommon::IsUObjectPointerSafe(WorldContextObject) == false)
	{
		L_ERR(MEOHelpers, "world context is null");
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World) == false)
	{
		L_ERR(MEOHelpers, "world is null");
		return nullptr;
	}

	for (FConstControllerIterator Iterator = World->GetControllerIterator(); Iterator; ++Iterator)
	{
		AAIController* ComputerControllerIterator = Cast<AAIController>(Iterator->Get());
		if (UMCommon::IsUObjectPointerSafe(ComputerControllerIterator) == true &&
			IPlayerTaggedActorInterface::GetActorPlayerTag(ComputerControllerIterator) == Tag)
		{
			return ComputerControllerIterator;
		}
	}

	L_WARN(MEOHelpers, "failed to find computer controller on server with tag '{0}'", Tag);


	return nullptr;
}


APlayerState* UDiplomacyTypes::GetPlayerStateFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
{
	if (Tag.IsUserPlayer() == false)
	{
		L_ERR(MEOHelpers, "invalid player tag '{0}'", Tag);
		return nullptr;
	}

	if (UMCommon::IsUObjectPointerSafe(WorldContextObject) == false)
	{
		L_ERR(MEOHelpers, "world context is null");
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World) == false)
	{
		L_ERR(MEOHelpers, "world is null");
		return nullptr;
	}

	AGameStateBase* GameState = World->GetGameState();
	if (UMCommon::IsUObjectPointerSafe(GameState) == false)
	{
		L_ERR(MEOHelpers, "game state is null");
		return nullptr;
	}

	FOR_FORWARD(i, GameState->PlayerArray.Num())
	{
		if (UMCommon::IsUObjectPointerSafe(GameState->PlayerArray[i]) == true &&
			IPlayerTaggedActorInterface::GetActorPlayerTag(GameState->PlayerArray[i]) == Tag)
		{
			return GameState->PlayerArray[i];
		}
	}

	L_WARN(MEOHelpers, "failed to find player state with tag '{0}'", Tag);


	return nullptr;
}


AController* UDiplomacyTypes::GetActorOwnerController(AActor* InActor)
{
	const FPlayerTag PlayerTag = IPlayerTaggedActorInterface::GetActorPlayerTag(InActor);


	return UDiplomacyTypes::GetControllerFromTag(InActor, PlayerTag);
}