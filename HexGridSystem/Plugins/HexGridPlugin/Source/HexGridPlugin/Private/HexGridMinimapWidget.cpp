// Copyright 2025 Marc Ohlmann, All Rights Reserved.


#include "HexGridMinimapWidget.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "HexGrid.h"
#include "HexGridFogOfWarComponent.h"
#include "HexGridTileTypeAsset.h"

#include "Engine/Texture2D.h"
#include "Rendering/DrawElements.h"
#include "Styling/SlateBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Framework/Application/SlateApplication.h"


const float UHexGridMinimapWidget::c_ViewRotationChange_Clockwise = UMathTypes::c_Sixth_Circle_Degrees;
const float UHexGridMinimapWidget::c_ViewRotationChange_CounterClockwise = -1.f * c_ViewRotationChange_Clockwise;


UHexGridMinimapWidget::UHexGridMinimapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnMinimapTileClicked()
	, DrawBrush()
	, HiddenColor(FLinearColor::Black)
	, MinimapTileRadius(4.f)
	, ZoomScalar(1.f)
	, MinZoom(0.75f)
	, MaxZoom(4.f)
	, ZoomStepFactor(1.2f)
	, PinchZoomSpeed(0.5f)
	, PanInterpSpeed(8.f)
	, PanRotationInterpSpeed(8.f)
	, PanDragThreshold(5.f)
	, PanBoundaryPadding(200.f)
	, bEnableTouchRotation(true)
	, TouchRotateSpeed(1.f)
	, BackgroundTexture(nullptr)
	, BackgroundColor(0.05f, 0.05f, 0.05f, 0.75f)
	, NorthIndicatorTexture(nullptr)
	, NorthIndicatorSize(32.f, 32.f)
	, NorthIndicatorPadding(16.f)
	, _RenderedTileData(nullptr)
	, _RenderedPlayerPerspectiveData(nullptr)
	, _ViewCenter()
	, _TransitionTargetViewCenter()
	, _bIsPanning(false)
	, _bIsDragging(false)
	, _PanStartPosition_ScreenSpace(FVector2D::ZeroVector)
	, _LastPanPosition_ScreenSpace(FVector2D::ZeroVector)
	, _ViewRotationDegrees(0.f)
	, _TransitionTargetViewRotationDegrees(0.f)
	, _MapPixelBounds()
	, _CachedWidgetSize(FVector2D::ZeroVector)
	, _DynamicIndicatorMap()
{
	// This is a simple brush we can use to draw solid colors.
	// We only need one, and we can tint it as needed.
	DrawBrush.SetResourceObject(nullptr); // No texture
	DrawBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	DrawBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	DrawBrush.Margin = FMargin(0);
	DrawBrush.TintColor = FLinearColor::White;

	// receive input events for game and ui input modes
	SetIsFocusable(true);

	// needs to be hit testable for input events
	SetVisibility(ESlateVisibility::Visible);

	// Mark as volatile to ensure OnPaint is not called every frame
	// done again in NativeConstruct to ensure the flag stays clear
	bIsVolatile = false;
}


void UHexGridMinimapWidget::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	IAssetProviderInterface::CollectPreloadAsset(BackgroundTexture, OutAssets);
	IAssetProviderInterface::CollectPreloadAsset(NorthIndicatorTexture, OutAssets);


	return;
}



void UHexGridMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Mark as volatile to ensure OnPaint is not called every frame
	// this gets set to true on NativeConstruct so must clear this flag here
	bIsVolatile = false;

	// get the default drag threshold from the application
	PanDragThreshold = FSlateApplication::Get().GetDragTriggerDistance();


	return;
}


void UHexGridMinimapWidget::NativeDestruct()
{
	_RenderedTileData = nullptr;
	_RenderedPlayerPerspectiveData = nullptr;


	Super::NativeDestruct();
	return;
}


void UHexGridMinimapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	return;
}


void UHexGridMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Cache the widget size for clamping logic
	_CachedWidgetSize = MyGeometry.GetLocalSize();

	OnTick_HandleSmoothPanning(InDeltaTime);


	return;
}


