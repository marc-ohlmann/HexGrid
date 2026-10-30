// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "MCommon.h"

#include "MEOHelpersPlugin.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h" 	
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "AISystem.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameMapsSettings.h"
#include "Components/ShapeComponent.h"


const FName UMCommon::c_TemporaryUntitledWorldName = FName("Untitled");


bool UMCommon::IsUObjectPointerSafe(const UObject* UObjectPointer)
{
	return	(
		UObjectPointer != nullptr &&
		IsValid(UObjectPointer) == true &&
		UObjectPointer->IsValidLowLevel() == true
		);
}


bool UMCommon::IsValidWorldContextObject(const UObject* WorldContextObject)
{
	return UMCommon::IsUObjectPointerSafe(
		UMCommon::GetWorldFromContextWithSilentFailure(WorldContextObject)
	);
}


FName UMCommon::GetUObjectName(const UObject* Object)
{
	if (IsUObjectPointerSafe(Object) == false)
	{
		return FName();
	}


	return Object->GetFName();
}


UClass* UMCommon::GetUObjectClass(const UObject* Object)
{
	if (IsUObjectPointerSafe(Object) == false)
	{
		return nullptr;
	}


	return Object->GetClass();
}


FName UMCommon::GetUObjectClassName(const UObject* Object)
{
	UClass* ObjectClass = GetUObjectClass(Object);
	if (IsUObjectPointerSafe(ObjectClass) == false)
	{
		return FName();
	}


	return ObjectClass->GetFName();
}


bool UMCommon::DoesUObjectHaveName(const UObject* Object, FName const& ObjectName, ENameCase CompareMethod, bool bCompareNumber)
{
	if (IsUObjectPointerSafe(Object) == false)
	{
		return false;
	}


	return GetUObjectName(Object).IsEqual(ObjectName, CompareMethod, bCompareNumber);
}


int UMCommon::GetControllerIndexFromPlayerController(const APlayerController* PlayerController)
{
	IF_NULL_RETURN_VAL(PlayerController, -1);


	return GetControllerIndexFromLocalPlayer(PlayerController->GetLocalPlayer());
}


int UMCommon::GetControllerIndexFromLocalPlayer(const ULocalPlayer* LocalPlayer)
{
	IF_NULL_RETURN_VAL(LocalPlayer, -1);


	return LocalPlayer->GetPlatformUserIndex();
}



void UMCommon::SetControllerIndexForPlayerController(APlayerController* PlayerController, const int ControllerIndex)
{
	IF_NULL_RETURN(PlayerController);

	UGameplayStatics::SetPlayerControllerID(PlayerController, ControllerIndex);


	return;
}


APlayerController* UMCommon::GetFirstLocalPlayerController(const UObject* WorldContextObject)
{
	TArray<APlayerController*> LocalPlayers = GetLocalPlayerControllers(WorldContextObject);

	// return first valid pointer
	FOR_FORWARD(i, LocalPlayers.Num())
	{
		if (IsUObjectPointerSafe(LocalPlayers[i]))
		{
			return LocalPlayers[i];
		}
	}


	return nullptr;
}


APlayerController* UMCommon::GetLocalPlayerController(const UObject* WorldContextObject, const ULocalPlayer* LocalPlayer)
{
	IF_NULL_RETURN_NULL(LocalPlayer);

	const UWorld* World = UMCommon::GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL(World);


	return LocalPlayer->GetPlayerController(World);
}


APlayerController* UMCommon::GetLocalPlayerController(const UObject* WorldContextObject, const int ControllerIndex)
{
	return UGameplayStatics::GetPlayerControllerFromID(WorldContextObject, ControllerIndex);
}


ULocalPlayer* UMCommon::GetLocalPlayer(const UObject* WorldContextObject, const int ControllerIndex)
{
	const APlayerController* LocalPlayerController = GetLocalPlayerController(WorldContextObject, ControllerIndex);
	IF_NULL_RETURN_NULL(LocalPlayerController);


	return GetLocalPlayer(LocalPlayerController);
}


ULocalPlayer* UMCommon::GetLocalPlayer(const APlayerController* LocalPlayerController)
{
	IF_NULL_RETURN_NULL(LocalPlayerController);


	return LocalPlayerController->GetLocalPlayer();
}



AHUD* UMCommon::GetLocalPlayerHUD(const UObject* WorldContextObject, const int ControllerIndex)
{
	const APlayerController* LocalPlayerController = GetLocalPlayerController(WorldContextObject, ControllerIndex);
	if (IsUObjectPointerSafe(LocalPlayerController) == false)
	{
		return nullptr;
	}


	return LocalPlayerController->GetHUD();
}


