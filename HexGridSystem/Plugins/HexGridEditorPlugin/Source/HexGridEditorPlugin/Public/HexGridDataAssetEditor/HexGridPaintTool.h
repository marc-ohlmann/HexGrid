// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "InteractiveTool.h"
#include "InputBehavior.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
#include "HexGridTileTypeAsset.h"
#include "HexGridTypes.h"
#include "HexGridPaintTool.generated.h"


class UHexGridDataAsset;
class UHexGridTileTypeAsset;
class ULocalPlayer;


/*
 * Properties for the paint tool that show up in the details panel
 */
UCLASS(Transient)
class UHexGridPaintToolProperties : public UInteractiveToolPropertySet
{
    GENERATED_BODY()

public:

    // the coordinate under your cursor
    UPROPERTY(VisibleAnywhere, Category = "Cursor Coordinate", meta = (ShowOnlyInnerProperties))
    FHexGridCoordinate CursorCoordinate = FHexGridCoordinate();

    // enables the tile painting tool
    UPROPERTY(EditAnywhere, Category = "Tile Settings")
    bool bEnableTilePaint = true;

    // the type of tile to paint
    UPROPERTY(EditAnywhere, Category = "Tile Settings", meta = (EditCondition = "bEnableTilePaint"))
    UHexGridTileTypeAsset* TileTypeToPaint = nullptr;

    // the shape of the brush used to paint
    UPROPERTY(EditAnywhere, Category = "Brush Settings")
    EHexGridShape BrushShape = EHexGridShape::hexagon;

    // the size of the brush used to paint
    UPROPERTY(EditAnywhere, Category = "Brush Settings")
    int BrushSize = 1;

    // click to empty the grid of all tiles and units
    UPROPERTY(EditAnywhere, Category = "Grid Controls")
    bool ClearGrid = false;

};


/*
 * Paint tool class
 */
UCLASS()
class UHexGridPaintTool : public UInteractiveTool, public IClickDragBehaviorTarget, public IHoverBehaviorTarget
{
    GENERATED_BODY()

public:

    // UInteractiveTool overrides
    virtual void Setup() override;
    virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

    // IClickDragBehaviorTarget implementation
    virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
    virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
    virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
    virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
    virtual void OnTerminateDragSequence() override;

    // IHoverBehaviorTarget implementation
    virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
    virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
    virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
    virtual void OnEndHover() override;

    void SetEditAsset(UHexGridDataAsset* InEditAsset);

protected:

    UPROPERTY()
    TObjectPtr<UHexGridPaintToolProperties> Properties;

    UPROPERTY()
    TObjectPtr<UHexGridDataAsset> EditAsset;

private:
    
    void PaintAt(FHexGridCoordinate const& Coordinate);

    FHexGridCoordinate GetCoordinateFromInputDeviceRay(FInputDeviceRay const& DevicePos);

    bool GetBrushCoordinatesAroundTargetCoordinate(FHexGridCoordinate const& TargetCoordinate, FHexGridCoordinateList& OutCoordinates);

};