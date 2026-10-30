// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/HexGridEditorViewportClient.h"

#include "MCommon.h"
#include "HexGridDataAssetEditor/HexGridEditorToolkit.h"
#include "HexGridDataAsset.h"
#include "HexGridTileTypeAsset.h"
#include "HexGridDataAssetEditor/SHexGridEditorViewport.h"
#include "HexGridDataAssetEditor/HexGridEditorMode.h"
#include "ColorTypes.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "AssetViewerSettings.h" // For scene lighting profiles
#include "SEditorViewport.h"
#include "EditorModeManager.h"


const float FHexGridEditorViewportClient::c_MinRedrawGridPeriodicDurationSeconds = 0.2f;
const FLinearColor FHexGridEditorViewportClient::c_BrushHighlightColor = FLinearColor::Blue;


FHexGridEditorViewportClient::FHexGridEditorViewportClient(
    const TSharedRef<FHexGridEditorToolkit>& InToolkit,
    const TSharedRef<FAdvancedPreviewScene>& InPreviewScene
)
    : FEditorViewportClient(&InToolkit->GetEditorModeManager(), &InPreviewScene.Get(), StaticCastSharedPtr<SEditorViewport>(InToolkit->GetViewportWidget()))
    , PreviewScene(InPreviewScene.Get())
    , HexGridAsset(InToolkit->GetAsset())
    , _TileTypeIsmComponentMap()
    , _AssetVisualizationPropertyChangedDelegateHandle()
    , _bShouldRedrawGrid(false)
    , _RedrawGridBuffer(0.f)
    , _bIsPaintBrushInProgress(false)
    , _PaintedCoordinates()
    , _PaintBrushStartedDelegateHandle()
    , _PaintBrushEndedDelegateHandle()
    , _AssetTileMapChangedDelegateHandle()
    , _CoordinateInstanceDataMap()
    , _bChangeCollectedDuringPaint_Tile(false)
    , _MapChangeTileTypeIndex(-1)
    , _BrushTargetCoordinates()
    , _BrushTargetsChangedDelegateHandle()
    , _TileTypeDynamicMaterialMap()
{
    // --- Initial Setup ---
    SetRealtime(true); // Enable realtime updates
    EngineShowFlags.SetGrid(false); // Hide the editor's default grid
    PreviewScene.SetFloorVisibility(false);
    SetViewLocation(FVector(-1000.f, 0.f, 2500.f)); // Set starting camera position
    SetViewRotation(FRotator(-40.f, 0.f, 0.f)); // Set starting camera position

    // bind to asset visualization changes
    if (HexGridAsset)
    {
        _AssetVisualizationPropertyChangedDelegateHandle = HexGridAsset.Get()->OnVisualizationPropertyChanged.AddRaw(this, &FHexGridEditorViewportClient::HandleAssetVisualizationPropertyChanged);
        _PaintBrushStartedDelegateHandle = HexGridAsset.Get()->OnPaintBrushStarted.AddRaw(this, &FHexGridEditorViewportClient::HandlePaintBrushStarted);
        _PaintBrushEndedDelegateHandle = HexGridAsset.Get()->OnPaintBrushEnded.AddRaw(this, &FHexGridEditorViewportClient::HandlePaintBrushEnded);
        _AssetTileMapChangedDelegateHandle = HexGridAsset.Get()->OnTileMapChanged.AddRaw(this, &FHexGridEditorViewportClient::HandleAssetTileMapChanged);
        _BrushTargetsChangedDelegateHandle = HexGridAsset.Get()->OnBrushTargetsChanged.AddRaw(this, &FHexGridEditorViewportClient::HandleBrushTargetsChanged);
    }

    // start loading tile assets before drawing the grid
    RegisterTileType(nullptr);
    RedrawGrid();
}