APlayerState* UMCommon::GetLocalPlayerState(const UObject* WorldContextObject, const int ControllerIndex)
{
	const APlayerController* LocalPlayerController = GetLocalPlayerController(WorldContextObject, ControllerIndex);
	if (IsUObjectPointerSafe(LocalPlayerController) == false)
	{
		return nullptr;
	}


	return LocalPlayerController->GetPlayerState<APlayerState>();
}


TArray<APlayerController*> UMCommon::GetLocalPlayerControllers(const UObject* WorldContextObject)
{
	TArray<APlayerController*> PlayerControllers = GetPlayerControllers(WorldContextObject);
	PlayerControllers.RemoveAllSwap(
		[&](APlayerController* PC)
		{
			return IsPlayerControllerLocalToThisClient(PC) == false;
		}
	);


	return PlayerControllers;
}


TArray<APlayerController*> UMCommon::GetClientPlayerControllers(const UObject* WorldContextObject)
{
	TArray<APlayerController*> PlayerControllers = GetPlayerControllers(WorldContextObject);
	PlayerControllers.RemoveAllSwap(
		[&](APlayerController* PC)
		{
			return IsPlayerControllerLocalToThisClient(PC) == true;
		}
	);


	return PlayerControllers;
}


TArray<int> UMCommon::GetLocalPlayerControllerIndices(const UObject* WorldContextObject)
{
	TArray<APlayerController*> LocalPlayerControllers = GetPlayerControllers(WorldContextObject);
	TArray<int> Out;

	FOR_FORWARD(i, LocalPlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(LocalPlayerControllers[i]))
		{
			const int ControllerIndex = GetControllerIndexFromPlayerController(LocalPlayerControllers[i]);
			if (IsPlayerControllerIndexValid(ControllerIndex))
			{
				Out.Add(ControllerIndex);
			}
		}
	}


	return Out;
}


TArray<AHUD*> UMCommon::GetLocalPlayerHUDs(const UObject* WorldContextObject)
{
	TArray<APlayerController*> LocalPlayerControllers = GetLocalPlayerControllers(WorldContextObject);
	TArray<AHUD*> Out;
	FOR_FORWARD(i, LocalPlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(LocalPlayerControllers[i]) == true)
		{
			AHUD* PlayerHUD = LocalPlayerControllers[i]->GetHUD();
			if (IsUObjectPointerSafe(PlayerHUD) == true)
			{
				Out.Add(PlayerHUD);
			}
		}
	}


	return Out;
}


TArray<APlayerState*> UMCommon::GetLocalPlayerStates(const UObject* WorldContextObject)
{
	TArray<APlayerController*> LocalPlayerControllers = GetLocalPlayerControllers(WorldContextObject);
	TArray<APlayerState*> Out;
	FOR_FORWARD(i, LocalPlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(LocalPlayerControllers[i]) == true)
		{
			APlayerState* PlayerState = LocalPlayerControllers[i]->GetPlayerState<APlayerState>();
			if (IsUObjectPointerSafe(PlayerState) == true)
			{
				Out.Add(PlayerState);
			}
		}
	}


	return Out;
}


TArray<APlayerState*> UMCommon::GetClientPlayerStates(const UObject* WorldContextObject)
{
	TArray<APlayerController*> ClientPlayerControllers = GetClientPlayerControllers(WorldContextObject);
	TArray<APlayerState*> Out;
	FOR_FORWARD(i, ClientPlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(ClientPlayerControllers[i]) == true)
		{
			APlayerState* PlayerState = ClientPlayerControllers[i]->GetPlayerState<APlayerState>();
			if (IsUObjectPointerSafe(PlayerState) == true)
			{
				Out.Add(PlayerState);
			}
		}
	}


	return Out;
}


TArray<APawn*> UMCommon::GetLocalPlayerPawns(const UObject* WorldContextObject)
{
	TArray<APlayerController*> LocalPlayerControllers = GetLocalPlayerControllers(WorldContextObject);
	TArray<APawn*> Out;
	FOR_FORWARD(i, LocalPlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(LocalPlayerControllers[i]) == true)
		{
			APawn* Pawn = LocalPlayerControllers[i]->GetPawn();
			if (IsUObjectPointerSafe(Pawn) == true)
			{
				Out.Add(Pawn);
			}
		}
	}


	return Out;
}


APawn* UMCommon::GetLocalPlayerPawn(const UObject* WorldContextObject, const int LocalPlayerIndex)
{
	const APlayerController* LocalPlayer = GetLocalPlayerController(WorldContextObject, LocalPlayerIndex);
	if (IsUObjectPointerSafe(LocalPlayer) == false)
	{
		return nullptr;
	}


	return LocalPlayer->GetPawn();
}