int32 UHexGridMinimapWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	// Call the base implementation
	LayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled
	);

	UWorld* World = GetWorld();
	IF_NULL_RETURN_VAL_LERR(World, LayerId, HexGrid, "World is null");

	// skip in editor previews
	if (World->WorldType == EWorldType::EditorPreview || World->WorldType == EWorldType::Editor)
	{
		return LayerId;
	}

	// Get the local size of this widget.
	const FVector2D LocalWidgetSize = _CachedWidgetSize;
	const FVector2D WidgetPixelCenter = LocalWidgetSize / 2.f;
	const float EffectiveTileRadius = GetZoomTileRadius();
	const FVector2D ScaledViewCenter_PixelSpace = GetZoomViewCenter();

	// Calculate the pixel center of the widget. We'll treat this as our (0,0) origin.
	const FVector2D CenterOffset = LocalWidgetSize / 2.f;

	// Calculate the size of a "box" to draw for each tile.
	// This is an approximation for performance. For true hexes, you'd
	// calculate 6 verts and use Context.DrawLines.
	const FVector2D TileDrawSize(EffectiveTileRadius * 2.f, EffectiveTileRadius * 2.f);
	const FVector2D TileDrawHalfSize = TileDrawSize / 2.f;

	// Increment layer so all subsequent draws are on top of parent draws
	LayerId += 1;

	// Draw Background (before clipping)
	if (BackgroundTexture.IsNull() == false)
	{
		UTexture2D* LoadedBackgroundTexture = UMCommon::EnsureSoftObjectPtrLoaded(BackgroundTexture);
		if (UMCommon::IsUObjectPointerSafe(LoadedBackgroundTexture))
		{
			// A valid texture is provided, draw it.
			// Create a temporary brush on the stack.
			const FSlateImageBrush TempBrush = FSlateImageBrush(
				LoadedBackgroundTexture,
				LocalWidgetSize, // Stretch the texture to the full widget size
				BackgroundColor  // Use BackgroundColor as the tint
			);

			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(LocalWidgetSize, FSlateLayoutTransform()),
				&TempBrush,
				ESlateDrawEffect::None,
				TempBrush.TintColor.GetSpecifiedColor()
			);
		}
	}
	else
	{
		// No texture, draw a solid box with BackgroundColor.
		// We create a temporary Box brush on the stack.
		FSlateBrush TempBoxBrush;
		TempBoxBrush.SetResourceObject(nullptr);
		TempBoxBrush.DrawAs = ESlateBrushDrawType::Box;
		TempBoxBrush.TintColor = BackgroundColor; // Use the property directly

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(LocalWidgetSize, FSlateLayoutTransform()),
			&TempBoxBrush,
			ESlateDrawEffect::None,
			TempBoxBrush.TintColor.GetSpecifiedColor()
		);
	}

	// Setup Clipping
	// This is the most important fix for "spill over".
	// We tell Slate to clip all subsequent drawing to the widget's geometry.
	OutDrawElements.PushClip(FSlateClippingZone(AllottedGeometry.ToPaintGeometry()));

	const bool bHasTileData = HasRenderTileData();
	const bool bHasVisibilityData = HasRenderPlayerPerspectiveData();

	LayerId = PreTilesPainted(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled
	);

	FHexGridCoordinateList TileCoordinates;
	if (bHasTileData)
	{
		// Increment layer
		LayerId += 1;

		// Draw Tile Terrain
		for (const FHexGridTileTypeRegistry& TileRegistry : _RenderedTileData->TileTypeRegistries)
		{
			IF_NULL_CONTINUE_LERR(TileRegistry.TileType, HexGrid, "TileType is null");

			FOR_MAP_CONST(TilePair, TileRegistry.Tiles)
			{
				const FHexGridCoordinate& TileCoordinate = TilePair.Key;

				// Inject directly into our master list for the Fog pass later
				TileCoordinates.List.Add(TileCoordinate);

				if (bHasVisibilityData &&
					GetTileVisibility(TileCoordinate) == EHexGridTileVisibility::hidden)
				{
					// Only draw visible or fogged tiles. Hidden tiles will be drawn in the fog pass.
					continue;
				}

				// Get the "world" pixel location of this tile
				const FVector2D TileBasePixelLocation = TileCoordinate.GetCenterPlaneOffsetLocation(MinimapTileRadius);
				const FVector2D TileZoomedPixelLocation = TileBasePixelLocation * ZoomScalar;

				// Get the tile's location relative to the zoomed and rotated view center
				const FVector2D DeltaPixelLocation = (TileZoomedPixelLocation - ScaledViewCenter_PixelSpace).GetRotated(-1.f * _ViewRotationDegrees);

				// Get the final position on the widget: widget center + delta
				const FVector2D FinalPixelPositionOnWidget = WidgetPixelCenter + DeltaPixelLocation;

				// Culling check
				const FVector2D TileTopLeft = FinalPixelPositionOnWidget - TileDrawHalfSize;
				const FVector2D TileBottomRight = FinalPixelPositionOnWidget + TileDrawHalfSize;

				if (TileBottomRight.X < 0.f || TileTopLeft.X > LocalWidgetSize.X ||
					TileBottomRight.Y < 0.f || TileTopLeft.Y > LocalWidgetSize.Y)
				{
					continue; // This tile is outside the widget's bounds, skip drawing
				}

				// Try to find a specific brush (icon) for this tile type
				UTexture2D* TextureToDraw = UMCommon::EnsureSoftObjectPtrLoaded(TileRegistry.TileType->IconImage);
				const FSlateLayoutTransform LayoutTransform = FSlateLayoutTransform(FinalPixelPositionOnWidget - TileDrawHalfSize);

				// If a valid texture is found, draw it
				if (UMCommon::IsUObjectPointerSafe(TextureToDraw))
				{
					const FSlateImageBrush TileBrush = FSlateImageBrush(
						TextureToDraw,
						TileDrawSize
					);

					FSlateDrawElement::MakeBox(
						OutDrawElements,
						LayerId,
						AllottedGeometry.ToPaintGeometry(TileDrawSize, LayoutTransform),
						&TileBrush,
						ESlateDrawEffect::None,
						FLinearColor::White
					);
				}
				// If no texture is found, fall back to solid colors
				else
				{
					// Use the generic, untextured color brush
					const FSlateBrush* BrushToDraw = &DrawBrush;
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						LayerId,
						AllottedGeometry.ToPaintGeometry(TileDrawSize, LayoutTransform),
						BrushToDraw, // Use the FSlateBrush*
						ESlateDrawEffect::None,
						FLinearColor::White
					);
				}
			}
		}
	}

	LayerId = PostTilesPainted(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled
	);

	LayerId = PreFogPainted(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled
	);

	if (bHasVisibilityData)
	{
		// Increment layer
		LayerId += 1;

		FHexGridCoordinateList MinimapCoordinates;
		TileCoordinates.GenerateHexCoordinatesInRegion(MinimapCoordinates);

		// Draw Fog of War
		// (This is a separate loop to ensure fog draws on top of everything)
		for (const FHexGridCoordinate& TileCoordinate : MinimapCoordinates.List)
		{
			if (GetTileVisibility(TileCoordinate) == EHexGridTileVisibility::hidden)
			{
				const FVector2D TileBasePixelLocation = TileCoordinate.GetCenterPlaneOffsetLocation(MinimapTileRadius);
				const FVector2D TileZoomedPixelLocation = TileBasePixelLocation * ZoomScalar;

				// Get the tile's location relative to the zoomed and rotated view center
				const FVector2D DeltaPixelLocation = (TileZoomedPixelLocation - ScaledViewCenter_PixelSpace).GetRotated(-1.f * _ViewRotationDegrees);

				const FVector2D FinalPixelPositionOnWidget = WidgetPixelCenter + DeltaPixelLocation;

				// Culling check
				const FVector2D TileTopLeft = FinalPixelPositionOnWidget - TileDrawHalfSize;
				const FVector2D TileBottomRight = FinalPixelPositionOnWidget + TileDrawHalfSize;

				if (TileBottomRight.X < 0.f || TileTopLeft.X > LocalWidgetSize.X ||
					TileBottomRight.Y < 0.f || TileTopLeft.Y > LocalWidgetSize.Y)
				{
					continue; // This tile is outside the widget's bounds, skip drawing
				}

				const FSlateLayoutTransform LayoutTransform = FSlateLayoutTransform(FinalPixelPositionOnWidget - TileDrawHalfSize);

				FSlateDrawElement::MakeBox(
					OutDrawElements,
					LayerId,
					AllottedGeometry.ToPaintGeometry(TileDrawSize, LayoutTransform),
					&DrawBrush,
					ESlateDrawEffect::None,
					HiddenColor
				);
			}
		}
	}

	LayerId = PostFogPainted(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled
	);


	// Draw Dynamic Indicators
	if (_DynamicIndicatorMap.Num() > 0)
	{
		LayerId += 1; // Draw on top of fog / post-fog

		const FVector2D WidgetHalfSize = LocalWidgetSize / 2.f;

		FOR_MAP_CONST(IndicatorPair, _DynamicIndicatorMap)
		{
			const FName IndicatorHandle = IndicatorPair.Key;
			const auto& IndicatorData = IndicatorPair.Value;

			UTexture2D* IndicatorTexture = UMCommon::EnsureSoftObjectPtrLoaded(IndicatorData.VisualProperties.Texture);
			IF_NULL_CONTINUE_LERR(IndicatorTexture, HexGrid, "indicator named '{0}' texture is null", IndicatorHandle);

			const FVector2D IndicatorHalfSize = IndicatorData.VisualProperties.Size / 2.f;
			const FVector2D PaddedHalfSize = (WidgetHalfSize - FVector2D(IndicatorData.VisualProperties.OffScreenEdgePadding)).ComponentMax(FVector2D::ZeroVector);
			const FVector2D BasePixelLocation = IndicatorData.MinimapLocation;
			const FVector2D ZoomedPixelLocation = BasePixelLocation * ZoomScalar;
			const FVector2D DeltaPixelLocation = (ZoomedPixelLocation - ScaledViewCenter_PixelSpace).GetRotated(-1.f * _ViewRotationDegrees);
			const FVector2D FinalPixelPositionOnWidget = WidgetPixelCenter + DeltaPixelLocation;
			const FVector2D TopLeft = FinalPixelPositionOnWidget - IndicatorHalfSize;
			const FVector2D BottomRight = FinalPixelPositionOnWidget + IndicatorHalfSize;
			const bool bIsOnScreen = (TopLeft.X > 0 && BottomRight.X < LocalWidgetSize.X &&
				TopLeft.Y > 0 && BottomRight.Y < LocalWidgetSize.Y);

			const FSlateImageBrush TempBrush = FSlateImageBrush(
				IndicatorTexture,
				IndicatorData.VisualProperties.Size,
				IndicatorData.VisualProperties.TintColor
			);

			if (bIsOnScreen)
			{
				const FSlateLayoutTransform LayoutTransform(FinalPixelPositionOnWidget - IndicatorHalfSize);
				FSlateRenderTransform OnScreenRotation; // Default is Identity (upright)

				if (IndicatorData.VisualProperties.bRotateOnScreen)
				{
					// Apply the map's current rotation
					OnScreenRotation = FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(_ViewRotationDegrees)));
				}

				FSlateDrawElement::MakeBox(
					OutDrawElements,
					LayerId,
					AllottedGeometry.ToPaintGeometry(
						IndicatorData.VisualProperties.Size,
						LayoutTransform,
						OnScreenRotation
					),
					&TempBrush,
					ESlateDrawEffect::None,
					TempBrush.TintColor.GetSpecifiedColor()
				);
			}
			else
			{
				const FVector2D DirectionToTarget = (FinalPixelPositionOnWidget - WidgetPixelCenter).GetSafeNormal();
				const float ScalarX = FMath::IsNearlyZero(DirectionToTarget.X) ? FLT_MAX : FMath::Abs(PaddedHalfSize.X / DirectionToTarget.X);
				const float ScalarY = FMath::IsNearlyZero(DirectionToTarget.Y) ? FLT_MAX : FMath::Abs(PaddedHalfSize.Y / DirectionToTarget.Y);
				const float IntersectScalar = FMath::Min(ScalarX, ScalarY);
				const FVector2D EdgePosition = WidgetPixelCenter + DirectionToTarget * IntersectScalar;
				const FSlateLayoutTransform LayoutTransform(EdgePosition - IndicatorHalfSize);
				FSlateRenderTransform OffScreenRotation;
				if (IndicatorData.VisualProperties.bRotateOffScreen)
				{
					const float AngleRad = FMath::Atan2(DirectionToTarget.Y, DirectionToTarget.X) + UE_PI / 2.f;
					OffScreenRotation = FSlateRenderTransform(FQuat2D(AngleRad));
				}
				FSlateDrawElement::MakeBox(
					OutDrawElements,
					LayerId,
					AllottedGeometry.ToPaintGeometry(
						IndicatorData.VisualProperties.Size,
						LayoutTransform,
						OffScreenRotation
					),
					&TempBrush,
					ESlateDrawEffect::None,
					TempBrush.TintColor.GetSpecifiedColor()
				);
			}
		}
	}


	if (NorthIndicatorTexture.IsNull() == false)
	{
		LayerId += 1; // Draw on top of dynamic indicators

		// Draw North Indicator
		UTexture2D* IndicatorTexture = UMCommon::EnsureSoftObjectPtrLoaded(NorthIndicatorTexture);
		if (UMCommon::IsUObjectPointerSafe(IndicatorTexture))
		{
			// (0, -1) is "Up" in pixel space, which we define as Map North.
			const FVector2D ScreenNorthDirection = FVector2D(0.f, -1.f).GetRotated(-1.f * _ViewRotationDegrees);

			// Use a clear name for the widget's half-size
			const FVector2D WidgetHalfSize = LocalWidgetSize / 2.f;
			const FVector2D PaddedHalfSize = (WidgetHalfSize - FVector2D(NorthIndicatorPadding)).ComponentMax(FVector2D::ZeroVector);

			// Find the "time" (scalar) it takes the ray to hit the X and Y planes of our padded box.
			const float ScalarX = FMath::IsNearlyZero(ScreenNorthDirection.X)
				? FLT_MAX
				: FMath::Abs(PaddedHalfSize.X / ScreenNorthDirection.X);

			const float ScalarY = FMath::IsNearlyZero(ScreenNorthDirection.Y)
				? FLT_MAX
				: FMath::Abs(PaddedHalfSize.Y / ScreenNorthDirection.Y);

			// The first edge we hit is the one with the *smallest* scalar.
			const float IntersectScalar = FMath::Min(ScalarX, ScalarY);

			// The final position is the center + the direction vector scaled by our intersection "time".
			const FVector2D IndicatorCenterPosition = WidgetPixelCenter + ScreenNorthDirection * IntersectScalar;

			// Use a clear name for the indicator's half-size
			const FVector2D IndicatorHalfSize = NorthIndicatorSize / 2.f;

			// We need the angle to rotate the brush.
			const float IndicatorAngleRadians = FMath::Atan2(ScreenNorthDirection.Y, ScreenNorthDirection.X) + UE_PI / 2.f;

			// Create a simple layout transform for the (un-rotated) top-left position.
			// Use the new IndicatorHalfSize variable
			const FSlateLayoutTransform LayoutTransform(IndicatorCenterPosition - IndicatorHalfSize);

			// Create a render transform that is *only* the rotation.
			// Rename the local variable to avoid collision with the UWidget member
			const FSlateRenderTransform IndicatorRotationTransform = FSlateRenderTransform(FQuat2D(IndicatorAngleRadians));

			// Create the temporary brush for drawing.
			const FSlateImageBrush TempBrush = FSlateImageBrush(
				IndicatorTexture,
				NorthIndicatorSize,
				FLinearColor::White
			);

			// Call the correct ToPaintGeometry overload.
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(
					NorthIndicatorSize,
					LayoutTransform,
					IndicatorRotationTransform // Use the new, safe variable name
				),
				&TempBrush,
				ESlateDrawEffect::None,
				TempBrush.TintColor.GetSpecifiedColor()
			);
		}
	}

	// Pop Clipping Zone
	// This stops the clipping, restoring drawing for the rest of the UI.
	OutDrawElements.PopClip();


	return LayerId;
}


