// Copyright 2025 Marc Ohlmann, All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "Layout/Geometry.h"
#include "HexGridTypes.h"
#include "AssetProviderInterface.h"
#include "HexGridMinimapWidget.generated.h"


class AHexGrid;


// Delegate for when a tile is clicked on the minimap
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinimapTileClicked, FHexGridCoordinate const& /*ClickedCoordinate*/);


/**
 * A high-performance minimap widget that manually draws tens of thousands
 * of hex tiles using the OnPaint (NativePaint) method.
 */
UCLASS()
class HEXGRIDPLUGIN_API UHexGridMinimapWidget : public UUserWidget, public IAssetProviderInterface
{
	GENERATED_BODY()

public:

	UHexGridMinimapWidget(const FObjectInitializer& ObjectInitializer);

	// UUserWidget Interface
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Main override for drawing all content.
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const;

	// Override to capture mouse inputs.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// --- Touch Input Overrides ---
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply NativeOnTouchGesture(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent) override;
	
	// 

	// IAssetProviderInterface Interface

public:

	// fill an array with all soft paths that need to be preloaded for this object
	virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const;

	//

public:

	// delegate broadcasts the hex coordinate that was clicked.
	FOnMinimapTileClicked OnMinimapTileClicked;

	// call to invalidate the widget and force a redraw
	void RedrawMap();

	// setup references to the grid data to be rendered
	void SetRenderedGridDataReferences(
		const FHexGridTileMap* GridTileDataRef,
		const FFogOfWarPerspectiveData* PlayerPerspectiveDataRef
	);

	// Zooms in by a fixed amount.
	void ZoomIn();

	// Zooms out by a fixed amount.
	void ZoomOut();

	// Gets the current zoom level.
	float GetZoom() const { return ZoomScalar; }

	// Gets the max zoom level.
	float GetZoomMax() const { return MaxZoom; }

	// Gets the min zoom level.
	float GetZoomMin() const { return MinZoom; }

	// set the zoom level of the minimap
	void SetZoom(const float NewZoom, bool bForceRedraw = true);

	// get the plane location that the view is centered on
	FVector2D GetViewCenter() const { return _ViewCenter; }

	// get the coordinate that the view is centered on
	FHexGridCoordinate GetViewCenterCoordinate() const;

	// get the plane location that the view is transitioning towards
	FVector2D GetTransitionTargetViewCenter() const { return _TransitionTargetViewCenter; }

	// get the coordinate that the view is transitioning towards
	FHexGridCoordinate GetTransitionTargetCoordinate() const;

	// set the plane location that the view is centered on
	void SetViewCenter(const FVector2D& NewCenter, bool bForceRedraw = true);

	// set the coordinate that the view is centered on
	void SetViewCenter(const FHexGridCoordinate& NewCenter, bool bForceRedraw = true);

	// smooth transition for the view center from its current location to the the given plane location 
	void PanToViewCenter(const FVector2D& NewCenter);

	// smooth transition for the view center from its current location to the the given coordinate 
	void PanToViewCenter(const FHexGridCoordinate& NewCenter);

	// check if the rendered tile data reference is valid
	bool HasRenderTileData() const { return _RenderedTileData != nullptr; }

	// check if the rendered player perspective data reference is valid
	bool HasRenderPlayerPerspectiveData() const { return _RenderedPlayerPerspectiveData != nullptr; }

	// get the rotation angle, in degrees, of the current view
	float GetViewRotationDegrees() const { return _ViewRotationDegrees; }

	// set the rotation angle, in degrees, of the current view
	void SetViewRotationDegrees(const float ViewRotationDegrees, bool bForceRedraw = true);

	// apply a smooth transition for the view rotation 
	void PanToViewRotationDegrees(const float ViewRotationDegrees);

	// set rotation to rotate one-hexagon directional facing (60 degrees) in the clockwise direction
	void SetRotationClockwise(bool bForceRedraw = true);

