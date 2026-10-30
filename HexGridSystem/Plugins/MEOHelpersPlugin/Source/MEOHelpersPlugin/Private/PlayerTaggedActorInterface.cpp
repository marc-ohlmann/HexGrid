// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "PlayerTaggedActorInterface.h"

#include "GameFramework/Actor.h"


// Add default functionality here for any IPlayerTaggedActorInterface functions that are not pure virtual.


FPlayerTag IPlayerTaggedActorInterface::GetActorPlayerTag(AActor* InActor)
{
	IF_NULL_RETURN_VAL_LERR(InActor, FPlayerTag::NeutralPassive, MEOHelpers, "InActor is null");

	FPlayerTag OutTag = FPlayerTag::NeutralPassive;
	if (InActor->Implements<UPlayerTaggedActorInterface>())
	{
		const IPlayerTaggedActorInterface* PlayerTaggedActor = Cast<IPlayerTaggedActorInterface>(InActor);
		if(PlayerTaggedActor)
		{
			OutTag = PlayerTaggedActor->GetPlayerTag();
		}
	}


	return OutTag;
}


bool IPlayerTaggedActorInterface::SetActorPlayerTag(AActor* InActor, FPlayerTag const& PlayerTag)
{
	IF_NULL_RETURN_VAL_LERR(InActor, false, MEOHelpers, "InActor is null");

	if (InActor->Implements<UPlayerTaggedActorInterface>())
	{
		IPlayerTaggedActorInterface* PlayerTaggedActor = Cast<IPlayerTaggedActorInterface>(InActor);
		if (PlayerTaggedActor)
		{
			PlayerTaggedActor->SetPlayerTag(PlayerTag);
			return true;
		}
	}


	return false;
}


bool IPlayerTaggedActorInterface::IsActorPlayerTagged(AActor* InActor)
{
	return UMCommon::IsUObjectPointerSafe(InActor) && InActor->Implements<UPlayerTaggedActorInterface>();
}


FPlayerTag UPlayerTaggedActorInterface::GetActorPlayerTag(AActor* InActor)
{
	return IPlayerTaggedActorInterface::GetActorPlayerTag(InActor);
}


void UPlayerTaggedActorInterface::SetActorPlayerTag(AActor* InActor, FPlayerTag const& PlayerTag)
{
	IPlayerTaggedActorInterface::SetActorPlayerTag(InActor, PlayerTag);


	return;
}


bool UPlayerTaggedActorInterface::IsActorPlayerTagged(AActor* InActor)
{
	return IPlayerTaggedActorInterface::IsActorPlayerTagged(InActor);
}