UEngine* UMCommon::GetEngine()
{
	IF_NULL_RETURN_NULL_LERR(GEngine, MEOHelpers, "global engine pointer is null");


	return GEngine;
}


UEngineSubsystem* UMCommon::GetEngineSubsystem(TSubclassOf<UEngineSubsystem> const& EngineSubsystemClass)
{
	const UEngine* Engine = GetEngine();
	IF_NULL_RETURN_NULL_LERR(Engine, MEOHelpers, "global engine pointer is null");


	return Engine->GetEngineSubsystemBase(EngineSubsystemClass);
}


UWorld* UMCommon::GetWorldFromOuter(UObject* Outer)
{
	IF_NULL_RETURN_NULL_LERR(Outer, MEOHelpers, "outer is null");

	UWorld* World = nullptr;

	// try casting the outer to a world directly
	World = Cast<UWorld>(Outer);
	if (UMCommon::IsUObjectPointerSafe(World))
	{
		return World;
	}

	// see if the outer is an actor
	AActor* OuterActor = Cast<AActor>(Outer);
	if (UMCommon::IsUObjectPointerSafe(OuterActor))
	{
		return OuterActor->GetWorld();
	}

	// try the normal method of stepping up the outer chain
	World = Outer->GetWorld();
	if (UMCommon::IsUObjectPointerSafe(World))
	{
		return World;
	}

	L_ERR(MEOHelpers, "could not find world from outer '{0}'", Outer);


	return nullptr;
}


UWorld* UMCommon::GetWorldFromContextWithSilentFailure(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_NULL(WorldContextObject);

	const UEngine* Engine = GetEngine();
	IF_NULL_RETURN_NULL(Engine);


	return Engine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
}


UWorld* UMCommon::GetWorldFromContext(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_NULL_LERR(WorldContextObject, MEOHelpers, "world context object is null");

	const UEngine* Engine = GetEngine();
	IF_NULL_RETURN_NULL_LERR(Engine, MEOHelpers, "global engine pointer is null");


	return Engine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
}


FTimerManager* UMCommon::GetGameTimerManager(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(GameInstance, MEOHelpers, "could not get game instance");


	return &GameInstance->GetTimerManager();
}


FTimerManager* UMCommon::GetWorldTimerManager(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world -- consider using 'GetGameTimerManager' for a world-independent timer");


	return &World->GetTimerManager();
}


UAISystemBase* UMCommon::GetAISystem(const UObject* WorldContextObject)
{
	UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world");


	return UAISystem::GetCurrent(*World);
}


UGameInstance* UMCommon::GetGameInstance(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world");


	return World->GetGameInstance();
}

UGameInstanceSubsystem* UMCommon::GetGameInstanceSubsystem(const UObject* WorldContextObject, const TSubclassOf<UGameInstanceSubsystem> GameInstanceSubsystemClass)
{
	const UGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(GameInstance, MEOHelpers, "could not get game instance");


	return GameInstance->GetSubsystemBase(GameInstanceSubsystemClass);
}


UWorldSubsystem* UMCommon::GetWorldSubsystem(
	const UObject* WorldContextObject,
	const TSubclassOf<UWorldSubsystem> WorldSubsystemClass
)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world");


	return World->GetSubsystemBase(WorldSubsystemClass);
}


ULocalPlayerSubsystem* UMCommon::GetLocalPlayerSubsystem(
	const ULocalPlayer* LocalPlayer,
	const TSubclassOf<ULocalPlayerSubsystem> LocalPlayerSubsystemClass
)
{
	IF_NULL_RETURN_NULL_LERR(LocalPlayer, MEOHelpers, "local player is null");


	return LocalPlayer->GetSubsystemBase(LocalPlayerSubsystemClass);
}


/*
float UMCommon::GetGameElapsedTime(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	IF_NULL_RETURN_VAL_LERR(GameInstance, -1.f, MEOHelpers, "could not get game instance");

	UGameplayStatics::GetElapsed
	return GameInstance->GetEl();
}*/


double UMCommon::GetWorldElapsedTime(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_VAL_LERR(World, -1.f, MEOHelpers, "could not get world");


	return World->GetTimeSeconds();
}


double UMCommon::GetWorldElapsedRealTime(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_VAL_LERR(World, -1.f, MEOHelpers, "could not get world");


	return World->GetRealTimeSeconds();
}


