// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HexGridTypes.h"
#include "HexGridSubsystem.generated.h"


class AExampleHexGrid;


/**
 * 
 */
UCLASS()
class HEXGRIDSYSTEM_API UHexGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UHexGridSubsystem();

	static UHexGridSubsystem* Get(UWorld* World);
	static UHexGridSubsystem* Get(UObject* WorldContextObject);
	static UHexGridSubsystem* Get(const UObject* WorldContextObject);

	// Begin USubsystem

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// End USubsystem


	// Hex Grid Generator
public:

	// registers a reference to the given hex grid generator
	static void RegisterHexGrid(AExampleHexGrid* InGenerator);

	// creates and registers a hex grid generator of the given class
	AExampleHexGrid* CreateHexGrid(TSubclassOf<AExampleHexGrid> GeneratorClass, FVector const& WorldLocation = FVector::ZeroVector);
	static AExampleHexGrid* CreateHexGrid(const UObject* WorldContextObject, TSubclassOf<AExampleHexGrid> GeneratorClass, FVector const& WorldLocation = FVector::ZeroVector);

	AExampleHexGrid* GetHexGrid() const;
	static AExampleHexGrid* GetHexGrid(const UObject* WorldContextObject);

private:

	UPROPERTY()
		TWeakObjectPtr<AExampleHexGrid> _HexGrid;

	//
	
};