FHexGridEditorViewportClient::~FHexGridEditorViewportClient()
{
    if (HexGridAsset)
    {
        HexGridAsset.Get()->OnVisualizationPropertyChanged.Remove(_AssetVisualizationPropertyChangedDelegateHandle);
        HexGridAsset.Get()->OnPaintBrushStarted.Remove(_PaintBrushStartedDelegateHandle);
        HexGridAsset.Get()->OnPaintBrushEnded.Remove(_PaintBrushEndedDelegateHandle);
        HexGridAsset.Get()->OnTileMapChanged.Remove(_AssetTileMapChangedDelegateHandle);
        HexGridAsset.Get()->OnBrushTargetsChanged.Remove(_BrushTargetsChangedDelegateHandle);
    }
    _AssetVisualizationPropertyChangedDelegateHandle.Reset();
    _PaintBrushStartedDelegateHandle.Reset();
    _PaintBrushEndedDelegateHandle.Reset();
    _AssetTileMapChangedDelegateHandle.Reset();
    _BrushTargetsChangedDelegateHandle.Reset();
}


void FHexGridEditorViewportClient::Tick(float DeltaSeconds)
{
    FEditorViewportClient::Tick(DeltaSeconds);

    OnTick_RedrawGrid(DeltaSeconds);


    return;
}


void FHexGridEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
    PreviewScene.AddReferencedObjects(Collector);

    Collector.AddReferencedObject(HexGridAsset);

    TArray<TObjectPtr<UInstancedStaticMeshComponent>> IsmComponents;
    _TileTypeIsmComponentMap.GenerateValueArray(IsmComponents);
    FOR_FORWARD(i, IsmComponents.Num())
    {
        Collector.AddReferencedObject(IsmComponents[i]);
    }

    TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
    _TileTypeDynamicMaterialMap.GenerateValueArray(DynamicMaterials);
    FOR_FORWARD(i, DynamicMaterials.Num())
    {
        Collector.AddReferencedObject(DynamicMaterials[i]);
    }
}


FTransform FHexGridEditorViewportClient::GetCoordinateWorldTransform(FHexGridCoordinate const& Coordinate)
{
    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_TYPE_LERR(DataAsset, FTransform, HexGridEditor, "DataAsset is null");

    const FVector TileLocation = DataAsset->GridData.GetTileCoordinateWorldOffsetLocation(Coordinate);


    return FTransform(FRotator::ZeroRotator, TileLocation, FVector::OneVector);
}


void FHexGridEditorViewportClient::SetupTileIsmc(UInstancedStaticMeshComponent* Ismc, TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType)
{
    IF_NULL_RETURN_LERR(Ismc, HexGridEditor, "Ismc isn ull");

    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    const UHexGridTileTypeAsset* TileTypeAsset = nullptr;
    if (TileType.IsNull())
    {
        TileTypeAsset = DataAsset->VisualizationEmptyTileAsset.LoadSynchronous();
    }
    else
    {
        TileTypeAsset = TileType.LoadSynchronous();
    }
    IF_NULL_RETURN_LERR(TileTypeAsset, HexGridEditor, "TileTypeAsset is null");

    // load all dependency assets and hold them until the ismc can be setup
    TArray<UObject*> TileTypeAssetDependencies;
    IAssetProviderInterface::SyncLoadAssetProviderRecursively(TileTypeAsset, TileTypeAssetDependencies);

    UMaterialInstanceDynamic* IsmcMid = UHexGridTileTypeAsset::SetupTileIsmComponentMaterials(GetTransientPackage(), Ismc, TileTypeAsset);
    IF_NULL_RETURN_LERR(IsmcMid, HexGridEditor, "IsmcMid is null");

    if (_TileTypeDynamicMaterialMap.Contains(TileType))
    {
        _TileTypeDynamicMaterialMap[TileType] = IsmcMid;
    }
    else
    {
        _TileTypeDynamicMaterialMap.Add(TileType, IsmcMid);
    }


    return;
}