UGameViewportClient* UMCommon::GetGameViewportClient(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(GameInstance, MEOHelpers, "could not get game instance");


	return GameInstance->GetGameViewportClient();
}


AGameModeBase* UMCommon::GetGameMode(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world");


	return World->GetAuthGameMode();
}


AGameStateBase* UMCommon::GetGameState(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "could not get world");


	return World->GetGameState();
}


TArray<AController*> UMCommon::GetControllers(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_TYPE_LERR(World, TArray<AController*>, MEOHelpers, "could not get world");

	TArray<AController*> Out;
	for (FConstControllerIterator Iterator = World->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* Controller = Iterator->Get();
		if (IsUObjectPointerSafe(Controller))
		{
			Out.Add(Controller);
		}
	}


	return Out;
}


TArray<APawn*> UMCommon::GetControllerPawns(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_TYPE_LERR(World, TArray<APawn*>, MEOHelpers, "could not get world");

	TArray<APawn*> Out;
	TArray<AController*> Controllers = GetControllers(WorldContextObject);
	FOR_FORWARD(i, Controllers.Num())
	{
		if (IsUObjectPointerSafe(Controllers[i]))
		{
			APawn* ControllerPawn = Controllers[i]->GetPawn();
			if (IsUObjectPointerSafe(ControllerPawn))
			{
				Out.Add(ControllerPawn);
			}
		}
	}


	return Out;
}


TArray<AAIController*> UMCommon::GetAIControllers(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_TYPE_LERR(World, TArray<AAIController*>, MEOHelpers, "could not get world");

	TArray<AAIController*> Out;
	for (FConstControllerIterator Iterator = World->GetControllerIterator(); Iterator; ++Iterator)
	{
		AAIController* AIController = Cast<AAIController>(Iterator->Get());
		if (IsUObjectPointerSafe(AIController))
		{
			Out.Add(AIController);
		}
	}


	return Out;
}


TArray<APlayerState*> UMCommon::GetAIPlayerStates(const UObject* WorldContextObject)
{
	TArray<AAIController*> AIControllers = UMCommon::GetAIControllers(WorldContextObject);
	TArray<APlayerState*> Out;

	FOR_FORWARD(i, AIControllers.Num())
	{
		if (IsUObjectPointerSafe(AIControllers[i]))
		{
			APlayerState* PlayerState = AIControllers[i]->GetPlayerState<APlayerState>();
			if (IsUObjectPointerSafe(PlayerState))
			{
				Out.Add(PlayerState);
			}
		}
	}

	L_FATAL(MEOHelpers, "");


	return Out;
}


TArray<APawn*> UMCommon::GetAIPlayerPawns(const UObject* WorldContextObject)
{
	TArray<AAIController*> AIControllers = UMCommon::GetAIControllers(WorldContextObject);
	TArray<APawn*> Out;

	FOR_FORWARD(i, AIControllers.Num())
	{
		if (IsUObjectPointerSafe(AIControllers[i]))
		{
			APawn* Pawn = AIControllers[i]->GetPawn();
			if (IsUObjectPointerSafe(Pawn))
			{
				Out.Add(Pawn);
			}
		}
	}


	return Out;
}


TArray<APlayerController*> UMCommon::GetPlayerControllers(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_TYPE_LERR(World, TArray<APlayerController*>, MEOHelpers, "could not get world");

	TArray<APlayerController*> Out;
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (IsUObjectPointerSafe(PlayerController))
		{
			Out.Add(PlayerController);
		}
	}


	return Out;
}


TArray<APlayerState*> UMCommon::GetHumanPlayerStates(const UObject* WorldContextObject)
{
	TArray<APlayerController*> PlayerControllers = GetPlayerControllers(WorldContextObject);
	TArray<APlayerState*> PlayerStates;
	FOR_FORWARD(i, PlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(PlayerControllers[i]) == false)
		{
			continue;
		}

		APlayerState* PlayerState = PlayerControllers[i]->GetPlayerState<APlayerState>();
		if (IsUObjectPointerSafe(PlayerState) == false)
		{
			continue;
		}

		PlayerStates.Add(PlayerState);
	}


	return PlayerStates;
}


TArray<APlayerState*> UMCommon::GetPlayerStates(const UObject* WorldContextObject)
{
	const AGameStateBase* GameState = GetGameState(WorldContextObject);
	IF_NULL_RETURN_TYPE_LERR(GameState, TArray<APlayerState*>, MEOHelpers, "could not get game state");


	return GameState->PlayerArray;
}


