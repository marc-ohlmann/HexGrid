// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DiplomacyTypes.h"
#include "PlayerTaggedActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerTaggedActorInterface : public UInterface
{
	GENERATED_BODY()

public:

	// set the player tag associated with the given actor
	static void SetActorPlayerTag(AActor* InActor, FPlayerTag const& PlayerTag);

	// check if the actor implements the player tagged interface
	static bool IsActorPlayerTagged(AActor* InActor);

	// get the player tag associated with the given actor
	static FPlayerTag GetActorPlayerTag(AActor* InActor);

	// check if the given actor has a specific player tag
	static bool DoesActorHavePlayerTag(AActor* InActor, FPlayerTag const& InTag)
	{
		return IsActorPlayerTagged(InActor) && GetActorPlayerTag(InActor) == InTag;
	}

};

/**
 * Interface for an actor that has a player tag to be used for diplomacy checks
 * 
 * Suggested to be used for player controllers and / or pawns
 */
class MEOHELPERSPLUGIN_API IPlayerTaggedActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// set the player tag associated with this actor
	virtual void SetPlayerTag(FPlayerTag const& InTag) { return; }

	// get the player tag associated with this actor
	virtual FPlayerTag GetPlayerTag() const { return FPlayerTag::NeutralPassive; }

	// set the player tag associated with the given actor
	static bool SetActorPlayerTag(AActor* InActor, FPlayerTag const& PlayerTag);

	// check if the actor implements the player tagged interface
	static bool IsActorPlayerTagged(AActor* InActor);

	// get the player tag associated with the given actor
	static FPlayerTag GetActorPlayerTag(AActor* InActor);

	// check if the given actor has a specific player tag
	static bool DoesActorHavePlayerTag(AActor* InActor, FPlayerTag const& InTag)
	{
		return IsActorPlayerTagged(InActor) && GetActorPlayerTag(InActor) == InTag;
	}

};