void FHexGridEditorViewportClient::RedrawEmptyTiles(bool bShouldInvalidate)
{
    ClearRenderedEmptyTiles();

    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    UInstancedStaticMeshComponent* EmptyTileTypeIsmc = GetTileTypeIsmc(nullptr);
    IF_NULL_RETURN_LERR(EmptyTileTypeIsmc, HexGridEditor, "TileIsmc[TileType: 'nullptr'] is null");

    FHexGridCoordinateList VisualizedEmptyCoordinates;
    DataAsset->GetEmptyTileCoordinatesForVisualization(VisualizedEmptyCoordinates);

    SetupTileIsmc(EmptyTileTypeIsmc, nullptr);

    FOR_FORWARD(i, VisualizedEmptyCoordinates.Num())
    {
        const FTransform TileTransform = GetCoordinateWorldTransform(VisualizedEmptyCoordinates[i]);
        const int TileIsmcIndex = EmptyTileTypeIsmc->AddInstance(TileTransform);
        _CoordinateInstanceDataMap.Add(VisualizedEmptyCoordinates[i], FHexGridTileInstanceData(-1, TileIsmcIndex));
    }

    if (bShouldInvalidate)
    {
        // Force the viewport to redraw with the new data
        Invalidate();
    }


    return;
}


void FHexGridEditorViewportClient::RedrawGrid()
{
    // Clear all tiles before redrawing
    ClearTileTypes(false);

    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    // register tile type ismc's
    FOR_FORWARD(i, DataAsset->GridData.GridProperties.TileTypes.Num())
    {
        RegisterTileType(DataAsset->GridData.GridProperties.TileTypes[i]);
    }

    // Iterate through the asset data and add an instance for each tile
    FHexGridCoordinateList TileCoordinates;
    DataAsset->GridData.TileMap.GenerateKeyArray(TileCoordinates.List);
    FOR_FORWARD(i, TileCoordinates.Num())
    {
        const TObjectPtr<const UHexGridTileTypeAsset> TileType = DataAsset->GridData.GetTileTypeFromCoordinate(TileCoordinates[i]).LoadSynchronous();

        UInstancedStaticMeshComponent* TileTypeIsmc = GetTileTypeIsmc(TileType);
        IF_NULL_CONTINUE_LERR(TileTypeIsmc, HexGridEditor, "TileIsmc[TileType: '{0}'] is null", TileType);

        SetupTileIsmc(TileTypeIsmc, TileType);

        const FTransform TileTransform = GetCoordinateWorldTransform(TileCoordinates[i]);
        const int TileIsmcIndex = TileTypeIsmc->AddInstance(TileTransform);

        const int TileTypeIndex = DataAsset->GridData.GetTileTypeIndex(TileType);
        _CoordinateInstanceDataMap.Add(TileCoordinates[i], FHexGridTileInstanceData(TileTypeIndex, TileIsmcIndex));

    }

    // render the empty tile set for visualization
    // and force the viewport to redraw with the new data
    RedrawEmptyTiles(true);


    return;
}


bool FHexGridEditorViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
    if (ModeTools->InputKey(this, EventArgs.Viewport, EventArgs.Key, EventArgs.Event))
    {
        // If the tool handled the input, we are done. Return true.
        return true;
    }

    // The Super call is what passes the input to the FEditorModeTools and your active tool.
    return FEditorViewportClient::InputKey(EventArgs);
}


void FHexGridEditorViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
    // The Super call is what passes mouse clicks to the FEditorModeTools and your active tool.
    FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
    return;
}

void FHexGridEditorViewportClient::ReceivedFocus(FViewport* InViewport)
{
    // Crucially, call the parent implementation.
    // This tells the FEditorModeTools that this viewport now has focus.
    FEditorViewportClient::ReceivedFocus(InViewport);
}


void FHexGridEditorViewportClient::LostFocus(FViewport* InViewport)
{
    // Crucially, call the parent implementation.
    // This tells the FEditorModeTools that this viewport has lost focus.
    FEditorViewportClient::LostFocus(InViewport);
}


UInstancedStaticMeshComponent* FHexGridEditorViewportClient::GetTileTypeIsmc(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType) const
{
    if (_TileTypeIsmComponentMap.Contains(TileType) == false)
    {
        return nullptr;
    }


    return _TileTypeIsmComponentMap[TileType].Get();
}


