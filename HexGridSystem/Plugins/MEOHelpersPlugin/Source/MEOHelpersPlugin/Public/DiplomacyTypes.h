// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MCommon.h"
#include "GenericTeamAgentInterface.h" 	
#include "DiplomacyTypes.generated.h"


// diplomacies
UENUM(BlueprintType)
enum class EDiplomacyTypes : uint8
{
	// judging object's player considers the owner of the other object an enemy
	enemy UMETA(DisplayName = "Enemy"),

	// judging object's player considers the owner of the other object an ally
	ally UMETA(DisplayName = "Ally"),

	// the judging object's player has not indicated their stance towards the owner of the other object
	neutral UMETA(DisplayName = "Neutral"),

	// the other object belongs to the same owner as the judging object
	family UMETA(DisplayName = "Family"),
};


// player types
UENUM(BlueprintType)
enum class EPlayerTypes : uint8
{
	// a human controlled player
	user		UMETA(DisplayName = "User Player"),

	// a computer controlled player
	computer	UMETA(DisplayName = "Computer Player"),

	// a neutral player with predefined behaviors
	neutral		UMETA(DisplayName = "Neutral Player")
};


// neutral faction types
// these are essentially computer players with preset diplomacies
UENUM(BlueprintType)
enum class ENeutralFactionTypes : uint8
{
	// treats everyone as neutral, no exceptions
	// is treated by everyone as neutral, no exceptions
	// nobody hates this, but nobody really likes it either
	passive		UMETA(DisplayName = "Passive"),

	// treats all players as neutral, no exceptions
	// is treated by everyone as enemy (except passive and victim)
	// everybody hates this
	unpopular	UMETA(DisplayName = "Unpopular"),

	// treats all players as neutral, no exceptions
	// is treated by neutral players as enemy (except passive and unpopular)
	// is treated by actual players as neutral
	// everybody but you seems to hate this
	victim		UMETA(DisplayName = "Victim"),

	// treats everyone as enemies (except treats passive and frenemy as neutral)
	// is treated by everyone as an enemy (except passive, victim, unpopular)
	// this is your enemy and your ally's enemy
	hostile		UMETA(DisplayName = "Hostile"),

	// treats all players as allies, except treats passive and frenemy as neutral and hostile as enemy
	// is treated by everyone as an ally (except passive, hostile, victim, and unpopular)
	// this is your friend and ally
	friendly	UMETA(DisplayName = "Friendly"),

	// treats all players as enemy, (except passive and friendly)
	// is treated by neutral players as neutral (except friendly)
	// is treated by actual players as enemy
	// nobody wants to help you fight this enemy
	frenemy		UMETA(DisplayName = "Frenemy"),
};


