// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/HexGridPaintTool.h"

#include "HexGridEditorPlugin.h"
#include "MCommon.h"
#include "HexGridDataAsset.h"
#include "HexGridDataAssetEditor/HexGridPaintTool.h"
#include "HexGridDataAssetEditor/HexGridEditorViewportClient.h"
#include "HexGridDataAssetEditor/HexGridEditorToolkit.h"
#include "HexGridDataAssetEditor/HexGridEditorMode.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "AssetEditorModeManager.h"
#include "InteractiveToolManager.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "Engine/World.h"
#include "SceneView.h"
#include "ToolContextInterfaces.h"


#define LOCTEXT_NAMESPACE "UHexGridPaintTool"


void UHexGridPaintTool::Setup()
{
    UInteractiveTool::Setup();

    // Create the property set that holds our tool's settings (e.g., brush type)
    Properties = NewObject<UHexGridPaintToolProperties>(this);
    AddToolPropertySource(Properties);

    // Register a click-drag behavior to receive mouse events
    UClickDragInputBehavior* MouseBehavior_ClickDrag = NewObject<UClickDragInputBehavior>();
    MouseBehavior_ClickDrag->Initialize(this);
    AddInputBehavior(MouseBehavior_ClickDrag);

    UMouseHoverBehavior* MouseBehavior_Hover = NewObject<UMouseHoverBehavior>();
    MouseBehavior_Hover->Initialize(this);
    AddInputBehavior(MouseBehavior_Hover);


}


void UHexGridPaintTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
    Super::OnPropertyModified(PropertySet, Property);

    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return;
    }

    if (!Property)
    {
        L_ERR(HexGridEditor, "Property is null");
        return;
    }

    UHexGridPaintToolProperties* ToolProperties = Properties.Get();
    if (!ToolProperties)
    {
        L_ERR(HexGridEditor, "invalid ToolProperties");
        return;
    }

    const FString PropertyName = Property->GetName();

    // clear grid button
    if (PropertyName.Equals(FString(TEXT(STRINGIFY(ClearGrid)))))
    {
        // mark the asset as modified to make it undoable
        // when this scoped transaction loses scope, the transaction automatically ends
        const FScopedTransaction Transaction(LOCTEXT("ClearGridTransaction", "Clear Hex Grid"));
        GridDataAsset->Modify();

        ToolProperties->ClearGrid = false;
        GridDataAsset->GridData.TileMap.Empty();
        GridDataAsset->GridData.GridProperties.TileTypes.Empty();
        GridDataAsset->BroadcastVisualizationChange();

        //  scoped transaction loses scope here
    }

    // brush changes trigger brush target changes
    if (PropertyName.Equals(FString(TEXT(STRINGIFY(BrushShape)))) ||
        PropertyName.Equals(FString(TEXT(STRINGIFY(BrushSize)))))
    {
        FHexGridCoordinateList BrushTargets;
        GetBrushCoordinatesAroundTargetCoordinate(ToolProperties->CursorCoordinate, BrushTargets);

        GridDataAsset->BroadcastBrushTargetsChanged(BrushTargets);
    }


    return;
}


FInputRayHit UHexGridPaintTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
    // We can always start painting, so return a hit. The actual grid hit is done in PaintAt.
    return FInputRayHit(1.0f);
}


void UHexGridPaintTool::OnClickPress(const FInputDeviceRay& PressPos)
{
    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return;
    }

    // broadcast the visualization change
    GridDataAsset->BroadcastPaintBrushStarted(FHexGridCoordinateList());

    // When the user first clicks, begin the transaction.
    GEditor->BeginTransaction(
        TEXT("UHexGridPaintTool"),
        LOCTEXT("PaintTransactionName", "Paint Hex Tiles"), // Name for the undo history
        EditAsset.Get() // The asset being changed
    );

    const FHexGridCoordinate TargetCoordinate = GetCoordinateFromInputDeviceRay(PressPos);
    PaintAt(TargetCoordinate);


    return;
}


void UHexGridPaintTool::OnClickDrag(const FInputDeviceRay& DragPos)
{
    const FHexGridCoordinate TargetCoordinate = GetCoordinateFromInputDeviceRay(DragPos);
    PaintAt(TargetCoordinate);
}


FHexGridCoordinate UHexGridPaintTool::GetCoordinateFromInputDeviceRay(FInputDeviceRay const& DevicePos)
{
    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return FHexGridCoordinate();
    }

    // Raycast against the grid plane (at Z=0 for this example)
    const FPlane GridPlane(FVector::ZeroVector, FVector::UpVector);

    const FVector IntersectionPoint = FMath::RayPlaneIntersection(DevicePos.WorldRay.Origin, DevicePos.WorldRay.Direction, GridPlane);

    return FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(IntersectionPoint, GridDataAsset->GridData.GridProperties.TileRadius);
}