UInstancedStaticMeshComponent* FHexGridEditorViewportClient::GetTileTypeIsmc(const int TileTypeIndex) const
{
    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_VAL_LERR(DataAsset, nullptr, HexGridEditor, "DataAsset is null");

    if (UMCommon::IsValidIndex(DataAsset->GridData.GridProperties.TileTypes, TileTypeIndex) == false)
    {
        return GetTileTypeIsmc(nullptr);
    }


    return GetTileTypeIsmc(DataAsset->GridData.GridProperties.TileTypes[TileTypeIndex]);
}


void FHexGridEditorViewportClient::ClearRenderedTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType)
{
    UInstancedStaticMeshComponent* TileTypeIsmc = GetTileTypeIsmc(TileType);
    IF_NULL_RETURN_LERR(TileTypeIsmc, HexGridEditor, "TileTypeIsmc[TileType: '{0}'] is null", TileType);

    TileTypeIsmc->ClearInstances();


    return;
}


void FHexGridEditorViewportClient::ClearRenderedTiles()
{
    TArray<TSoftObjectPtr<const UHexGridTileTypeAsset>> TileTypes;
    _TileTypeIsmComponentMap.GenerateKeyArray(TileTypes);
    FOR_FORWARD(i, TileTypes.Num())
    {
        ClearRenderedTileType(TileTypes[i]);
    }


    return;
}


void FHexGridEditorViewportClient::RegisterTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType)
{
    UInstancedStaticMeshComponent* NewTileMeshComponent = NewObject<UInstancedStaticMeshComponent>(GetTransientPackage());
    IF_NULL_RETURN_LERR(NewTileMeshComponent, HexGridEditor, "NewTileMeshComponent is null");

    SetupTileIsmc(NewTileMeshComponent, TileType);

    PreviewScene.AddComponent(NewTileMeshComponent, FTransform::Identity);

    _TileTypeIsmComponentMap.Add(TileType, NewTileMeshComponent);


    return;
}


void FHexGridEditorViewportClient::RemoveTileType(TSoftObjectPtr<const UHexGridTileTypeAsset> const& TileType)
{
    if (_TileTypeIsmComponentMap.Contains(TileType) == false)
    {
        return;
    }

    UInstancedStaticMeshComponent* ComponentToRemove = _TileTypeIsmComponentMap[TileType].Get();
    if (UMCommon::IsUObjectPointerSafe(ComponentToRemove))
    {
        // Remove the component from the scene before removing it from the map
        PreviewScene.RemoveComponent(ComponentToRemove);
    }

    _TileTypeIsmComponentMap.Remove(TileType);

    if (_TileTypeDynamicMaterialMap.Contains(TileType))
    {
        _TileTypeDynamicMaterialMap.Remove(TileType);
    }


    return;
}


void FHexGridEditorViewportClient::ClearTileTypes(bool bRemoveEmptyTileType)
{
    // clear rendered tiles first
    ClearRenderedTiles();

    // remove all components except the empty tile type
    TArray<TSoftObjectPtr<const UHexGridTileTypeAsset>> TileTypes;
    _TileTypeIsmComponentMap.GenerateKeyArray(TileTypes);
    FOR_FORWARD(i, TileTypes.Num())
    {
        if (bRemoveEmptyTileType || TileTypes[i].IsNull() == false)
        {
            RemoveTileType(TileTypes[i]);
        }
    }


    return;
}


void FHexGridEditorViewportClient::HandleAssetVisualizationPropertyChanged()
{
    _bShouldRedrawGrid = true;


    return;
}


void FHexGridEditorViewportClient::OnTick_RedrawGrid(float DeltaSeconds)
{
    if (UMCommon::AccumulateBuffer(_RedrawGridBuffer, DeltaSeconds, c_MinRedrawGridPeriodicDurationSeconds) &&
        _bShouldRedrawGrid)
    {
        _bShouldRedrawGrid = false;

        RedrawGrid();
    }


    return;
}