// represents the unique identity of a faction in the world
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FPlayerTag
{
	GENERATED_BODY()

public:

	// convert a neutral type to an id
	static int GetIdFromNeutralType(const ENeutralFactionTypes NeutralType)
	{
		return static_cast<int>(NeutralType);
	}

	// convert an id to a neutral type
	static ENeutralFactionTypes GetNeutralTypeFromId(const int Id)
	{
		return static_cast<ENeutralFactionTypes>(Id);
	}

	// default constructor
	FPlayerTag()
		: PlayerType(EPlayerTypes::neutral)
		, Id(-1)
		, NeutralType(ENeutralFactionTypes::passive)
	{
	}

	// human or computer constructor
	FPlayerTag(
		const EPlayerTypes InType,
		const int InIndex)
		: PlayerType(InType)
		, Id(InIndex)
		, NeutralType(GetNeutralTypeFromId(InIndex))
	{
	}

	// neutral constructor
	FPlayerTag(const ENeutralFactionTypes InProfile)
		: PlayerType(EPlayerTypes::neutral)
		, Id(GetIdFromNeutralType(InProfile))
		, NeutralType(InProfile)
	{
	}

protected:

	// the type of player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerTypes PlayerType;

public:

	// get the type of player
	EPlayerTypes GetPlayerType() const
	{
		return PlayerType;
	}

	// check if this is a human player
	bool IsUserPlayer() const { return PlayerType == EPlayerTypes::user; }

	// check if this is a computer player
	bool IsComputerPlayer() const { return PlayerType == EPlayerTypes::computer; }

	// check if this is a neutral player
	bool IsNeutralPlayer() const { return PlayerType == EPlayerTypes::neutral; }


protected:

	// the single backing field used at runtime. Represents Index for players/computers, and Profile for neutrals
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlayerType != EPlayerTypes::neutral", EditConditionHides))
	int Id;

public:

	// get the id of the player
	int GetId() const 
	{
		return IsNeutralPlayer() ? GetIdFromNeutralType(NeutralType) : Id;
	}

	// check if the id is valid for this player tag 
	bool IsValidId() const { return GetId() > -1; }


	// converts the player tag to a printable string
	FString ToString() const;

	// get the display name for this player tag
	FText GetDisplayName() const;

	// equality operator for use in TMaps
	bool operator==(FPlayerTag const& Other) const
	{
		return	PlayerType == Other.PlayerType &&
				GetId() == Other.GetId();
	}

	// Tells TMap how to sort this struct into memory.
	friend uint32 GetTypeHash(FPlayerTag const& Ref)
	{
		return HashCombine(GetTypeHash(static_cast<uint8>(Ref.PlayerType)), GetTypeHash(Ref.GetId()));
	}


protected:

	// only exists in the editor to provide the dropdown UX for level designers
	UPROPERTY(EditAnywhere, Category = "Diplomacy", meta = (DisplayName = "Neutral Type", EditCondition = "PlayerType == EPlayerTypes::neutral", EditConditionHides))
	ENeutralFactionTypes NeutralType;

public:

	// get the neutral type indicated by the id of this player tag
	// only valid if player type is neutral
	ENeutralFactionTypes GetNeutralType() const { return NeutralType; }


	// check if this is a specific neutral profile - passive
	bool IsNeutralPassive() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::passive; }

	// check if this is a specific neutral profile - unpopular
	bool IsNeutralUnpopular() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::unpopular; }

	// check if this is a specific neutral profile - victim
	bool IsNeutralVictim() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::victim; }

	// check if this is a specific neutral profile - hostile
	bool IsNeutralHostile() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::hostile; }

	// check if this is a specific neutral profile - friendly
	bool IsNeutralFriendly() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::friendly; }

	// check if this is a specific neutral profile - frenemy
	bool IsNeutralFrenemy() const { return IsNeutralPlayer() && NeutralType == ENeutralFactionTypes::frenemy; }


	// neutral player tag - passive
	// treats everyone as neutral, no exceptions
	// is treated by everyone as neutral, no exceptions
	// nobody hates this, but nobody really likes it either
	static const FPlayerTag NeutralPassive;

	// neutral player tag - hostile
	// treats everyone as enemies (except treats passive and frenemy as neutral)
	// is treated by everyone as an enemy (except passive, victim, unpopular)
	// this is your enemy and your ally's enemy
	static const FPlayerTag NeutralHostile;

	// neutral player tag - friendly
	// treats all players as allies, except treats passive and frenemy as neutral and hostile as enemy
	// is treated by everyone as an ally (except passive, hostile, victim, and unpopular)
	// this is your friend and ally
	static const FPlayerTag NeutralFriendly;

	// neutral player tag - frenemy
	// treats all players as enemy, (except passive and friendly)
	// is treated by neutral players as neutral (except friendly)
	// is treated by actual players as enemy
	// nobody wants to help you fight this enemy
	static const FPlayerTag NeutralFrenemy;

	// neutral player tag - unpopular
	// treats all players as neutral, no exceptions
	// is treated by everyone as enemy (except passive and victim)
	// everybody hates this
	static const FPlayerTag NeutralUnpopular;

	// neutral player tag - victim
	// treats all players as neutral, no exceptions
	// is treated by neutral players as enemy (except passive and unpopular)
	// is treated by actual players as neutral
	// everybody but you seems to hate this
	static const FPlayerTag NeutralVictim;

};


// a list of standings towards other players
// represents how a player views other players
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FPlayerDiplomacies
{
	GENERATED_BODY()

public:

	FPlayerDiplomacies()
		: StandingTowards()
	{
	}

	// map of players and their associated standing diplomacy
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FPlayerTag, EDiplomacyTypes> StandingTowards;

};