TArray<APawn*> UMCommon::GetPlayerPawns(const UObject* WorldContextObject)
{
	TArray<APlayerController*> PlayerControllers = GetPlayerControllers(WorldContextObject);
	TArray<APawn*> Out;

	FOR_FORWARD(i, PlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(PlayerControllers[i]))
		{
			APawn* Pawn = PlayerControllers[i]->GetPawn();
			if (IsUObjectPointerSafe(Pawn))
			{
				Out.Add(Pawn);
			}
		}
	}


	return Out;
}


TArray<APawn*> UMCommon::GetClientPlayerPawns(const UObject* WorldContextObject)
{
	TArray<APlayerController*> PlayerControllers = GetClientPlayerControllers(WorldContextObject);
	TArray<APawn*> Out;

	FOR_FORWARD(i, PlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(PlayerControllers[i]))
		{
			APawn* Pawn = PlayerControllers[i]->GetPawn();
			if (IsUObjectPointerSafe(Pawn))
			{
				Out.Add(Pawn);
			}
		}
	}


	return Out;
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UWorld* World,
	FVector const& SearchLocation,
	const float InnerSearchRadius,
	const float OuterSearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FRandomStream const& RandomStream,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	IF_NULL_RETURN_VAL_LERR(World, false, MEOHelpers, "world is null");

	FVector SpillLocation = FVector::ZeroVector;
	FHitResult HitResult = FHitResult();

	FOR_FORWARD(i, MaxSearchAttempts)
	{
		// select a spill location
		SpillLocation = UMathTypes::GetRandomLocationWithinRadiusOfLocation(SearchLocation, InnerSearchRadius, OuterSearchRadius, RandomStream, true);

		// trace downward to find a location on the floor, or another static object
		const FVector SpillTraceLocationEnd = SpillLocation + (FVector::DownVector * SearchHeight);
		const bool bHitSuccessResult = World->LineTraceSingleByChannel(
			HitResult,
			SpillLocation,
			SpillTraceLocationEnd,
			TraceChannel,
			CollisionQueryParams,
			CollisionResponseParams
		);

		if (bHitSuccessResult)
		{
			OutVector = HitResult.Location;
			return true;
		}
	}

	L_WARN(MEOHelpers, "failed to find a ground location after {0} attempts", MaxSearchAttempts);


	return false;
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UWorld* World,
	FVector const& SearchLocation,
	const float InnerSearchRadius,
	const float OuterSearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FName const& Seed,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	const FRandomStream SRand = UMathTypes::GenerateSeededRandomStream(Seed);


	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		InnerSearchRadius,
		OuterSearchRadius,
		SearchHeight,
		OutVector,
		SRand,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UWorld* World,
	FVector const& SearchLocation,
	const float SearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FName const& Seed,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		0.f,
		SearchRadius,
		SearchHeight,
		OutVector,
		Seed,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UWorld* World,
	FVector const& SearchLocation,
	const float SearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FRandomStream const& RandomStream,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		0.f,
		SearchRadius,
		SearchHeight,
		OutVector,
		RandomStream,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UObject* WorldContextObject,
	FVector const& SearchLocation,
	const float InnerSearchRadius,
	const float OuterSearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FRandomStream const& RandomStream,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);

	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		InnerSearchRadius,
		OuterSearchRadius,
		SearchHeight,
		OutVector,
		RandomStream,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UObject* WorldContextObject,
	FVector const& SearchLocation,
	const float InnerSearchRadius,
	const float OuterSearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FName const& Seed,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);

	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		InnerSearchRadius,
		OuterSearchRadius,
		SearchHeight,
		OutVector,
		Seed,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UObject* WorldContextObject,
	FVector const& SearchLocation,
	const float SearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FName const& Seed,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);

	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		0.f,
		SearchRadius,
		SearchHeight,
		OutVector,
		Seed,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


bool UMCommon::GetRandomGroundLocationWithinRadiusOfLocation(
	const UObject* WorldContextObject,
	FVector const& SearchLocation,
	const float SearchRadius,
	const float SearchHeight,
	FVector& OutVector,
	FRandomStream const& RandomStream,
	const ECollisionChannel TraceChannel,
	FCollisionQueryParams const& CollisionQueryParams,
	FCollisionResponseParams const& CollisionResponseParams,
	const int MaxSearchAttempts)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);

	return GetRandomGroundLocationWithinRadiusOfLocation(
		World,
		SearchLocation,
		0.f,
		SearchRadius,
		SearchHeight,
		OutVector,
		RandomStream,
		TraceChannel,
		CollisionQueryParams,
		CollisionResponseParams,
		MaxSearchAttempts);
}