FReply UHexGridMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		ResetView();
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		_bIsPanning = true;
		_bIsDragging = false;
		_PanStartPosition_ScreenSpace = InMouseEvent.GetScreenSpacePosition();
		_LastPanPosition_ScreenSpace = _PanStartPosition_ScreenSpace;

		// We must capture the input event
		return FReply::Handled().CaptureMouse(TakeWidget());
	}


	return FReply::Unhandled();
}


FReply UHexGridMinimapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (_bIsPanning)
	{
		const FVector2D CurrentPosition = InMouseEvent.GetScreenSpacePosition();

		if (_bIsDragging == false)
		{
			// Check if we've moved past the drag threshold
			const float DragDistance = FVector2D::DistSquared(_PanStartPosition_ScreenSpace, CurrentPosition);
			if (DragDistance > FMath::Square(PanDragThreshold))
			{
				_bIsDragging = true;
			}
		}

		if (_bIsDragging)
		{
			// Calculate the delta in screen space
			const FVector2D ScreenSpaceDelta = CurrentPosition - _LastPanPosition_ScreenSpace;

			// Convert the screen space delta to local space delta
			// We must account for the widget's own scale in the UI
			const float WidgetDTScale = InGeometry.GetAccumulatedLayoutTransform().GetScale();
			if (WidgetDTScale > KINDA_SMALL_NUMBER)
			{
				// Convert to "world pixel" delta (un-zoomed)
				const FVector2D PixelSpaceDelta = ScreenSpaceDelta / (WidgetDTScale * ZoomScalar);

				// **Apply rotation to the pan delta**
				const FVector2D RotatedPixelSpaceDelta = PixelSpaceDelta.GetRotated(_ViewRotationDegrees);

				// Get the new target, then clamp it
				const FVector2D NewTargetCenter = _TransitionTargetViewCenter - RotatedPixelSpaceDelta;
				_TransitionTargetViewCenter = GetClampedViewCenter(NewTargetCenter);

				// Also update the current view center for immediate responsiveness, and clamp it
				const FVector2D NewViewCenter = _ViewCenter - RotatedPixelSpaceDelta;
				_ViewCenter = GetClampedViewCenter(NewViewCenter);

				RedrawMap();
			}
		}

		_LastPanPosition_ScreenSpace = CurrentPosition;
		return FReply::Handled();
	}


	return FReply::Unhandled();
}