// a list of players and their standings towards other players
// represents how all players view other players in a game
// neutral player standings are setup by default
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FGameDiplomacies
{
	GENERATED_BODY()

public:

	FGameDiplomacies()
		: PlayerStandings()
	{
	}

	// set of player standings toward given players
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FPlayerTag, FPlayerDiplomacies> PlayerStandings;

	// register a new player and resolves its standings toward all existing players dynamically
	void RegisterPlayerTag(FPlayerTag const& NewPlayerTag);

	// checks if a player tag exists in the map and has a complete set of mutual standings with all other registered players
	bool IsPlayerTagRegisteredProperly(FPlayerTag const& PlayerTag) const;

	// repairs a player tag by ensuring it is in the map and resetting all mutual standings to their defaults
	void ResetPlayerTag(FPlayerTag const& PlayerTag);

	// gets the default standing that a perspective faction should hold towards a target faction
	static EDiplomacyTypes GetDefaultStanding(FPlayerTag const& Perspective, FPlayerTag const& Target);

	// get the diplomacy type of the given perspective player towards a target player
	EDiplomacyTypes GetPlayerAttitudeTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer) const;

	// set the diplomacy type of the given perspective player towards a target player
	void SetPlayerAttitudeTowardsPlayer(FPlayerTag const& PerspectivePlayer, FPlayerTag const& TargetPlayer, const EDiplomacyTypes NewAttitude);

	// gets all currently registered identities in the game
	TArray<FPlayerTag> GetActivePlayerTags() const;

	// print all stored diplomacies
	void Print() const;

};


/**
 * common diplomacy related functions and constants
 */
UCLASS()
class MEOHELPERSPLUGIN_API UDiplomacyTypes : public UObject
{
	GENERATED_BODY()

public:

	// get the display name for the given diplomacy type
	static FText GetDiplomacyTypeDisplayName(const EDiplomacyTypes DiplomacyType);

	// get the display name for the given diplomacy type
	static FText GetPlayerTypeDisplayName(const EPlayerTypes PlayerType);

	// get the display name for the given neutral type
	static FText GetNeutralTypeDisplayName(const ENeutralFactionTypes NeutralType);

	// convert diplomacy to ue4 ETeamAttitude
	static ETeamAttitude::Type ConvertDiplomacyToTeamAttitude(const EDiplomacyTypes DiplomacyType);


	// gets the controller associated with the owner tag of the given actor
	static AController* GetActorOwnerController(AActor* InActor);

	// gets the controller associated with the owner tag of the given actor
	template <class T>
	static T* GetActorOwnerController(AActor* Actor)
	{
		return Cast<T>(GetActorOwnerController(Actor));
	}

	// iterates over player controllers and finds the one with the given tag
	// player controllers are not replicated, and clients will only be able to see their own
	static APlayerController* GetPlayerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag);

	// iterates over player controllers and finds the one with the given tag
	// player controllers are not replicated, and clients will only be able to see their own
	template <class T>
	static T* GetPlayerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
	{
		return Cast<T>(GetPlayerControllerFromTag(WorldContextObject, Tag));
	}

	// iterates over controllers and finds the one with the given tag
	static AController* GetControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag);

	// iterates over controllers and finds the one with the given tag
	template <class T>
	static T* GetControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
	{
		return Cast<T>(GetControllerFromTag(WorldContextObject, Tag));
	}

	// iterates over controllers and finds the one with the given tag
	static AAIController* GetComputerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag);

	// iterates over controllers and finds the one with the given tag
	template <class T>
	static T* GetComputerControllerFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
	{
		return Cast<T>(GetComputerControllerFromTag(WorldContextObject, Tag));
	}

	// iterates over player states and finds the one with the given tag
	static APlayerState* GetPlayerStateFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag);

	// iterates over player states and finds the one with the given tag
	template <class T>
	static T* GetPlayerStateFromTag(const UObject* WorldContextObject, FPlayerTag const& Tag)
	{
		return Cast<T>(GetPlayerStateFromTag(WorldContextObject, Tag));
	}
};