int UMCommon::GetNetworkIndexFromPlayerState(const APlayerState* PlayerState)
{
	if (IsUObjectPointerSafe(PlayerState) == false)
	{
		return -1;
	}


	return PlayerState->GetPlayerId();
}


int UMCommon::GetNetworkIndexFromPlayerController(const APlayerController* PlayerController)
{
	if (IsUObjectPointerSafe(PlayerController) == false)
	{
		return -1;
	}


	return UMCommon::GetNetworkIndexFromPlayerState(PlayerController->GetPlayerState<APlayerState>());
}


APlayerState* UMCommon::GetPlayerStateWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
{
	TArray<APlayerState*> PlayerStates = GetHumanPlayerStates(WorldContextObject);
	FOR_FORWARD(i, PlayerStates.Num())
	{
		if (IsUObjectPointerSafe(PlayerStates[i]) && GetNetworkIndexFromPlayerState(PlayerStates[i]) == NetworkIndex)
		{
			return PlayerStates[i];
		}
	}


	return nullptr;
}


APlayerController* UMCommon::GetPlayerControllerWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
{
	TArray<APlayerController*> PlayerControllers = GetPlayerControllers(WorldContextObject);
	FOR_FORWARD(i, PlayerControllers.Num())
	{
		if (IsUObjectPointerSafe(PlayerControllers[i]) && GetNetworkIndexFromPlayerController(PlayerControllers[i]) == NetworkIndex)
		{
			return PlayerControllers[i];
		}
	}


	return nullptr;
}


float UMCommon::GetWorldDeltaSeconds(const UObject* WorldContextObject)
{
	const UWorld* World = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_VAL(World, 0.f);


	return World->GetDeltaSeconds();
}


UActorComponent* UMCommon::GetComponentFromActor(const AActor* ComponentOwner, const TSubclassOf<UActorComponent> ComponentClass)
{
	IF_NULL_RETURN_NULL(ComponentOwner);


	return ComponentOwner->GetComponentByClass(ComponentClass);
}


UActorComponent* UMCommon::GetComponentForLocalPlayerState(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int ControllerIndex
)
{
	const APlayerState* Player = GetLocalPlayerState(WorldContextObject, ControllerIndex);
	IF_NULL_RETURN_NULL(Player);


	return GetComponentFromActor(Player, ComponentClass);
}


UActorComponent* UMCommon::GetComponentForPlayerStateWithNetworkIndex(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int NetworkIndex
)
{
	const APlayerState* Player = GetPlayerStateWithNetworkIndex(WorldContextObject, NetworkIndex);
	IF_NULL_RETURN_NULL(Player);


	return GetComponentFromActor(Player, ComponentClass);
}


UActorComponent* UMCommon::GetComponentForPlayerControllerWithNetworkIndex(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int NetworkIndex
)
{
	const APlayerController* Player = GetPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex);
	IF_NULL_RETURN_NULL(Player);


	return GetComponentFromActor(Player, ComponentClass);
}


APawn* UMCommon::GetPawnForPlayerControllerWithNetworkIndex(
	const UObject* WorldContextObject,
	const int NetworkIndex
)
{
	const APlayerController* Player = GetPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex);
	IF_NULL_RETURN_NULL(Player);


	return Player->GetPawn();
}


UActorComponent* UMCommon::GetComponentForPlayerPawnWithNetworkIndex(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int NetworkIndex
)
{
	const APawn* Pawn = GetPawnForPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex);
	IF_NULL_RETURN_NULL(Pawn);


	return GetComponentFromActor(Pawn, ComponentClass);
}


UActorComponent* UMCommon::GetComponentForLocalPlayerController(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int ControllerIndex
)
{
	const APlayerController* Player = GetLocalPlayerController(WorldContextObject, ControllerIndex);
	IF_NULL_RETURN_NULL(Player);


	return GetComponentFromActor(Player, ComponentClass);
}


UActorComponent* UMCommon::GetComponentForLocalPlayerHUD(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int ControllerIndex
)
{
	const AHUD* HUD = GetLocalPlayerHUD(WorldContextObject, ControllerIndex);
	IF_NULL_RETURN_NULL(HUD);


	return GetComponentFromActor(HUD, ComponentClass);
}


UActorComponent* UMCommon::GetComponentForLocalPlayerPawn(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> ComponentClass,
	const int ControllerIndex
)
{
	const APawn* Pawn = GetLocalPlayerPawn(WorldContextObject, ControllerIndex);
	IF_NULL_RETURN_NULL(Pawn);


	return GetComponentFromActor(Pawn, ComponentClass);
}