FReply UHexGridMinimapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		_bIsPanning = false;

		if (_bIsDragging == false)
		{
			// This was a click, not a drag. Fire the delegate.
			HandleClickOrTap(InGeometry, InMouseEvent.GetScreenSpacePosition());
		}

		_bIsDragging = false;

		// Release input event capture
		return FReply::Handled().ReleaseMouseCapture();
	}


	return FReply::Unhandled();
}


void UHexGridMinimapWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// If the mouse leaves the widget *while* we are panning, stop the pan.
	if (_bIsPanning)
	{
		_bIsPanning = false;
		_bIsDragging = false;

		// Get the underlying Slate widget
		TSharedPtr<SWidget> CachedWidget = GetCachedWidget();
		if (CachedWidget.IsValid() && CachedWidget->HasMouseCapture())
		{
			// Tell the main Slate Application to release capture
			FSlateApplication::Get().ReleaseAllPointerCapture();
		}
	}


	return;
}


FReply UHexGridMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float WheelDelta = InMouseEvent.GetWheelDelta();
	if (WheelDelta > 0)
	{
		ZoomIn();
	}
	else if (WheelDelta < 0)
	{
		ZoomOut();
	}

	// We handled it, but don't force a redraw here, SetZoom will do it.
	return FReply::Handled();
}


