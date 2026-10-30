// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "HexGridTypes.h"
#include "AssetProviderInterface.h"
#include "HexGridFogOfWarComponent.generated.h"


class AHexGrid;
class UMaterialInstanceDynamic;
class UCanvasRenderTarget2D;
class UTexture;

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnActivePerspectivePlayerIndexChanged, 
	const int /*OldPerspectivePlayerIndex*/, 
	const int /*NewActivePerspectivePlayerIndex*/
);

DECLARE_MULTICAST_DELEGATE_FourParams(
	FOnPlayerVisibilityDataChanged,
	const int /*PerspectivePlayerIndex*/,
	FHexGridCoordinate const& /*Coordinate*/,
	const EHexGridTileVisibility /*OldVisibility*/,
	const EHexGridTileVisibility /*NewVisibility*/
);


/*
* A component for managing fog of war on a grid
*
*/
UCLASS(ClassGroup = (Custom),
	meta =
	(BlueprintSpawnableComponent),
	HideCategories = (
		ComponentTick,
		MeshPainting,
		MaterialCache,
		Rendering,
		Tags,
		ComponentReplication,
		Cooking,
		Variable,
		LOD,
		RayTracing,
		MaterialParameters,
		Replication,
		AssetUserData,
		Object,
		Physics,
		Collision,
		Lighting,
		Navigation,
		VirtualTexture,
		HLOD,
		Mobile,
		TextureStreaming
		)
)
class HEXGRIDPLUGIN_API UHexGridFogOfWarComponent : public UStaticMeshComponent, public IAssetProviderInterface
{
	GENERATED_BODY()

public:

	UHexGridFogOfWarComponent();


	// IAssetProviderInterface implementation
protected:
	virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const override;
	//

public:

	AHexGrid* GetOwnerHexGrid() const;


	// updates and applies visibility state for a group of tiles in the data, then rebuilds the visuals for the active player
	void ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(FHexGridTileVisibilityGroupMap const& TileVisibilityGroups);

	// updates and applies visibility state for a group of tiles in the data, then rebuilds the visuals for the active player
	void ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(FHexGridCoordinateList const& Coordinates, const EHexGridTileVisibility Visibility)
	{
		ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(FHexGridTileVisibilityGroupMap(Coordinates, Visibility));
	}

	// updates and applies visibility state for a group of tiles in the data, then rebuilds the visuals for the active player
	void ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(TMap<FHexGridCoordinate, EHexGridTileVisibility> const& TileVisibilityMap)
	{
		ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(FHexGridTileVisibilityGroupMap(TileVisibilityMap));
	}


	// updates visibility state for a group of tiles in the data, then rebuilds the visuals for the given player
	void UpdateVisibilityAtCoordinatesForPerspectivePlayer(const int PerspectivePlayerIndex, FHexGridTileVisibilityGroupMap const& TileVisibilityGroups);

	// updates visibility state for a group of tiles in the data, then rebuilds the visuals for the given player
	void UpdateVisibilityAtCoordinatesForPerspectivePlayer(const int PerspectivePlayerIndex, FHexGridCoordinateList const& Coordinates, const EHexGridTileVisibility Visibility)
	{
		UpdateVisibilityAtCoordinatesForPerspectivePlayer(PerspectivePlayerIndex, FHexGridTileVisibilityGroupMap(Coordinates, Visibility));
	}

	// updates visibility state for a group of tiles in the data, then rebuilds the visuals for the given player
	void UpdateVisibilityAtCoordinatesForPerspectivePlayer(const int PerspectivePlayerIndex, TMap<FHexGridCoordinate, EHexGridTileVisibility> const& TileVisibilityMap)
	{
		UpdateVisibilityAtCoordinatesForPerspectivePlayer(PerspectivePlayerIndex, FHexGridTileVisibilityGroupMap(TileVisibilityMap));
	}


	// set the given player index to be the current active perspective
	// loads and applies the new player perspective
	void SetActivePerspectivePlayer(const int PlayerIndex);

	// reload the current active player perspective
	void ReloadActivePerspectivePlayer();

	// get the index of the current active perspective player
	int GetActivePerspectivePlayerIndex() const { return _ActivePerspectivePlayerIndex; }

	// check if there is a valid active perspective player index
	bool HasActivePersepectivePlayer() const;

	// delegate broadcasts when the active player perspective index changes
	FOnActivePerspectivePlayerIndexChanged OnActivePerspectivePlayerIndexChanged;


	// get tile visibility data for the given player
	EHexGridTileVisibility GetTileVisibilityForPlayer(const int PlayerIndex, FHexGridCoordinate const& Coordinate) const;

