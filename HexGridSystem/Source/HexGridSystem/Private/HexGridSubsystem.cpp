// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexGridSubsystem.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "ExampleHexGrid.h"


UHexGridSubsystem::UHexGridSubsystem()
{

}


void UHexGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	return;
}


void UHexGridSubsystem::Deinitialize()
{
	Super::Deinitialize();


	return;
}


UHexGridSubsystem* UHexGridSubsystem::Get(UWorld* World)
{
	IF_NULL_RETURN_NULL_LERR(World, HexGrid, "world is null");


	return World->GetSubsystem<UHexGridSubsystem>();
}

UHexGridSubsystem* UHexGridSubsystem::Get(UObject* WorldContextObject)
{
	IF_NULL_RETURN_NULL_LERR(WorldContextObject, HexGrid, "world context is null");


	return UHexGridSubsystem::Get(UMCommon::GetWorldFromContext(WorldContextObject));
}


UHexGridSubsystem* UHexGridSubsystem::Get(const UObject* WorldContextObject)
{
	IF_NULL_RETURN_NULL_LERR(WorldContextObject, HexGrid, "world context is null");


	return UHexGridSubsystem::Get(UMCommon::GetWorldFromContext(WorldContextObject));
}


void UHexGridSubsystem::RegisterHexGrid(AExampleHexGrid* InGenerator)
{
	IF_NULL_RETURN_LERR(InGenerator, HexGrid, "generator is null");

	UHexGridSubsystem* HexGridSubsystem = Get(InGenerator);
	IF_NULL_RETURN_LERR(HexGridSubsystem, HexGrid, "could not get hex grid subsystem");

	AExampleHexGrid* ExistingGenerator = GetHexGrid(InGenerator);
	if (UMCommon::IsUObjectPointerSafe(ExistingGenerator) && 
		UMCommon::IsUObjectPointerSafe(InGenerator))
	{
		if (ExistingGenerator == InGenerator)
		{
			// this generator has already registered
			return;
		}

		// registering a second world generator -- this is unusual, are two separate grids necessary?
		L_WARN(HexGrid, "generator already registered: '{0}' -- replacing reference with '{1}'", ExistingGenerator, InGenerator);
	}

	HexGridSubsystem->_HexGrid = InGenerator;

	L_VERBOSE(HexGrid, "registered hex grid generator: '{0}'", InGenerator);


	return;
}


AExampleHexGrid* UHexGridSubsystem::GetHexGrid() const
{
	return UMCommon::SafelyDereferenceWeakObjectPtr<AExampleHexGrid>(_HexGrid);
}


AExampleHexGrid* UHexGridSubsystem::CreateHexGrid(TSubclassOf<AExampleHexGrid> GeneratorClass, FVector const& WorldLocation)
{
	IF_NULL_RETURN_NULL_LERR(GeneratorClass, HexGrid, "generator class is null");

	UWorld* World = GetWorld();
	IF_NULL_RETURN_NULL_LERR(World, HexGrid, "could not get world");

	FActorSpawnParameters ActorSpawnParams;
	AExampleHexGrid* NewHexGrid = World->SpawnActor<AExampleHexGrid>(GeneratorClass, FTransform(WorldLocation), ActorSpawnParams);
	IF_NULL_RETURN_NULL_LERR(NewHexGrid, HexGrid, "failed to spawn hex grid generator of class '{0}'", GeneratorClass);

	UHexGridSubsystem::RegisterHexGrid(NewHexGrid);


	return NewHexGrid;
}


DEFINE_STATIC_PTR(UHexGridSubsystem, HexGrid, GetHexGrid, AExampleHexGrid);
DEFINE_STATIC_PTR_TwoParams(UHexGridSubsystem, HexGrid, CreateHexGrid, AExampleHexGrid, TSubclassOf<AExampleHexGrid>, GeneratorClass, FVector const&, WorldLocation);