FReply UHexGridMinimapWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent)
{
	// Only track panning for the first finger
	if (InTouchEvent.GetPointerIndex() == 0)
	{
		_bIsPanning = true;
		_bIsDragging = false;
		_PanStartPosition_ScreenSpace = InTouchEvent.GetScreenSpacePosition();
		_LastPanPosition_ScreenSpace = _PanStartPosition_ScreenSpace;
	}
	else
	{
		// A second (or more) finger has touched down.
		// This is a multi-touch gesture, so we MUST disable single-finger panning.
		_bIsPanning = false;
		_bIsDragging = false;
	}

	// We handle this to get OnTouchMoved events, but we DO NOT capture the "mouse".
	// This allows Slate to detect multi-touch and fire OnTouchGesture.
	return FReply::Unhandled();
}


FReply UHexGridMinimapWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent)
{
	// Only pan if we are in panning mode (one finger) and it's the first finger
	if (_bIsPanning && InTouchEvent.GetPointerIndex() == 0)
	{
		const FVector2D CurrentPosition = InTouchEvent.GetScreenSpacePosition();

		if (_bIsDragging == false)
		{
			// Check if we've moved past the drag threshold
			const float DragDistance = FVector2D::DistSquared(_PanStartPosition_ScreenSpace, CurrentPosition);
			if (DragDistance > FMath::Square(PanDragThreshold))
			{
				_bIsDragging = true;
				_LastPanPosition_ScreenSpace = CurrentPosition;
				return FReply::Handled();
			}
		}

		if (_bIsDragging)
		{
			// Calculate the delta in screen space
			const FVector2D ScreenSpaceDelta = CurrentPosition - _LastPanPosition_ScreenSpace;

			// Convert the screen space delta to local space delta
			// We must account for the widget's own scale in the UI
			const float WidgetDTScale = InGeometry.GetAccumulatedLayoutTransform().GetScale();
			if (WidgetDTScale > KINDA_SMALL_NUMBER)
			{
				// Convert to "world pixel" delta (un-zoomed)
				const FVector2D PixelSpaceDelta = ScreenSpaceDelta / (WidgetDTScale * ZoomScalar);

				// **Apply rotation to the pan delta**
				const FVector2D RotatedPixelSpaceDelta = PixelSpaceDelta.GetRotated(_ViewRotationDegrees);

				// Get the new target, then clamp it
				const FVector2D NewTargetCenter = _TransitionTargetViewCenter - RotatedPixelSpaceDelta;
				_TransitionTargetViewCenter = GetClampedViewCenter(NewTargetCenter);

				// Also update the current view center for immediate responsiveness, and clamp it
				const FVector2D NewViewCenter = _ViewCenter - RotatedPixelSpaceDelta;
				_ViewCenter = GetClampedViewCenter(NewViewCenter);

				RedrawMap();
			}

			_LastPanPosition_ScreenSpace = CurrentPosition;
			return FReply::Handled();
		}

		// If we are "panning" (finger 0 is down) but NOT yet "dragging",
		// we must return UNHANDLED. This allows the gesture recognizer
		// to see the move event and detect a pinch if a 
		// second finger touches down.
		_LastPanPosition_ScreenSpace = CurrentPosition;
		return FReply::Unhandled();
	}

	// Not panning mode, or not finger 0. Let gestures handle it.
	return FReply::Unhandled();
}


