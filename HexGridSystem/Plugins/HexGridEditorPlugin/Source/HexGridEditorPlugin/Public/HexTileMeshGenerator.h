// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridTypes.h"
#include "HexTileMeshGenerator.generated.h"


class UStaticMesh;
class UDynamicMesh;
class UDynamicMeshComponent;
class UGeometryScriptDebug;


// editor-only class that creates a hex tile mesh from selected tile properties
UCLASS(Blueprintable, Transient, NotPlaceable, hideCategories = (
	Actor,
	"Actor Tick",
	Input,
	WorldPartition,
	Events,
	DataLayers,
	Transform,
	ComponentTick,
	Physics,
	Rendering,
	Collision,
	BodySetup,
	Lighting,
	HLOD,
	Navigation,
	VirtualTags,
	Tags,
	ComponentReplication,
	Activation,
	Variable,
	Cooking,
	MaterialParameters,
	TextureStreaming,
	Mobile,
	RayTracing,
	AssetUserData,
	Replication
	)
)
class HEXGRIDEDITORPLUGIN_API AHexTileMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	

	AHexTileMeshGenerator();

	// get tile size
	float GetTileRadius() const { return TileRadius; }


protected:
	
	// button used to create a hex tile static mesh within the editor
	UPROPERTY(EditDefaultsOnly)
		bool GenerateStaticMesh;

	UPROPERTY()
		UDynamicMeshComponent* DynamicMeshComp;

	// size of a hex tile
	UPROPERTY(EditDefaultsOnly)
		float TileRadius;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* HexTileMaterial;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// generates static mesh for a hex tile from a dynamic mesh component and tile size property
	void GenerateHexTileStaticMesh();

private:

	// object that contains debug messages for geometry script dynamic mesh to static mesh conversions
	UPROPERTY(Transient)
		UGeometryScriptDebug* _GeometryScriptDebug;

	void PrintGeometryScriptDebugMessages() const;

	// setup the dynamic mesh used to create hex tiles
	UDynamicMesh* GenerateHexTileDynamicMesh();

	// setup the static mesh used to create hex tiles
	void CreateHexTileStaticMesh(UStaticMesh* OutStaticMesh);

	// dynamically renders a mesh for a hex tile
	void RenderDynamicMeshHexagon(UDynamicMesh* TargetMesh, const float HexTileRadius, const int GroupId);

};