void FHexGridEditorViewportClient::ChangeTilesToNewType(FHexGridCoordinateList const& ChangedCoordinates, const int NewTileTypeIndex)
{
    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    // --- STAGE 1: Identify every tile type affected by this change ---
    TSet<int32> AffectedTypeIndices;
    if (NewTileTypeIndex >= 0)
    {
        AffectedTypeIndices.Add(NewTileTypeIndex);
    }

    FOR_FORWARD(i, ChangedCoordinates.Num())
    {
        if (const FHexGridTileInstanceData* OldDataPtr = _CoordinateInstanceDataMap.Find(ChangedCoordinates[i]))
        {
            AffectedTypeIndices.Add(OldDataPtr->TileTypeId);
        }
    }

    // --- STAGE 2: Clear the visuals and tracking data for ONLY the affected types ---
    // Clear the ISMC instances for each affected type.
    for (int32 TypeId : AffectedTypeIndices)
    {
        if (UInstancedStaticMeshComponent* IsmcToClear = GetTileTypeIsmc(TypeId))
        {
            IsmcToClear->ClearInstances();
        }
    }

    // Remove any entry from our tracking map that belongs to an affected type.
    // We must do this in two steps to avoid modifying the map while iterating it.
    FHexGridCoordinateList CoordsToRemove;
    for (const TPair<FHexGridCoordinate, FHexGridTileInstanceData>& Pair : _CoordinateInstanceDataMap)
    {
        if (AffectedTypeIndices.Contains(Pair.Value.TileTypeId))
        {
            CoordsToRemove.Add(Pair.Key);
        }
    }
    for (const FHexGridCoordinate& Coord : CoordsToRemove.List)
    {
        _CoordinateInstanceDataMap.Remove(Coord);
    }

    // --- STAGE 3: Rebuild ONLY the affected tile types from the asset's ground truth ---
    // Iterate through the entire asset map once.
    FHexGridCoordinateList TileMapCoordinates;
    DataAsset->GridData.TileMap.GenerateKeyArray(TileMapCoordinates.List);
    FOR_FORWARD(i, TileMapCoordinates.Num())
    {
        const FHexGridCoordinate& Coordinate = TileMapCoordinates[i];
        const TObjectPtr<const UHexGridTileTypeAsset> TileType = DataAsset->GridData.GetTileTypeFromCoordinate(Coordinate).LoadSynchronous();
        const int32 CurrentTileTypeIndex = DataAsset->GridData.GetTileTypeIndex(TileType);

        // If this tile's type is one of the ones we cleared, we need to re-add its instance.
        if (UMCommon::IsUObjectPointerSafe(TileType) && AffectedTypeIndices.Contains(CurrentTileTypeIndex))
        {
            if (_TileTypeIsmComponentMap.Contains(TileType) == false)
            {
                RegisterTileType(TileType);
            }

            UInstancedStaticMeshComponent* Ismc = GetTileTypeIsmc(CurrentTileTypeIndex);
            if (Ismc)
            {
                const FTransform TileTransform = GetCoordinateWorldTransform(Coordinate);
                const int32 NewInstanceId = Ismc->AddInstance(TileTransform);
                _CoordinateInstanceDataMap.Add(Coordinate, FHexGridTileInstanceData(CurrentTileTypeIndex, NewInstanceId));
            }
            else
            {
                L_ERR(HexGridEditor, "Ismc is null for type '{0}'", TileType);
            }
        }
    }

    // Finally, redraw the empty tiles overlay and invalidate the viewport.
    RedrawEmptyTiles(true);


    return;
}


void FHexGridEditorViewportClient::CollectBrushedCoordinates(FHexGridCoordinateList const& BrushedCoordinates)
{
    FOR_FORWARD(i, BrushedCoordinates.Num())
    {
        _PaintedCoordinates.AddUnique(BrushedCoordinates[i]);
    }


    return;
}


void FHexGridEditorViewportClient::CollectTileMapChange(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex)
{
    CollectBrushedCoordinates(ChangedCoordinates);
    _MapChangeTileTypeIndex = TileTypeIndex;
    _bChangeCollectedDuringPaint_Tile = true;


    return;
}



void FHexGridEditorViewportClient::HandlePaintBrushStarted(FHexGridCoordinateList const& ChangedCoordinates)
{
    _bIsPaintBrushInProgress = true;

    _PaintedCoordinates.Empty();
    CollectBrushedCoordinates(ChangedCoordinates);


    return;
}