FReply UHexGridMinimapWidget::NativeOnTouchGesture(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// Stop any single-finger panning when a gesture starts
	_bIsPanning = false;
	_bIsDragging = false;

	bool bHandled = false;

	// Handle Pinch-to-Zoom
	if (InGestureEvent.GetGestureType() == EGestureEvent::Magnify)
	{
		const float ZoomDelta = InGestureEvent.GetGestureDelta().X * PinchZoomSpeed;
		SetZoom(ZoomScalar + ZoomDelta);
		bHandled = true;
	}

	// Handle Two-Finger Rotate
	if (bEnableTouchRotation && InGestureEvent.GetGestureType() == EGestureEvent::Rotate)
	{
		// GetRotateDelta() is in radians, convert to degrees
		const float RotateDeltaDegrees = FMath::RadiansToDegrees(InGestureEvent.GetGestureDelta().X * TouchRotateSpeed);

		// Set rotation directly (don't use smooth, it feels laggy with gestures)
		SetViewRotationDegrees(_ViewRotationDegrees + RotateDeltaDegrees);
		bHandled = true;
	}

	return bHandled ? FReply::Handled() : FReply::Unhandled();
}


FReply UHexGridMinimapWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent)
{
	// Only process the "end" event for the first finger
	if (InTouchEvent.GetPointerIndex() == 0)
	{
		// If we were in a pan, but not dragging, it was a tap.
		if (_bIsPanning && !_bIsDragging)
		{
			// This was a click, not a drag. Fire the delegate.
			HandleClickOrTap(InGeometry, InTouchEvent.GetScreenSpacePosition());
		}

		// Reset flags now that the first finger is up
		_bIsPanning = false;
		_bIsDragging = false;
		return FReply::Handled();
	}


	return FReply::Handled();
}


void UHexGridMinimapWidget::RedrawMap()
{
	TSharedPtr<SWidget> SlateMapWidget = GetCachedWidget();
	if (SlateMapWidget.IsValid() == false)
	{
		L_ERR(HexGrid, "SlateMapWidget is invalid");
		return;
	}

	SlateMapWidget->Invalidate(EInvalidateWidgetReason::Paint);


	return;
}


void UHexGridMinimapWidget::SetRenderedGridDataReferences(
	const FHexGridTileMap* GridTileDataRef,
	const FFogOfWarPerspectiveData* PlayerPerspectiveDataRef
)
{
	_RenderedTileData = GridTileDataRef;
	_RenderedPlayerPerspectiveData = PlayerPerspectiveDataRef;

	// Now, pre-calculate the total bounds of the map in pixel space
	if (_RenderedTileData != nullptr)
	{
		FHexGridCoordinateList AllTiles;
		_RenderedTileData->GetTileCoordinates(AllTiles);

		// GetBounds() returns the pixel-space bounds of all hex corners
		_MapPixelBounds = AllTiles.GetBounds(MinimapTileRadius);
	}
	else
	{
		_MapPixelBounds = FBox2D(ForceInit); // Invalidate the bounds
	}

	// Reset the view to the new map's center
	ResetView();


	return;
}


void UHexGridMinimapWidget::SetViewCenter(const FVector2D& NewCenter, bool bForceRedraw)
{
	_ViewCenter = GetClampedViewCenter(NewCenter);
	_TransitionTargetViewCenter = _ViewCenter;

	if (bForceRedraw)
	{
		RedrawMap();
	}


	return;
}


void UHexGridMinimapWidget::SetViewCenter(const FHexGridCoordinate& InCenterCoordinate, bool bForceRedraw)
{
	// Convert hex to "base pixel" space and set instantly
	SetViewCenter(
		InCenterCoordinate.GetCenterPlaneOffsetLocation(MinimapTileRadius),
		bForceRedraw
	);


	return;
}


void UHexGridMinimapWidget::SetZoom(const float NewZoom, bool bForceRedraw)
{
	// Clamp zoom to reasonable values
	ZoomScalar = FMath::Clamp(NewZoom, MinZoom, MaxZoom);

	if (bForceRedraw)
	{
		RedrawMap();
	}


	return;
}


void UHexGridMinimapWidget::ZoomIn()
{
	SetZoom(ZoomScalar * ZoomStepFactor);


	return;
}


void UHexGridMinimapWidget::ZoomOut()
{
	if (FMath::IsNearlyZero(ZoomStepFactor))
	{
		L_ERR(HexGrid, "ZoomStepFactor is zero");
		return;
	}

	SetZoom(ZoomScalar / ZoomStepFactor);


	return;
}


void UHexGridMinimapWidget::PanToViewCenter(const FVector2D& NewCenter)
{
	_TransitionTargetViewCenter = GetClampedViewCenter(NewCenter);


	return;
}


void UHexGridMinimapWidget::PanToViewCenter(const FHexGridCoordinate& NewCenter)
{
	// Set the *target* to the new coordinate's pixel location
	PanToViewCenter(NewCenter.GetCenterPlaneOffsetLocation(MinimapTileRadius));


	return;
}