	// get tile visibility data for the active perspective player
	EHexGridTileVisibility GetTileVisibilityForActivePlayer(FHexGridCoordinate const& Coordinate) const;

	// set tile visibility data for the given player
	// this does not change current active visibility, only effects underlying data
	void SetTileVisibilityForPlayer(
		const int PlayerIndex,
		FHexGridCoordinate const& Coordinate,
		const EHexGridTileVisibility VisibilityState
	);

	// delegate broadcasts when player visibility data changes
	FOnPlayerVisibilityDataChanged OnPlayerVisibilityDataChanged;

	// get all tile coordinates with the given visibility for the given player
	// NOTE: only adds coordinates that are listed in the map, unlisted tiles are assumed hidden but wont be added
	void GetListedTilesWithVisibilityForPlayer(
		const int PlayerIndex,
		const EHexGridTileVisibility VisibilityState,
		FHexGridCoordinateList& OutCoordinates
	) const;

	// get a list of tiles with the given visibility for the current active perspective player
	// NOTE: only adds coordinates that are listed in the map, unlisted tiles are assumed hidden but wont be added
	void GetListedTilesWithVisibilityForActivePlayer(
		const EHexGridTileVisibility VisibilityState,
		FHexGridCoordinateList& OutCoordinates
	) const;

	// initialize the fog of war
	void InitializeFogOfWar(FFogOfWarSetupProperties const& SetupProperties);

	// register the player to the fog of war visibility data
	void RegisterPlayerPerspective(const int PlayerIndex);

	// set the color of the fog of war
	void SetFogColor(FLinearColor const& FogColor_Hidden, FLinearColor const& FogColor_Explored);

	// get fog of war perspectives for all players
	void GetFogOfWarPerspectives(FFogOfWarPerspectiveMap& OutData) const;

	// set fog of war perspectives for all players
	void SetFogOfWarPerspectives(FFogOfWarPerspectiveMap const& FogOfWarData);

	// set perspective data for the given player
	// this does not change current active visibility, only effects underlying data
	void SetPlayerPerspectiveData(const int PlayerIndex, FFogOfWarPerspectiveData const& PerspectiveData);

	// get perspective data for the given player
	FFogOfWarPerspectiveData GetPlayerPerspectiveData(const int PlayerIndex) const;


	// capture current state of the fog thru render target raw pixel data
	void ReadRawFogOfWarPixelVisibilityData(TArray<FColor>& OutShroudPixelData, TArray<FColor>& OutLosPixelData) const;

	// set current state of the fog thru render target raw pixel data
	void ApplyRawFogOfWarPixelVisibilityData(TArray<FColor> const& ShroudPixelData, TArray<FColor> const& LosPixelData);


	// the name of the color parameter in the fog material for hidden tiles
	static const FName c_RenderTargetParameterName_FogColor_Hidden;

	// the name of the color parameter in the fog material for explored tiles
	static const FName c_RenderTargetParameterName_FogColor_Explored;

	// the name of the texture parameter in the fog material for the explored map
	static const FName c_RenderTargetParameterName_FogMask_Explored;

	// the name of the texture parameter in the fog material for the previous explored map
	static const FName c_RenderTargetParameterName_FogMask_Explored_Previous;

	// the name of the texture parameter in the fog material for the line of sight map
	static const FName c_RenderTargetParameterName_FogMask_LineOfSight;

	// the name of the texture parameter in the fog material for the previous line of sight map
	static const FName c_RenderTargetParameterName_FogMask_LineOfSight_Previous;

	// the name of the texture parameter in the fog material for the fog fade alpha
	static const FName c_RenderTargetParameterName_FogFadeAlpha;

	// the name of the texture parameter in the fog material for the copy texture
	static const FName c_RenderTargetParameterName_CopyTexture;

	// material slot index for the fog material
	static const int c_FogMaterialIndex;

	// the name of the texture parameter in the fog material
	static const float c_SizePadding_ZOffset;

	// offset radius for the tile reveal brush relative to the grid tile radius
	static const float c_GridTileRadiusRevealBrushOffset;

	// check if the given player index is valid
	bool IsValidPlayerIndex(const int PlayerIndex) const
	{
		return _FogOfWarData.IsValidPlayerIndex(PlayerIndex);
	}

	// get the player perspective data reference for the given player index
	FFogOfWarPerspectiveData& GetPlayerPerspectiveDataReference(const int PlayerIndex)
	{
		return _FogOfWarData.GetPlayerPerspectiveDataReference(PlayerIndex);
	}

