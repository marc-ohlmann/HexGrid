// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HexGridTypes.h"
#include "AssetProviderInterface.h"
#include "HexGridDataAsset.generated.h"


#if WITH_EDITORONLY_DATA

DECLARE_MULTICAST_DELEGATE(FVisualizationChange);
DECLARE_MULTICAST_DELEGATE_OneParam(FVisualizationPaintBrushChange, FHexGridCoordinateList const& /*BrushTargetCoordinates*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FVisualizationTileMapChange, FHexGridCoordinateList const& /*ChangedCoordinates*/, const int /*TileTypeIndex*/);

#endif


/*
* An asset that represents the state of a grid.
*
* Includes custom editor functionality to help create grid states.
*/
UCLASS(BlueprintType)
class HEXGRIDPLUGIN_API UHexGridDataAsset : public UDataAsset, public IAssetProviderInterface
{
    GENERATED_BODY()

public:

    UHexGridDataAsset();

    // IAssetProviderInterface implementation
protected:

    virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const override;

    //

public:

    // Radial size of each tile
    UPROPERTY(EditAnywhere, Category = "Hex Grid Data")
    FHexGridData GridData;

#if WITH_EDITORONLY_DATA

    // shape of the grid used to render empty tiles in the editor
    UPROPERTY(EditAnywhere, Category = "Hex Grid Visualization")
    EHexGridShape VisualizationGridShape;

    // size of the grid used to render empty tiles in the editor
    UPROPERTY(EditAnywhere, Category = "Hex Grid Visualization")
    int VisualizationGridSize;

    // get the tile coordinates for all tiles that are empty but are included for visualization
    void GetEmptyTileCoordinatesForVisualization(FHexGridCoordinateList& OutCoordinates) const;

    // the tile asset used to render empty tiles
    UPROPERTY(EditAnywhere, Category = "Hex Grid Visualization")
    TSoftObjectPtr<const UHexGridTileTypeAsset> VisualizationEmptyTileAsset;

    // delegate broadcasts when a visualization property changes
    FVisualizationChange OnVisualizationPropertyChanged;

    // broadcasts a visualization property change
    void BroadcastVisualizationChange()
    {
        OnVisualizationPropertyChanged.Broadcast();
    }

    // delegate broadcasts when a paintbrush change is started
    FVisualizationPaintBrushChange OnPaintBrushStarted;

    // broadcasts a paintbrush change started
    void BroadcastPaintBrushStarted(FHexGridCoordinateList const& ChangedCoordinates)
    {
        OnPaintBrushStarted.Broadcast(ChangedCoordinates);
    }

    // delegate broadcasts when a paintbrush change is ended
    FVisualizationPaintBrushChange OnPaintBrushEnded;

    // broadcasts a paintbrush change ended
    void BroadcastPaintBrushEnded(FHexGridCoordinateList const& ChangedCoordinates)
    {
        OnPaintBrushEnded.Broadcast(ChangedCoordinates);
    }

    // delegate broadcasts when the brush target coordinates change
    FVisualizationPaintBrushChange OnBrushTargetsChanged;

    // broadcasts a paintbrush change ended
    void BroadcastBrushTargetsChanged(FHexGridCoordinateList const& BrushCoordinates)
    {
        OnBrushTargetsChanged.Broadcast(BrushCoordinates);
    }

    // delegate broadcasts when the tile map changed
    FVisualizationTileMapChange OnTileMapChanged;

    // broadcasts a tile map change
    void BroadcastTileMapChanged(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex)
    {
        OnTileMapChanged.Broadcast(ChangedCoordinates, TileTypeIndex);
    }

#endif

protected:

#if WITH_EDITOR

    virtual void PostEditUndo() override;
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

};