void UHexGridMinimapWidget::OnTick_HandleSmoothPanning(float DeltaTime)
{
	bool bNeedsRedraw = false;

	_TransitionTargetViewCenter = GetClampedViewCenter(_TransitionTargetViewCenter);

	// Handle smooth panning
	if (!_ViewCenter.Equals(_TransitionTargetViewCenter, 0.01f))
	{
		// Interpolate the current view center towards the target
		_ViewCenter = FVector2D(FMath::VInterpTo(
			FVector(_ViewCenter, 0.0),
			FVector(_TransitionTargetViewCenter, 0.0),
			DeltaTime,
			PanInterpSpeed
		));

		// Re-clamp the *current* view center after interpolation
		_ViewCenter = GetClampedViewCenter(_ViewCenter);

		bNeedsRedraw = true;
	}

	// Handle smooth rotation
	if (!FMath::IsNearlyEqual(_ViewRotationDegrees, _TransitionTargetViewRotationDegrees, 0.01f))
	{
		// Use RInterpTo. It's designed to find the shortest angular path.
		const FRotator CurrentRot(0.f, _ViewRotationDegrees, 0.f);
		const FRotator TargetRot(0.f, _TransitionTargetViewRotationDegrees, 0.f);
		const FRotator NewRot = FMath::RInterpTo(
			CurrentRot,
			TargetRot,
			DeltaTime,
			PanRotationInterpSpeed
		);

		_ViewRotationDegrees = NewRot.Yaw;

		bNeedsRedraw = true;
	}

	if (bNeedsRedraw)
	{
		RedrawMap();
	}


	return;
}


void UHexGridMinimapWidget::HandleClickOrTap(const FGeometry& InGeometry, const FVector2D& InScreenSpacePosition)
{
	// Get the mouse position in the local space of this widget
	const FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InScreenSpacePosition);

	// Convert that pixel position back into a hex coordinate
	const FHexGridCoordinate ClickedCoordinate = ConvertLocalPositionToHex(LocalMousePosition, InGeometry);

	// Broadcast the event to any listeners
	OnMinimapTileClicked.Broadcast(ClickedCoordinate);


	return;
}


FHexGridCoordinate UHexGridMinimapWidget::ConvertLocalPositionToHex(const FVector2D& InLocalPosition, const FGeometry& InGeometry) const
{
	if (FMath::IsNearlyZero(ZoomScalar))
	{
		L_ERR(HexGrid, "zoom scalar is zero");
		return FHexGridCoordinate();
	}

	// Reverse the Viewport Math
	const FVector2D WidgetPixelCenter = _CachedWidgetSize / 2.f;
	const float EffectiveTileRadius = GetZoomTileRadius();
	const FVector2D ScaledViewCenter_PixelSpace = GetZoomViewCenter();

	// Find the pixel delta from the widget's center
	const FVector2D DeltaPixelLocation = (InLocalPosition - WidgetPixelCenter).GetRotated(_ViewRotationDegrees);

	// Reconstruct the "world" pixel location of the click (at the current zoom)
	const FVector2D ClickedZoomedPlaneLocation = ScaledViewCenter_PixelSpace + DeltaPixelLocation;

	// Un-zoom the click location to get the base "world pixel" coordinate
	const FVector2D ClickedBasePlaneLocation = ClickedZoomedPlaneLocation / ZoomScalar;

	// Convert Pixel to Hex
	const FVector ClickedWorldLocation(ClickedBasePlaneLocation.X, ClickedBasePlaneLocation.Y, 0.f);
	const FHexGridCoordinate ClickedCoordinate = FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(
		ClickedWorldLocation,
		MinimapTileRadius // Use the base radius for this conversion
	);


	return ClickedCoordinate;
}


FHexGridCoordinate UHexGridMinimapWidget::GetViewCenterCoordinate() const
{
	const FVector ViewCenterWorld(_ViewCenter.X, _ViewCenter.Y, 0.f);


	return FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(ViewCenterWorld, MinimapTileRadius);
}


FHexGridCoordinate UHexGridMinimapWidget::GetTransitionTargetCoordinate() const
{
	const FVector ViewCenterWorld(_TransitionTargetViewCenter.X, _TransitionTargetViewCenter.Y, 0.f);


	return FHexGridCoordinate::GetHexGridCoordinateFromWorldOffsetLocation(ViewCenterWorld, MinimapTileRadius);
}


EHexGridTileVisibility UHexGridMinimapWidget::GetTileVisibility(FHexGridCoordinate const& TileCoordinate) const
{
	if (HasRenderPlayerPerspectiveData() == false)
	{
		L_ERR(HexGrid, "render visibility data not available");
		return EHexGridTileVisibility::visible;
	}


	return _RenderedPlayerPerspectiveData->GetTileVisibility(TileCoordinate);
}


void UHexGridMinimapWidget::GetListedTilesWithVisibility(const EHexGridTileVisibility TileVisibility, FHexGridCoordinateList& OutTileCoordinates) const
{
	if (HasRenderPlayerPerspectiveData() == false)
	{
		L_ERR(HexGrid, "render visibility data not available");
		return;
	}

	_RenderedPlayerPerspectiveData->GetListedTilesWithVisibility(TileVisibility, OutTileCoordinates);


	return;
}


AActor* UHexGridMinimapWidget::GetFirstPresentActorOfClassAtTileCoordinate(UClass* ActorClass, FHexGridCoordinate const& TileCoordinate) const
{
	if (HasRenderTileData() == false)
	{
		L_ERR(HexGrid, "render tile data not available");
		return nullptr;
	}

	if (_RenderedTileData->ContainsTile(TileCoordinate) == false)
	{
		return nullptr;
	}


	return _RenderedTileData->GetRegisterContainingTile(TileCoordinate).GetFirstPresentActorOfClass(ActorClass);
}


