// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h" 	
#include "Subsystems/WorldSubsystem.h"
#include "DiplomacyTypes.h"
#include "WorldDiplomacySubsystem.generated.h"


class AController;
class AAIController;
class APlayerController;
class APlayerState;
class AGameStateBase;


/**
 * an actor component for a game state that contains game diplomacies
 * and provides related helper functions
 * 
 */
UCLASS()
class MEOHELPERSPLUGIN_API UWorldDiplomacySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UWorldDiplomacySubsystem();

	// USubsystem

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//

public:

	// getter for the subsystem
	static UWorldDiplomacySubsystem* Get(const UObject* WorldContextObject);

	// get data on all of the game diplomacies this frame
	static FGameDiplomacies GetGameDiplomacies(const UObject* WorldContextObject);

	// register the given player tag
	void RegisterPlayer(FPlayerTag const& NewPlayer);

	// get a list of all registered player tags
	TArray<FPlayerTag> GetActivePlayerTags() const;

	// get the diplomacy from the given perspective player towards a target player
	void SetPlayerStandingTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes Standing);

	// get the diplomacy from the given perspective player towards a target player
	static void SetPlayerStandingTowardsPlayer(const UObject* WorldContextObjectconst, FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes Standing);

	// get the diplomacy from the given perspective actor towards a target actor
	static EDiplomacyTypes GetActorStandingTowardsActor(AActor* PerspectiveActor, AActor* TargetActor);

	// get the diplomacy from the given perspective actor towards a target player
	static EDiplomacyTypes GetActorStandingTowardsPlayer(AActor* PerspectiveActor, FPlayerTag const& TargetPlayer);

	// get the diplomacy from the given perspective player towards a target actor
	static EDiplomacyTypes GetPlayerStandingTowardsActor(FPlayerTag const& PerspectivePlayer, AActor* TargetActor);

	// get the diplomacy from the given perspective player towards a target player
	static EDiplomacyTypes GetPlayerStandingTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const UObject* WorldContextObject);

	// get a list of player tags that are treating a certain player with a certain diplomacy type
	// ex: get a list of players that treat player 1 as an enemy
	static TArray<FPlayerTag> GetAllPlayersWithStandingTowardsPlayer(
		const UObject* WorldContextObject,
		const EDiplomacyTypes Standing,
		FPlayerTag const& TowardsPlayer,
		bool bExcludeHumanPlayers = false,
		bool bExcludeComputerPlayers = true,
		bool bExcludeNeutralPlayers = true
	);

	// get a list of players that are being treated with a certain diplomacy type by a given player
	// ex: get a list of players that player 1 treats as an enemy
	static TArray<FPlayerTag> GetAllPlayersBeingTreatedByPlayer(
		const UObject* WorldContextObject,
		const EDiplomacyTypes TreatmentType,
		FPlayerTag const& PerspectivePlayer,
		bool bExcludeHumanPlayers = false,
		bool bExcludeComputerPlayers = true,
		bool bExcludeNeutralPlayers = true
	);

	// assign the next player tag to the given player
	// first player receives player1, second becomes player2, etc.
	// does nothing if the given player is not tagged or there are no more remaining tags
	// will attempt to set the owner tag for the player pawn and player state as well
	static bool AssignNextPlayerTag(APlayerController* Player);

	// assign the next computer tag to the given computer controller
	// first computer receives computer1, second becomes computer2, etc.
	// does nothing if the given computer is not tagged or there are no more remaining tags
	// will attempt to set the owner tag for the computer pawn and player state as well
	static bool AssignNextComputerTag(AAIController* Computer);

private:

	// map of player tags that point to other player tags, which point to a diplomacy
	UPROPERTY()
	FGameDiplomacies _GameDiplomacies;
	 
	// number of assigned player tags
	UPROPERTY()
	int _TaggedPlayerIterator;
	 
	// number of assigned computer tags
	UPROPERTY()
	int _TaggedComputerIterator;

	// get next available player tag to use
	FPlayerTag GetNextAvailablePlayerTag() const;

	// iterate next available player tag to use
	void IterateNextAvailablePlayerTag();

	// get next available computer tag to use
	FPlayerTag GetNextAvailableComputerTag() const;

	// iterate next available computer tag to use
	void IterateNextAvailableComputerTag();

};
