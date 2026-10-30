
// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HexGridTileTypeAsset.h"
#include "HexGridTypes.h"
#include "ExampleHexGridTileTypeAsset.generated.h"


USTRUCT()
struct HEXGRIDSYSTEM_API FExampleTilePathData
{

	GENERATED_BODY();


	FExampleTilePathData()
		: bIsTraversalAllowed(false)
		, HeuristicCostScalar(1.0f)
		, TravelCostScalar(1.0f)
	{
	}


	FExampleTilePathData(bool InIsTraversalAllowed, const float InHeuristicCostScalar = 1.f, const float InTravelCostScalar = 1.f)
		: bIsTraversalAllowed(InIsTraversalAllowed)
		, HeuristicCostScalar(InHeuristicCostScalar)
		, TravelCostScalar(InTravelCostScalar)
	{
	}

	UPROPERTY(EditAnywhere)
	bool bIsTraversalAllowed;

	UPROPERTY(EditAnywhere)
	float HeuristicCostScalar;

	UPROPERTY(EditAnywhere)
	float TravelCostScalar;

};


USTRUCT()
struct HEXGRIDSYSTEM_API FExampleHexGridTileData : public FExampleTilePathData
{

	GENERATED_BODY();


	FExampleHexGridTileData()
		: FExampleTilePathData(true)
		, Coordinate()
	{
	}

	FExampleHexGridTileData(FHexGridCoordinate const& InCoordinate)
		: FExampleTilePathData(true)
		, Coordinate(InCoordinate)
	{
	}

	FExampleHexGridTileData(FExampleTilePathData const& PathData, FHexGridCoordinate const& InCoordinate)
		: FExampleTilePathData(PathData)
		, Coordinate(InCoordinate)
	{
	}

	UPROPERTY(VisibleAnywhere)
	FHexGridCoordinate Coordinate;


	// A* pathfinding

	// get estimated minimum cost for traveling between two nodes
	// if zero,									-- A* becomes Dijkstra's Algorithm	guaranteed to find shortest path, may run slower
	// if always lower or equal to actual cost	-- still guaranteed to find shortest path
	// if greater than actual cost				-- not guaranteed to find shortest path, but the algorithm may sometimes run faster
	// if much greater than actual cost			-- A* becomes greedy best-first-search, not guaranteed to find shortest path
	// static THexGridPathfindingProperties required function implementations for A* pathfinding
	static FVector::FReal GetHeuristicCost(const FExampleHexGridTileData& StartNode, const FExampleHexGridTileData& EndNode)
	{
		return StartNode.HeuristicCostScalar * EndNode.HeuristicCostScalar * (StartNode.Coordinate - EndNode.Coordinate).Length();
	}

	// scales cost provided by GetHeuristicCost()
	// static THexGridPathfindingProperties required function implementations for A* pathfinding
	static FVector::FReal GetHeuristicScale()
	{
		return 1.f;
	}

	// get actual cost for traveling between two nodes
	// static THexGridPathfindingProperties required function implementations for A* pathfinding
	static FVector::FReal GetTraversalCost(const FExampleHexGridTileData& StartNode, const FExampleHexGridTileData& EndNode)
	{
		return StartNode.TravelCostScalar * EndNode.TravelCostScalar * (StartNode.Coordinate - EndNode.Coordinate).Length();
	}

	// check if an edge exists between two nodes, and thus can be traversed
	// static THexGridPathfindingProperties required function implementations for A* pathfinding
	static bool IsTraversalAllowed(const FExampleHexGridTileData& NodeA, const FExampleHexGridTileData& NodeB)
	{
		return NodeA.Coordinate.IsNeighbor(NodeB.Coordinate) && NodeA.bIsTraversalAllowed && NodeB.bIsTraversalAllowed;
	}

};


/**
 * An object that defines a type of hex tile that can be used in a hex grid and its properties
 */
UCLASS()
class HEXGRIDSYSTEM_API UExampleHexGridTileTypeAsset : public UHexGridTileTypeAsset
{
	GENERATED_BODY()

public:

	UExampleHexGridTileTypeAsset();

	FExampleTilePathData GetTileData() const { return ExampleTilePathData; }

protected:

	UPROPERTY(EditAnywhere)
	FExampleTilePathData ExampleTilePathData;

};