UActorComponent* UMCommon::GetGameStateComponent(const UObject* WorldContextObject, const TSubclassOf<UActorComponent> ComponentClass)
{
	const AGameStateBase* GameState = GetGameState(WorldContextObject);
	IF_NULL_RETURN_NULL(GameState);


	return GetComponentFromActor(GameState, ComponentClass);
}


UActorComponent* UMCommon::GetGameModeComponent(const UObject* WorldContextObject, const TSubclassOf<UActorComponent> ComponentClass)
{
	const AGameModeBase* GameState = GetGameMode(WorldContextObject);
	IF_NULL_RETURN_NULL(GameState);

	return GetComponentFromActor(GameState, ComponentClass);
}


APlayerController* UMCommon::GetPlayerControllerFromWidget(const UWidget* Widget)
{
	IF_NULL_RETURN_NULL(Widget);


	return Widget->GetOwningPlayer();
}


int UMCommon::GetControllerIndexFromWidget(const UWidget* Widget)
{
	return GetControllerIndexFromPlayerController(
		GetPlayerControllerFromWidget(Widget)
	);
}


UUserWidget* UMCommon::FindUserWidgetOfClassForPlayerController(const APlayerController* Player, const TSubclassOf<UUserWidget> UserWidgetClass, bool bTopLevelOnly)
{
	IF_NULL_RETURN_NULL_LERR(UserWidgetClass, MEOHelpers, "UserWidgetClass is null");
	IF_NULL_RETURN_NULL_LERR(Player, MEOHelpers, "Player is null");

	// GetAllWidgetsOfClass requires non-const but it doesnt call any non-const functions
	// so we const-cast to a non-const player so that we can call this on players that only have const access
	APlayerController* DeConstifiedPlayer = const_cast<APlayerController*>(Player);
	IF_NULL_RETURN_NULL_LERR(DeConstifiedPlayer, MEOHelpers, "DeConstifiedPlayer is null");

	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(DeConstifiedPlayer, Widgets, UserWidgetClass, bTopLevelOnly);

	if (Widgets.Num() > 0)
	{
		// return first one owned by the player
		FOR_FORWARD(i, Widgets.Num())
		{
			if (GetPlayerControllerFromWidget(Widgets[i]) == Player)
			{
				return Widgets[i];
			}
		}
	}

	L_WARN(MEOHelpers, "failed to find widget of class '{0}' for player '{1}' index '{2}'",
		UserWidgetClass,
		Player,
		GetControllerIndexFromPlayerController(Player)
	);


	return nullptr;
}


UUserWidget* UMCommon::FindUserWidgetOfClassForPlayerController(const UObject* WorldContextObject, const int LocalPlayerId, const TSubclassOf<UUserWidget> UserWidgetClass, bool bTopLevelOnly)
{
	return FindUserWidgetOfClassForPlayerController(
		GetLocalPlayerController(WorldContextObject, LocalPlayerId),
		UserWidgetClass,
		bTopLevelOnly
	);
}


bool UMCommon::IsInWorld(const UObject* WorldContextObject, UWorld* World)
{
	return World == GetWorldFromContext(WorldContextObject);
}


bool UMCommon::IsInWorldSoftly(const UObject* WorldContextObject, TSoftObjectPtr<UWorld> const& World)
{
	const FName WorldName = FName(World.GetAssetName());
	if (WorldName.IsNone())
	{
		L_ERR(MEOHelpers, "WorldName is None");
		return false;
	}


	return IsInWorldName(WorldContextObject, WorldName);
}


bool UMCommon::IsInTransitionWorld(const UObject* WorldContextObject)
{
	const UGameMapsSettings* GameMapsDevSettings = GetDefault<UGameMapsSettings>();
	IF_NULL_RETURN_VAL_LERR(GameMapsDevSettings, false, MEOHelpers, "GameMapsDevSettings is null");

	const TSoftObjectPtr<UWorld> TransitionWorld = TSoftObjectPtr<UWorld>(GameMapsDevSettings->TransitionMap);


	return IsInWorldSoftly(WorldContextObject, TransitionWorld);
}


bool UMCommon::IsInWorldName(const UObject* WorldContextObject, FName const& WorldName)
{
	if (WorldName.IsNone())
	{
		return false;
	}

	UWorld* ContextWorld = GetWorldFromContext(WorldContextObject);
	IF_NULL_RETURN_VAL(ContextWorld, false);

	const FName ContextWorldName = ContextWorld->GetFName();
	if (ContextWorldName.IsNone())
	{
		L_ERR(MEOHelpers, "ContextWorldName is None");
		return false;
	}


	return WorldName.IsEqual(ContextWorldName);
}