	// sets the size and position of the fog plane to cover the given actor
	void TransformToFitBounds(FBox2D const& Bounds);

	// get the properties of the fog of war
	FFogOfWarProperties GetFogOfWarProperties() const { return FogOfWarProperties; }

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// a grayscale image of a circle with very soft, feathered edges (a radial gradient)
	// determines shape and sharpness of revealed areas
	// a perfect white circle on black background would cut the fog with perfect sharpness
	UPROPERTY(EditAnywhere, Category = "Fog of War")
	TSoftObjectPtr<UTexture2D> RevealBrush;

	// the resolution of the render targets used to render fog and line of sight
	UPROPERTY(EditAnywhere, Category = "Fog of War")
	FIntPoint RenderTargetResolution;

	// fog of war properties
	UPROPERTY(EditAnywhere, Category = "Fog of War")
	FFogOfWarProperties FogOfWarProperties;

	// called when player tile visibility data is changed
	virtual void HandlePlayerTileVisibilityDataChanged(
		const int PlayerIndex,
		FHexGridCoordinate const& Coordinate,
		const EHexGridTileVisibility OldVisibilityState,
		const EHexGridTileVisibility NewVisibilityState
	);

private:

	// canvas render target that acts as our dynamic fog map for the explored area shroud
	UPROPERTY()
	UCanvasRenderTarget2D* _FogRenderTarget;

	// canvas render target that acts as our dynamic fog map for line of sight
	UPROPERTY()
	UCanvasRenderTarget2D* _LineOfSightRenderTarget;

	// setup the render target objects
	void SetupRenderTargets();

	// dynamic material instance for the fog plane
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* _FogMaterialInstanceDynamic;

	// initialize the fog material with a dynamic material instance
	void InitializeFogMaterial(FFogOfWarSetupProperties const& SetupProperties);

	// initialize the fog of war to its owner grid
	void InitializeToGrid();

	// get the owning grid's tile radius
	float GetGridTileRadius() const;

	// data that represents the state of the fog of war
	UPROPERTY()
	FFogOfWarPerspectiveMap _FogOfWarData;

	// player index for the active perspective
	UPROPERTY()
	int _ActivePerspectivePlayerIndex;

	// rebuild the perspective of the given player index for both render targets
	void RenderPlayerPerspective(const int PlayerIndex);

	// render the permanent 'shroud' (hidden and partial tiles) to the FogRenderTarget
	void RenderShroud(const FHexGridTileVisibilityGroupMap& TileVisibilityGroups);

	// render the transient 'shroud' (visible tiles) to the LineOfSightRenderTarget
	void RenderLineOfSight(const FHexGridTileVisibilityGroupMap& TileVisibilityGroups);

	// clear the persistent FogRenderTarget to black
	void ClearShroud();

	// clear the transient LineOfSightRenderTarget to black
	void ClearLineOfSight();

	// capture current state of a render target's raw pixel data
	static void ReadRenderTargetRawPixelVisibilityData(const UObject* WorldContextObject, UCanvasRenderTarget2D* RenderTarget, TArray<FColor>& OutRawPixelVisibilityData);

	// set current state of a render target's raw pixel data
	static void ApplyRenderTargetRawPixelVisibilityData(const UObject* WorldContextObject, UCanvasRenderTarget2D* RenderTarget, TArray<FColor> const& RawPixelVisibilityData);

	// handles lerping the fog transition over time
	void OnTick_RenderFadingTiles(float DeltaSeconds);

	// set the material parameter for fog fade alpha
	void SetFogFadeAlpha(float Alpha);

	// update the fog texture parameters
	void UpdateFogTextureParameters();

	// performs a draw call on the given target with a given brush and coordinate
	void DrawTextureToTarget(
		UCanvasRenderTarget2D* Target,
		const FHexGridCoordinate& Coordinate,
		const FLinearColor& TintColor
	);

	// map of tiles that are actively being faded in for the shroud map
	UPROPERTY(Transient)
	TMap<FHexGridCoordinate, float> _FadingIn_Shroud;

	// map of tiles that are actively being faded out for the shroud map
	UPROPERTY(Transient)
	TMap<FHexGridCoordinate, float> _FadingOut_Shroud;

	// map of tiles that are actively being faded in for the line of sight map
	UPROPERTY(Transient)
	TMap<FHexGridCoordinate, float> _FadingIn_LineOfSight;

	// map of tiles that are actively being faded out for the line of sight map
	UPROPERTY(Transient)
	TMap<FHexGridCoordinate, float> _FadingOut_LineOfSight;

};