void UHexGridMinimapWidget::SetViewRotationDegrees(const float ViewRotationDegrees, bool bForceRedraw)
{
	_ViewRotationDegrees = ViewRotationDegrees;
	_TransitionTargetViewRotationDegrees = ViewRotationDegrees;

	if (bForceRedraw)
	{
		RedrawMap();
	}


	return;
}


void UHexGridMinimapWidget::PanToViewRotationDegrees(const float ViewRotationDegrees)
{
	_TransitionTargetViewRotationDegrees = ViewRotationDegrees;


	return;
}


void UHexGridMinimapWidget::SetRotationClockwise(bool bForceRedraw)
{
	SetViewRotationDegrees(_ViewRotationDegrees + c_ViewRotationChange_Clockwise, bForceRedraw);


	return;
}


void UHexGridMinimapWidget::SetRotateCounterClockwise(bool bForceRedraw)
{
	SetViewRotationDegrees(_ViewRotationDegrees + c_ViewRotationChange_CounterClockwise, bForceRedraw);


	return;
}


void UHexGridMinimapWidget::PanRotateClockwise()
{
	PanToViewRotationDegrees(_ViewRotationDegrees + c_ViewRotationChange_Clockwise);


	return;
}


void UHexGridMinimapWidget::PanRotateCounterClockwise()
{
	PanToViewRotationDegrees(_ViewRotationDegrees + c_ViewRotationChange_CounterClockwise);


	return;
}


float UHexGridMinimapWidget::GetClampedViewRotationDegrees(float InViewRotation)
{
	return FMath::Clamp(InViewRotation, UMathTypes::c_Zero, UMathTypes::c_Circle_Degrees);
}


void UHexGridMinimapWidget::ResetView()
{
	_ViewCenter = FVector2D::ZeroVector;
	_TransitionTargetViewCenter = FVector2D::ZeroVector;
	_ViewRotationDegrees = 0.f;
	ZoomScalar = 1.f;


	return;
}


FVector2D UHexGridMinimapWidget::GetClampedViewCenter(const FVector2D& InViewCenter) const
{
	if (_MapPixelBounds.bIsValid == false ||
		_CachedWidgetSize.IsZero())
	{
		return InViewCenter; // No bounds or no widget size yet, can't clamp
	}

	if (FMath::IsNearlyZero(ZoomScalar))
	{
		L_ERR(HexGrid, "ZoomScalar is zero");
		return FVector2d::ZeroVector;
	}

	// This logic assumes rotation is 0. For rotated clamping, we'd need to use the
	// rotated bounding box of the widget, which is much more complex.
	// For now, we clamp as if rotation is 0, which is generally acceptable.

	const FVector2D ViewSizeInBasePixels = _CachedWidgetSize / ZoomScalar;
	const FVector2D HalfViewSize = ViewSizeInBasePixels / 2.f;

	FVector2D ClampedCenter = InViewCenter;

	// Clamp X-Axis
	const float MapWidth = _MapPixelBounds.Max.X - _MapPixelBounds.Min.X;
	if (MapWidth < ViewSizeInBasePixels.X)
	{
		// Map is narrower than view, center it
		ClampedCenter.X = _MapPixelBounds.Min.X + MapWidth / 2.f;
	}
	else
	{
		// Map is wider, clamp
		const float MinX = _MapPixelBounds.Min.X + HalfViewSize.X - PanBoundaryPadding;
		const float MaxX = _MapPixelBounds.Max.X - HalfViewSize.X + PanBoundaryPadding;
		ClampedCenter.X = FMath::Clamp(InViewCenter.X, MinX, MaxX);
	}

	// Clamp Y-Axis
	const float MapHeight = _MapPixelBounds.Max.Y - _MapPixelBounds.Min.Y;
	if (MapHeight < ViewSizeInBasePixels.Y)
	{
		// Map is shorter than view, center it
		ClampedCenter.Y = _MapPixelBounds.Min.Y + MapHeight / 2.f;
	}
	else
	{
		// Map is taller, clamp
		const float MinY = _MapPixelBounds.Min.Y + HalfViewSize.Y - PanBoundaryPadding;
		const float MaxY = _MapPixelBounds.Max.Y - HalfViewSize.Y + PanBoundaryPadding;
		ClampedCenter.Y = FMath::Clamp(InViewCenter.Y, MinY, MaxY);
	}


	return ClampedCenter;
}


void UHexGridMinimapWidget::SetDynamicIndicator(FName const& Handle, FVector2D const& WorldPlaneLocation, FMinimapIndicatorProperties const& IndicatorProperties)
{
	if (_DynamicIndicatorMap.Contains(Handle))
	{
		_DynamicIndicatorMap[Handle] = FRuntimeMinimapIndicatorProperties(IndicatorProperties, WorldPlaneLocation);
	}
	else
	{
		_DynamicIndicatorMap.Add(Handle, FRuntimeMinimapIndicatorProperties(IndicatorProperties, WorldPlaneLocation));
	}


	return;
}


FRuntimeMinimapIndicatorProperties UHexGridMinimapWidget::GetDynamicIndicatorProperties(FName const& Handle) const
{
	if (_DynamicIndicatorMap.Contains(Handle) == false)
	{
		L_ERR(HexGrid, "handle '{0}' does not exist", Handle);
		return FRuntimeMinimapIndicatorProperties();
	}


	return _DynamicIndicatorMap[Handle];
}


void UHexGridMinimapWidget::ClearDynamicIndicator(FName const& Handle)
{
	if (_DynamicIndicatorMap.Contains(Handle) == false)
	{
		return;
	}

	_DynamicIndicatorMap.Remove(Handle);


	return;
}