bool UMCommon::IsInTemporaryUntitledWorld(const UObject* WorldContextObject)
{
	return IsInWorldName(WorldContextObject, c_TemporaryUntitledWorldName);
}


bool UMCommon::IsEditorWorld(const UWorld* World)
{
	IF_NULL_RETURN_VAL_LERR(World, false, MEOHelpers, "World is null");


	return World->IsEditorWorld();
}


bool UMCommon::IsInEditorWorld(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_VAL_LERR(WorldContextObject, false, MEOHelpers, "WorldContextObject is null");


	return UMCommon::IsEditorWorld(UMCommon::GetWorldFromContext(WorldContextObject));
}


bool UMCommon::IsGameWorld(const UWorld* World)
{
	IF_NULL_RETURN_VAL_LERR(World, false, MEOHelpers, "World is null");


	return World->IsGameWorld();
}


bool UMCommon::IsInGameWorld(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_VAL_LERR(WorldContextObject, false, MEOHelpers, "WorldContextObject is null");


	return UMCommon::IsGameWorld(UMCommon::GetWorldFromContext(WorldContextObject));
}


bool UMCommon::IsWorldStartingUp(UWorld* World)
{
	IF_NULL_RETURN_VAL_LERR(World, false, MEOHelpers, "World is null");


	return World->bStartup;
}


bool UMCommon::IsWorldStartingUp(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_VAL_LERR(WorldContextObject, false, MEOHelpers, "WorldContextObject is null");


	return IsWorldStartingUp(
		GetWorldFromContext(WorldContextObject)
	);
}


UPrimitiveComponent* UMCommon::GetActorColliderComponent(AActor* InActor)
{
	IF_NULL_RETURN_NULL_LERR(InActor, MEOHelpers, "InActor is null");

	// first, check for a shape
	UShapeComponent* ActorShape = InActor->GetComponentByClass<UShapeComponent>();
	if (UMCommon::IsUObjectPointerSafe(ActorShape))
	{
		return ActorShape;
	}

	// lastly, check for any primitive
	UPrimitiveComponent* ActorPrimitive = InActor->GetComponentByClass<UPrimitiveComponent>();
	if (UMCommon::IsUObjectPointerSafe(ActorPrimitive))
	{
		return ActorPrimitive;
	}

	L_WARN(MEOHelpers, "failed to find collider for actor '{0}'", InActor);


	return nullptr;
}


UObject* UMCommon::DuplicateUObject(UObject* SourceObject, UObject* Outer, FName const& DuplicateObjectName)
{
	IF_NULL_RETURN_NULL_LERR(SourceObject, MEOHelpers, "SourceObject is null");

#if WITH_EDITOR

	if (SourceObject->IsA<AActor>())
	{
		L_WARN(MEOHelpers, "SourceObject '{0}' is an actor, suggest using UMCommon::DuplicateActor instead", SourceObject);
	}

#endif

	// create a duplicate of the asset object to copy properties over
	UObject* DuplicatedObject = DuplicateObject_Internal(
		SourceObject->GetClass(),
		SourceObject,
		Outer,
		DuplicateObjectName
	);
	IF_NULL_RETURN_NULL_LERR(DuplicatedObject, MEOHelpers, "DuplicatedObject is null");

	// clear problematic object flags on the duplicated object
	DuplicatedObject->ClearFlags(EObjectFlags::RF_WasLoaded);


	return DuplicatedObject;
}


AActor* UMCommon::DuplicateActor(AActor* SourceActor, AActor* OwnerActor, FName const& DuplicateActorName)
{
	IF_NULL_RETURN_NULL_LERR(SourceActor, MEOHelpers, "SourceActor is null");

	UWorld* World = SourceActor->GetWorld();
	IF_NULL_RETURN_NULL_LERR(World, MEOHelpers, "World is null");

	// create a duplicate of the actor to copy properties over
	FActorSpawnParameters SpawnParams;
	SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Template = SourceActor;
	SpawnParams.ObjectFlags = EObjectFlags::RF_Transient | EObjectFlags::RF_DuplicateTransient;
	SpawnParams.Name = DuplicateActorName;
	SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.Owner = OwnerActor;

	const FTransform DefaultActorTransform = SourceActor->GetActorTransform();

	// create duplicate actor
	AActor* DuplicateActor = World->SpawnActor<AActor>(SourceActor->GetClass(), DefaultActorTransform, SpawnParams);
	IF_NULL_RETURN_NULL_LERR(DuplicateActor, MEOHelpers, "DuplicateActor is null");

#if WITH_EDITOR

	DuplicateActor->SetActorLabel(DuplicateActor->GetName());

#endif


	return DuplicateActor;
}