	// set rotation to rotate one-hexagon directional facing (60 degrees) in the counter-clockwise direction
	void SetRotateCounterClockwise(bool bForceRedraw = true);

	// smooth pan rotation to rotate one-hexagon directional facing (60 degrees) in the clockwise direction
	void PanRotateClockwise();

	// smooth pan rotation to rotate one-hexagon directional facing (60 degrees) in the counter-clockwise direction
	void PanRotateCounterClockwise();

	// reset the view center, zoom, and rotation back to default
	void ResetView();

	// get the pixel bounds of the minimap
	FBox2D GetMinimapBounds() const { return _MapPixelBounds; }

	// Adds or updates a dynamic indicator on the minimap (e.g., quest marker).
	// If a handle with this name already exists, it will be updated.
	void SetDynamicIndicator(FName const& Handle, FVector2D const& WorldPlaneLocation, FMinimapIndicatorProperties const& IndicatorProperties);

	// get the runtime properties for the given dynamic indicator
	FRuntimeMinimapIndicatorProperties GetDynamicIndicatorProperties(FName const& Handle) const;

	// Removes a dynamic indicator from the minimap.
	void ClearDynamicIndicator(FName const& Handle);

protected:

	// called before tiles are painted to the map
	virtual int32 PreTilesPainted(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const
	{
		return LayerId;
	}

	// called after tiles are painted to the map
	virtual int32 PostTilesPainted(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const
	{
		return LayerId;
	}

	// called before fog is painted to the map
	virtual int32 PreFogPainted(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const
	{
		return LayerId;
	}

	// called after fog is painted to the map
	virtual int32 PostFogPainted(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const
	{
		return LayerId;
	}

	// the brush to use for drawing
	UPROPERTY()
	FSlateBrush DrawBrush;


	// Color to draw over tiles that are completely hidden.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap")
	FLinearColor HiddenColor;

	// base tile radius to use when drawing on the minimap
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap")
	float MinimapTileRadius;

	// scalar used to zoom the view
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Zoom")
	float ZoomScalar;

	// the minimum zoom factor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Zoom")
	float MinZoom;

	// the maximum zoom factor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Zoom")
	float MaxZoom;

	// The factor to multiply/divide zoom by when calling ZoomIn/ZoomOut.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Zoom")
	float ZoomStepFactor;

	// Multiplier for pinch-to-zoom touch gesture.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Zoom", meta = (ClampMin = "0.0"))
	float PinchZoomSpeed;


	// The interpolation speed for smooth panning.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation")
	float PanInterpSpeed;

	// The interpolation speed for smooth rotation panning.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation")
	float PanRotationInterpSpeed;

	// How many pixels a user must drag before it's considered a "pan" instead of a "click".
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation")
	float PanDragThreshold;

	// How many pixels beyond the boundary of the grid bounds that we can pan.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation")
	float PanBoundaryPadding;

	// flag controls whether to enable touch rotation
	// NOT IMPLEMENTED:
	//	touch handling only tracks a single finger and cannot handle rotation currently
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation")
	bool bEnableTouchRotation;

	// rotation speed for touch input rotate gestures
	// NOT IMPLEMENTED:
	//	touch handling only tracks a single finger and cannot handle rotation currently
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Navigation", meta = (EditCondition = "bEnableTouchRotation == true"))
	float TouchRotateSpeed;


	// Optional texture to use for the background.
	// if not defined, will use the fallback color
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Appearance")
	TSoftObjectPtr<UTexture2D> BackgroundTexture;

	// Fallback color of the background if no texture is provided
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Appearance")
	FLinearColor BackgroundColor;

	// Texture to use for the North Indicator.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Appearance")
	TSoftObjectPtr<UTexture2D> NorthIndicatorTexture;

	// The size of the North Indicator icon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Appearance")
	FVector2D NorthIndicatorSize;

	// How far from the widget edge to place the North Indicator.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap|Appearance")
	float NorthIndicatorPadding;


	// Converts a local widget pixel position to its corresponding hex coordinate, factoring in pan and zoom.
	FHexGridCoordinate ConvertLocalPositionToHex(const FVector2D& InLocalPosition, const FGeometry& InGeometry) const;

	// Takes a potential view center and clamps it within the MapPixelBounds.
	FVector2D GetClampedViewCenter(const FVector2D& InViewCenter) const;

	// get the visibility of the tile at the given coordinate
	EHexGridTileVisibility GetTileVisibility(FHexGridCoordinate const& TileCoordinate) const;

	// get all listed tiles with the given visibility from render data
	void GetListedTilesWithVisibility(const EHexGridTileVisibility TileVisibility, FHexGridCoordinateList& OutTileCoordinates) const;

	// get the occupying actor on the tile at the given coordinate
	AActor* GetFirstPresentActorOfClassAtTileCoordinate(UClass* ActorClass, FHexGridCoordinate const& TileCoordinate) const;

	// templated get the occupying actor on the tile at the given coordinate
	template <class TActorClass>
	TActorClass* GetFirstPresentActorAtTileCoordinate(FHexGridCoordinate const& TileCoordinate) const
	{
		return Cast<TActorClass>(GetFirstPresentActorOfClassAtTileCoordinate(TActorClass::StaticClass(), TileCoordinate));
	}

	// get the effective tile radius after applying zoom
	float GetZoomTileRadius() const { return MinimapTileRadius * ZoomScalar; }

	// get the effective view center after applying zoom
	FVector2D GetZoomViewCenter() const { return _ViewCenter * ZoomScalar; }

private:

	// A pointer to the tile data to use for rendering
	const FHexGridTileMap* _RenderedTileData;

	// a pointer to the visibility data for the owning player's perspective
	const FFogOfWarPerspectiveData* _RenderedPlayerPerspectiveData;

	// the coordinate to center the view on
	UPROPERTY()
	FVector2D _ViewCenter;

	// the coordinate to transition the view towards
	UPROPERTY()
	FVector2D _TransitionTargetViewCenter;

	// True if the user is currently holding down the mouse button or a finger for panning.
	UPROPERTY()
	bool _bIsPanning;

	// True if the user has dragged past the PanDragThreshold, confirming a pan gesture.
	UPROPERTY()
	bool _bIsDragging;

	// The screen-space position where the current pan started (to check against drag threshold).
	UPROPERTY()
	FVector2D _PanStartPosition_ScreenSpace;

	// The screen-space position from the last frame, used to calculate pan delta.
	UPROPERTY()
	FVector2D _LastPanPosition_ScreenSpace;

	// handles smooth panning on every frame
	void OnTick_HandleSmoothPanning(float DeltaTime);

	// click or tap handler
	void HandleClickOrTap(const FGeometry& InGeometry, const FVector2D& InScreenSpacePosition);

	// the current view's rotation offset in degrees
	UPROPERTY()
	float _ViewRotationDegrees;

	// the transition target view's rotation offset in degrees
	UPROPERTY()
	float _TransitionTargetViewRotationDegrees;

	// clamp the given view rotation values to angle values within 0 and 360
	static float GetClampedViewRotationDegrees(float InViewRotation);

	// angle change, in degrees, to rotate clockwise
	static const float c_ViewRotationChange_Clockwise;

	// angle change, in degrees, to rotate counter-clockwise
	static const float c_ViewRotationChange_CounterClockwise;

	// cached bounds of the grid pixel map
	UPROPERTY()
	FBox2D _MapPixelBounds;

	// The last known size of this widget, updated in Tick and Paint.
	UPROPERTY()
	FVector2D _CachedWidgetSize;

	// Map of all dynamic indicators to be drawn.
	UPROPERTY()
	TMap<FName, FRuntimeMinimapIndicatorProperties> _DynamicIndicatorMap;

};