void UHexGridPaintTool::PaintAt(FHexGridCoordinate const& TargetCoordinate)
{
    UInteractiveToolManager* ToolManager = GetToolManager();
    IF_NULL_RETURN_LERR(ToolManager, HexGridEditor, "ToolManager is null");

    UHexGridPaintToolProperties* ToolProperties = Properties.Get();
    if (!ToolProperties)
    {
        L_ERR(HexGridEditor, "invalid ToolProperties");
        return;
    }

    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return;
    }

    // Tell the engine you are about to modify the asset.
    // This saves its current state for the undo buffer.
    GridDataAsset->Modify();

    // collect painted coordinates
    FHexGridCoordinateList PaintBrushCoordinates;
    GetBrushCoordinatesAroundTargetCoordinate(TargetCoordinate, PaintBrushCoordinates);

    // tile painting
    if (ToolProperties->bEnableTilePaint)
    {
        // get index of target tile type to paint
        // if we are painting nullptr, then the index is -1
        int TargetTileTypeIndex = -1;
        if (GridDataAsset->GridData.GridProperties.TileTypes.Contains(ToolProperties->TileTypeToPaint))
        {
            TargetTileTypeIndex = GridDataAsset->GridData.GetTileTypeIndex(ToolProperties->TileTypeToPaint);
        }
        else if (UMCommon::IsUObjectPointerSafe(ToolProperties->TileTypeToPaint))
        {
            TargetTileTypeIndex = GridDataAsset->GridData.GridProperties.TileTypes.Add(ToolProperties->TileTypeToPaint);
        }

#if false
        L_DBG(HexGridEditor, "brushing {0} coordinates around target '{1}' with tile type index '{2}'", PaintBrushCoordinates.Num(), TargetCoordinate, TargetTileTypeIndex);
#endif

        FOR_FORWARD(i, PaintBrushCoordinates.Num())
        {
            if (GridDataAsset->GridData.TileMap.Contains(PaintBrushCoordinates[i]))
            {
                if (UMCommon::IsUObjectPointerSafe(ToolProperties->TileTypeToPaint))
                {
                    GridDataAsset->GridData.TileMap[PaintBrushCoordinates[i]] = TargetTileTypeIndex;
                }
                else
                {
                    GridDataAsset->GridData.TileMap.Remove(PaintBrushCoordinates[i]);
                }
            }
            else if (UMCommon::IsUObjectPointerSafe(ToolProperties->TileTypeToPaint))
            {
                GridDataAsset->GridData.TileMap.Add(PaintBrushCoordinates[i], TargetTileTypeIndex);
            }
        }

        GridDataAsset->BroadcastTileMapChanged(PaintBrushCoordinates, TargetTileTypeIndex);
    }

    // mark dirty
    ToolManager->PostInvalidation();


    return;
}


void UHexGridPaintTool::SetEditAsset(UHexGridDataAsset* InEditAsset)
{
    EditAsset = InEditAsset;


    return;
}



void UHexGridPaintTool::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return;
    }

    // When the user releases the mouse, end the transaction.
    GEditor->EndTransaction();

    // broadcast the visualization change
    GridDataAsset->BroadcastPaintBrushEnded(FHexGridCoordinateList());


    return;
}


void UHexGridPaintTool::OnTerminateDragSequence()
{
    return;
}


FInputRayHit UHexGridPaintTool::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
    FInputRayHit Out;

    UHexGridDataAsset* GridDataAsset = EditAsset.Get();
    if (!GridDataAsset)
    {
        L_ERR(HexGridEditor, "invalid GridDataAsset");
        return Out;
    }

    UHexGridPaintToolProperties* ToolProperties = Properties.Get();
    if (!ToolProperties)
    {
        L_ERR(HexGridEditor, "invalid ToolProperties");
        return Out;
    }

    const FHexGridCoordinate HoverTargetCoordinate = GetCoordinateFromInputDeviceRay(PressPos);

    if (ToolProperties->CursorCoordinate != HoverTargetCoordinate)
    {
        ToolProperties->CursorCoordinate = HoverTargetCoordinate;

        FHexGridCoordinateList BrushTargets;
        GetBrushCoordinatesAroundTargetCoordinate(ToolProperties->CursorCoordinate, BrushTargets);

        GridDataAsset->BroadcastBrushTargetsChanged(BrushTargets);
    }


    return Out;
}


void UHexGridPaintTool::OnBeginHover(const FInputDeviceRay& DevicePos)
{
    return;
}



bool UHexGridPaintTool::OnUpdateHover(const FInputDeviceRay& DevicePos)
{
    return false;
}


void UHexGridPaintTool::OnEndHover()
{
    return;
}


bool UHexGridPaintTool::GetBrushCoordinatesAroundTargetCoordinate(FHexGridCoordinate const& TargetCoordinate, FHexGridCoordinateList& OutCoordinates)
{
    UHexGridPaintToolProperties* ToolProperties = Properties.Get();
    if (!ToolProperties)
    {
        L_ERR(HexGridEditor, "invalid ToolProperties");
        return false;
    }

    if (ToolProperties->BrushSize <= 0)
    {
        return false;
    }

    if (ToolProperties->BrushSize == 1)
    {
        OutCoordinates.Add(TargetCoordinate);
        return true;
    }


    return FHexGridCoordinate::GetCoordinatesInShapeAroundCoordinate(TargetCoordinate, ToolProperties->BrushShape, ToolProperties->BrushSize, OutCoordinates);
}


#undef LOCTEXT_NAMESPACE