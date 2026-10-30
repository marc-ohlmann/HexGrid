// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "HexGridTypes.h"
#include "UObject/GCObject.h"


class UHexGridDataAsset;
class UHexGridTileTypeAsset;
class UInstancedStaticMeshComponent;
class FHexGridEditorToolkit;
class SEditorViewport;


/*
 *
 */
class FHexGridEditorViewportClient : public FEditorViewportClient
{
public:
    /** Constructor */
    FHexGridEditorViewportClient(
        const TSharedRef<FHexGridEditorToolkit>& InToolkit,
        const TSharedRef<FAdvancedPreviewScene>& InPreviewScene
    );

    /** Destructor */
    ~FHexGridEditorViewportClient();

    // FEditorViewportClient interface
    virtual void Tick(float DeltaSeconds) override; 
    virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
    virtual void ReceivedFocus(FViewport* InViewport) override;
    virtual void LostFocus(FViewport* InViewport) override;


    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override { return TEXT("FHexGridEditorViewportClient"); }

    /** Rebuilds the grid display from the asset's data. */
    void RedrawGrid();

    // register a new tile type to render
    void RegisterTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType);

private:

    // The preview scene where we will render our grid
    FAdvancedPreviewScene& PreviewScene;

    /** The asset being edited. */
    TObjectPtr<UHexGridDataAsset> HexGridAsset;

    // map of tile type assets to their associated instanced static mesh components
    TMap<TSoftObjectPtr<const UHexGridTileTypeAsset>, TObjectPtr<UInstancedStaticMeshComponent>> _TileTypeIsmComponentMap;

    // setup the given instanced static mesh component for a given tile type
    void SetupTileIsmc(UInstancedStaticMeshComponent* Ismc, TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType);

    // get the ismc for the given tile type
    UInstancedStaticMeshComponent* GetTileTypeIsmc(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const;

    // get the ismc for the given tile type index
    UInstancedStaticMeshComponent* GetTileTypeIsmc(const int TileTypeIndex) const;

    // clear all rendered tiles
    void ClearRenderedTiles();

    // remove the given tile type
    void RemoveTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType);

    // clear all tile types
    void ClearTileTypes(bool bRemoveEmptyTileType);

    // called when a visualization property changes on the asset
    void HandleAssetVisualizationPropertyChanged();

    // delegate handle to asset visualization property changed delegate
    FDelegateHandle _AssetVisualizationPropertyChangedDelegateHandle;

    // handles redrawing the grid on every frame
    void OnTick_RedrawGrid(float DeltaTime);

    // flag indicates if the grid needs to be redrawn
    bool _bShouldRedrawGrid;

    // minimum time, in seconds, to periodically redraw the grid
    static const float c_MinRedrawGridPeriodicDurationSeconds;

    // buffer that accumulates time to trigger redraws
    float _RedrawGridBuffer;

    // flag indicates if a paintbrush started and is in progress
    bool _bIsPaintBrushInProgress;

    // the current painted coordinate set
    FHexGridCoordinateList _PaintedCoordinates;

    // called when a paintbrush tool started
    void HandlePaintBrushStarted(FHexGridCoordinateList const& ChangedCoordinates);

    // delegate handle to paintbrush tool started delegate
    FDelegateHandle _PaintBrushStartedDelegateHandle;

    // called when a paintbrush tool ended
    void HandlePaintBrushEnded(FHexGridCoordinateList const& ChangedCoordinates);

    // delegate handle to paintbrush tool ended delegate
    FDelegateHandle _PaintBrushEndedDelegateHandle;

    // change the given set of tiles toa new type
    void ChangeTilesToNewType(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex);

    // delegate handle to asset map changes delegate
    FDelegateHandle _AssetTileMapChangedDelegateHandle;

    // map of coordinate type and hism instance data
    TMap<FHexGridCoordinate, FHexGridTileInstanceData> _CoordinateInstanceDataMap;

    // get a world transform for the given coordinate
    FTransform GetCoordinateWorldTransform(FHexGridCoordinate const& Coordinate);

    // clears all rendered tiles of the given type
    void ClearRenderedTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType);

    // clears all rendered tiles from the empty tile set
    void ClearRenderedEmptyTiles() { ClearRenderedTileType(nullptr); }

    // redraw the empty tile set, optionally invalidating the viewport
    void RedrawEmptyTiles(bool bShouldInvalidate);

    // called when a tile map change occurs
    void HandleAssetTileMapChanged(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex);

    // collect brushed coordinates
    void CollectBrushedCoordinates(FHexGridCoordinateList const& BrushedCoordinates);

    // flag indicates if a tile change was collected during paint
    bool _bChangeCollectedDuringPaint_Tile;

    // collect coordinates for a tile change
    void CollectTileMapChange(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex);

    // the tile type index for the current map change
    int _MapChangeTileTypeIndex;

    // set custom data values for the given tile coordinates at the given custom data index
    void SetTileGroupHighlightData(
        FHexGridCoordinateList const& TileCoordinates, 
        FLinearColor const& HighlightColor, 
        const float HighlightIntensity, 
        const float HighlightRainbowBlend
    );

    // the last known target coordinates of the brush
    FHexGridCoordinateList _BrushTargetCoordinates;

    // called when the target changes
    void HandleBrushTargetsChanged(FHexGridCoordinateList const& BrushTargetCoordinates);

    // delegate handle to asset target changed delegate
    FDelegateHandle _BrushTargetsChangedDelegateHandle;

    // map of tile type assets to their associated dynamic  materials
    TMap<TSoftObjectPtr<const UHexGridTileTypeAsset>, TObjectPtr<UMaterialInstanceDynamic>> _TileTypeDynamicMaterialMap;

    // highlight color used for the brush tiles
    static const FLinearColor c_BrushHighlightColor;


};