void FHexGridEditorViewportClient::HandleAssetTileMapChanged(FHexGridCoordinateList const& ChangedCoordinates, const int TileTypeIndex)
{
    CollectTileMapChange(ChangedCoordinates, TileTypeIndex);

    // Turn the paint highlight ON using RAINBOW mode for the painted coordinate set
    SetTileGroupHighlightData(_PaintedCoordinates, FLinearColor::Black, 1.f, 1.f);


    return;
}


void FHexGridEditorViewportClient::HandlePaintBrushEnded(FHexGridCoordinateList const& ChangedCoordinates)
{
    // finalize the change on all collected coordinates and the type
    if (_bChangeCollectedDuringPaint_Tile)
    {
        ChangeTilesToNewType(_PaintedCoordinates, _MapChangeTileTypeIndex);
    }

    // Turn the highlight OFF
    SetTileGroupHighlightData(_PaintedCoordinates, FLinearColor::Black, 0.f, 0.f);

    // clear collected coordinates and type
    _MapChangeTileTypeIndex = -1;
    _PaintedCoordinates.Empty();

    _bIsPaintBrushInProgress = false;
    _bChangeCollectedDuringPaint_Tile = false;


    return;
}


void FHexGridEditorViewportClient::SetTileGroupHighlightData(FHexGridCoordinateList const& TileCoordinates, FLinearColor const& HighlightColor, const float HighlightIntensity, const float HighlightRainbowBlend)
{
    UHexGridDataAsset* DataAsset = HexGridAsset.Get();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    FOR_FORWARD(i, TileCoordinates.Num())
    {
        if (_CoordinateInstanceDataMap.Contains(TileCoordinates[i]) == false)
        {
            continue;
        }

        TObjectPtr<const UHexGridTileTypeAsset> TileType = nullptr;
        if (UMCommon::IsValidIndex(DataAsset->GridData.GridProperties.TileTypes, _CoordinateInstanceDataMap[TileCoordinates[i]].TileTypeId))
        {
            TileType = DataAsset->GridData.GridProperties.TileTypes[_CoordinateInstanceDataMap[TileCoordinates[i]].TileTypeId].LoadSynchronous();
        }

        if (_TileTypeIsmComponentMap.Contains(TileType) == false ||
            UMCommon::IsUObjectPointerSafe(_TileTypeIsmComponentMap[TileType]) == false)
        {
            L_ERR(HexGridEditor, "no ismc found for tile type '{0}'", TileType);
            continue;
        }

        const int IsmInstanceId = _CoordinateInstanceDataMap[TileCoordinates[i]].IsmInstanceId;

        if (_TileTypeIsmComponentMap[TileType]->IsValidInstance(IsmInstanceId))
        {
            UHexGridTileTypeAsset::SetTileInstanceMaterialHighlightData(_TileTypeIsmComponentMap[TileType], IsmInstanceId, HighlightColor, HighlightIntensity, HighlightRainbowBlend);

#if false
            L_DBG(HexGridEditor, "updating tile instance at '{0}' with type '{1}', ism id '{2}', to set custom data index '{3}' to value '{4}'",
                TileCoordinates[i],
                UMCommon::GetSoftPtrNameString(TileType),
                IsmInstanceId,
                CustomDataIndex,
                CustomDataValue
            );
#endif
        }
        else
        {
            L_WARN(HexGridEditor, "invalid ism instance id found for coordinate '{0}'", TileCoordinates[i]);

            _CoordinateInstanceDataMap.Remove(TileCoordinates[i]);
        }
    }


    return;
}


void FHexGridEditorViewportClient::HandleBrushTargetsChanged(FHexGridCoordinateList const& BrushTargetCoordinates)
{
    // Turn the old brush highlight OFF
    SetTileGroupHighlightData(_BrushTargetCoordinates, FLinearColor::Black, 0.f, 0.f);

    _BrushTargetCoordinates.Empty(BrushTargetCoordinates.Num());
    _BrushTargetCoordinates.Append(BrushTargetCoordinates);

    // set new brush highlights
    SetTileGroupHighlightData(_BrushTargetCoordinates, c_BrushHighlightColor, 1.f, 0.f);


    return;
}