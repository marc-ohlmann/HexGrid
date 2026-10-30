// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridFogOfWarComponent.h"

#include "HexGridPlugin.h"
#include "MCommon.h"
#include "HexGrid.h"

#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"


const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogColor_Hidden = FName("FogColor_Hidden");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogColor_Explored = FName("FogColor_Explored");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogMask_Explored = FName("FogMask_Explored");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogMask_Explored_Previous = FName("FogMask_Explored_Previous");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogMask_LineOfSight = FName("FogMask_LineOfSight");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogMask_LineOfSight_Previous = FName("FogMask_LineOfSight_Previous");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_CopyTexture = FName("CopyTexture");
const FName UHexGridFogOfWarComponent::c_RenderTargetParameterName_FogFadeAlpha = FName("FogFadeAlpha");
const int UHexGridFogOfWarComponent::c_FogMaterialIndex = 0;
const float UHexGridFogOfWarComponent::c_GridTileRadiusRevealBrushOffset = 10.f;


UHexGridFogOfWarComponent::UHexGridFogOfWarComponent()
	: Super()
	, RevealBrush(nullptr)
	, RenderTargetResolution(1024, 1024)
	, FogOfWarProperties()
	, _FogRenderTarget(nullptr)
	, _LineOfSightRenderTarget(nullptr)
	, _FogMaterialInstanceDynamic(nullptr)
	, _FogOfWarData()
	, _ActivePerspectivePlayerIndex(-1)
	, _FadingIn_Shroud()
	, _FadingOut_Shroud()
	, _FadingIn_LineOfSight()
	, _FadingOut_LineOfSight()
{

	PrimaryComponentTick.bCanEverTick = true;

	// Disable collision so the fog doesn't block clicks, camera, or pawns
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Disable lighting features (good for performance)
	SetCastShadow(false);

	// Ensure the fog doesn't affect navigation generation
	SetCanEverAffectNavigation(false);
}


void UHexGridFogOfWarComponent::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
	IAssetProviderInterface::CollectPreloadAsset(RevealBrush, OutAssets);


	return;
}


void UHexGridFogOfWarComponent::BeginPlay()
{
	Super::BeginPlay();


	return;
}


void UHexGridFogOfWarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OnTick_RenderFadingTiles(DeltaTime);


	return;
}


AHexGrid* UHexGridFogOfWarComponent::GetOwnerHexGrid() const
{
	return GetOwner<AHexGrid>();
}


void UHexGridFogOfWarComponent::InitializeFogOfWar(FFogOfWarSetupProperties const& SetupProperties)
{
	FogOfWarProperties = SetupProperties;

	InitializeFogMaterial(SetupProperties);
	InitializeToGrid();


	return;
}


void UHexGridFogOfWarComponent::InitializeToGrid()
{
	AHexGrid* OwnerGrid = GetOwnerHexGrid();
	IF_NULL_RETURN_LERR(OwnerGrid, HexGrid, "OwnerGrid is null");

	const FBox2D GridBounds = OwnerGrid->GetGridBounds();
	TransformToFitBounds(GridBounds);


	return;
}


void UHexGridFogOfWarComponent::SetupRenderTargets()
{
	if (UMCommon::IsUObjectPointerSafe(_FogRenderTarget) == false)
	{
		_FogRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), RenderTargetResolution.X, RenderTargetResolution.Y);
		if (UMCommon::IsUObjectPointerSafe(_FogRenderTarget))
		{
			_FogRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // RGBA format is safe for mobile
			_FogRenderTarget->bSupportsUAV = false;
			_FogRenderTarget->bForceLinearGamma = true; // CRITICAL: Disable sRGB. Vulkan bans sRGB formats from being used as Storage Images.
			_FogRenderTarget->ClearColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
			_FogRenderTarget->InitCustomFormat(RenderTargetResolution.X, RenderTargetResolution.Y, EPixelFormat::PF_R8G8B8A8, false); // RGBA format is safe for mobile
			_FogRenderTarget->UpdateResourceImmediate(true);
		}
		else
		{
			L_ERR(HexGrid, "failed to instantiate UCanvasRenderTarget2D for fog of size '{0}'", RenderTargetResolution);
		}
	}

	if (UMCommon::IsUObjectPointerSafe(_LineOfSightRenderTarget) == false)
	{
		_LineOfSightRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), RenderTargetResolution.X, RenderTargetResolution.Y);
		if (UMCommon::IsUObjectPointerSafe(_LineOfSightRenderTarget))
		{
			_LineOfSightRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // RGBA format is safe for mobile
			_LineOfSightRenderTarget->bSupportsUAV = false;
			_LineOfSightRenderTarget->bForceLinearGamma = true; // CRITICAL: Disable sRGB. Vulkan bans sRGB formats from being used as Storage Images.
			_LineOfSightRenderTarget->ClearColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
			_LineOfSightRenderTarget->InitCustomFormat(RenderTargetResolution.X, RenderTargetResolution.Y, EPixelFormat::PF_R8G8B8A8, false); // RGBA format is safe for mobile
			_LineOfSightRenderTarget->UpdateResourceImmediate(true);
		}
		else
		{
			L_ERR(HexGrid, "failed to instantiate UCanvasRenderTarget2D for line of sight of size '{0}'", RenderTargetResolution);
		}
	}


	return;
}


void UHexGridFogOfWarComponent::InitializeFogMaterial(FFogOfWarSetupProperties const& SetupProperties)
{
	SetupRenderTargets();

	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	UMaterialInterface* BaseMaterial = GetMaterial(c_FogMaterialIndex);
	IF_NULL_RETURN_LERR(BaseMaterial, HexGrid, "BaseMaterial is null");

	_FogMaterialInstanceDynamic = CreateDynamicMaterialInstance(c_FogMaterialIndex, BaseMaterial);
	IF_NULL_RETURN_LERR(_FogMaterialInstanceDynamic, HexGrid, "FogMaterialInstanceDynamic is null");

	UpdateFogTextureParameters();

	// Also set the "FadeAlpha" to 1.0 (fully visible) in the dynamic material
	SetFogFadeAlpha(1.f);

	// set the fog color vector parameters
	SetFogColor(SetupProperties.FogColor_Hidden, SetupProperties.FogColor_Explored);

	// Apply the new dynamic material to this component.
	SetMaterial(c_FogMaterialIndex, _FogMaterialInstanceDynamic);


	return;
}


void UHexGridFogOfWarComponent::SetFogColor(FLinearColor const& FogColor_Hidden, FLinearColor const& FogColor_Explored)
{
	IF_NULL_RETURN_LERR(_FogMaterialInstanceDynamic, HexGrid, "FogMaterialInstanceDynamic is null");

	FogOfWarProperties.FogColor_Hidden = FogColor_Hidden;
	FogOfWarProperties.FogColor_Explored = FogColor_Explored;

	// Set the "FogColor" vector parameter in the material to our render target.
	_FogMaterialInstanceDynamic->SetVectorParameterValue(c_RenderTargetParameterName_FogColor_Hidden, FogColor_Hidden);

	// Set the "FogColor" vector parameter in the material to our render target.
	_FogMaterialInstanceDynamic->SetVectorParameterValue(c_RenderTargetParameterName_FogColor_Explored, FogColor_Explored);


	return;
}


void UHexGridFogOfWarComponent::UpdateVisibilityAtCoordinatesForPerspectivePlayer(const int PerspectivePlayerIndex, FHexGridTileVisibilityGroupMap const& TileVisibilityGroups)
{
	if (IsValidPlayerIndex(PerspectivePlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PerspectivePlayerIndex);
		return;
	}

	// Directly iterate the map, bypassing the key array allocation and hash lookup
	FOR_MAP_CONST(VisibilityGroupPair, TileVisibilityGroups.VisibilityGroupMap)
	{
		const EHexGridTileVisibility NewState = VisibilityGroupPair.Key;
		const FHexGridCoordinateList& Coords = VisibilityGroupPair.Value;

		for (const FHexGridCoordinate& Coord : Coords.List)
		{
			const EHexGridTileVisibility OldState = GetTileVisibilityForPlayer(PerspectivePlayerIndex, Coord);

			if (OldState == NewState)
			{
				continue;
			}

			// Update the "Truth" data only
			SetTileVisibilityForPlayer(PerspectivePlayerIndex, Coord, NewState);
		}
	}


	return;
}


void UHexGridFogOfWarComponent::ApplyVisibilityAtCoordinatesForActivePerspectivePlayer(FHexGridTileVisibilityGroupMap const& TileVisibilityGroups)
{
	UWorld* World = GetWorld();
	IF_NULL_RETURN_LERR(World, CivCraft, "World is null");
	
	if (HasActivePersepectivePlayer() == false)
	{
		L_ERR(HexGrid, "Cannot apply visibility, no active player perspective set.");
		return;
	}
	
	const float CurrentTime = World->GetTimeSeconds();
	PrimaryComponentTick.SetTickFunctionEnable(true);

	FOR_MAP_CONST(VisibilityGroupPair, TileVisibilityGroups.VisibilityGroupMap)
	{
		const EHexGridTileVisibility NewState = VisibilityGroupPair.Key;
		const FHexGridCoordinateList& Coords = VisibilityGroupPair.Value;

		FOR_FORWARD(i, Coords.List.Num())
		{
			const FHexGridCoordinate& Coord = Coords.List[i];
			const EHexGridTileVisibility OldState = GetTileVisibilityForPlayer(_ActivePerspectivePlayerIndex, Coord);

			if (OldState == NewState)
			{
				continue;
			}

			// Update the underlying data
			SetTileVisibilityForPlayer(_ActivePerspectivePlayerIndex, Coord, NewState);

			// Manage Visual Shroud/Fading (Specific to "Apply")

			// Shroud Fading
			if ((NewState == EHexGridTileVisibility::partial || NewState == EHexGridTileVisibility::visible) && OldState == EHexGridTileVisibility::hidden)
			{
				_FadingOut_Shroud.Remove(Coord);
				_FadingIn_Shroud.Add(Coord, CurrentTime);
			}
			else if (NewState == EHexGridTileVisibility::hidden && (OldState == EHexGridTileVisibility::partial || OldState == EHexGridTileVisibility::visible))
			{
				_FadingIn_Shroud.Remove(Coord);
				_FadingOut_Shroud.Add(Coord, CurrentTime);
			}

			// LineOfSight Fading
			if (NewState == EHexGridTileVisibility::visible && OldState != EHexGridTileVisibility::visible)
			{
				_FadingOut_LineOfSight.Remove(Coord);
				_FadingIn_LineOfSight.Add(Coord, CurrentTime);
			}
			else if ((NewState == EHexGridTileVisibility::partial || NewState == EHexGridTileVisibility::hidden) && OldState == EHexGridTileVisibility::visible)
			{
				_FadingIn_LineOfSight.Remove(Coord);
				_FadingOut_LineOfSight.Add(Coord, CurrentTime);
			}
		}
	}


	return;
}


void UHexGridFogOfWarComponent::UpdateFogTextureParameters()
{
	IF_NULL_RETURN_LERR(_FogMaterialInstanceDynamic, HexGrid, "FogMaterialInstanceDynamic is null");
	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	_FogMaterialInstanceDynamic->SetTextureParameterValue(c_RenderTargetParameterName_FogMask_Explored, _FogRenderTarget);
	_FogMaterialInstanceDynamic->SetTextureParameterValue(c_RenderTargetParameterName_FogMask_LineOfSight, _LineOfSightRenderTarget);


	return;
}


void UHexGridFogOfWarComponent::SetActivePerspectivePlayer(const int PlayerIndex)
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return;
	}

	const int OldPlayerIndex = GetActivePerspectivePlayerIndex();

	_ActivePerspectivePlayerIndex = PlayerIndex;

	if (OldPlayerIndex != PlayerIndex)
	{
		// clear datasets being processed on tick
		_FadingIn_Shroud.Reset();
		_FadingOut_Shroud.Reset();
		_FadingIn_LineOfSight.Reset();
		_FadingOut_LineOfSight.Reset();

		if (OnActivePerspectivePlayerIndexChanged.IsBound())
		{
			OnActivePerspectivePlayerIndexChanged.Broadcast(OldPlayerIndex, PlayerIndex);
		}
	}

	RenderPlayerPerspective(PlayerIndex);


	return;
}


void UHexGridFogOfWarComponent::ReloadActivePerspectivePlayer()
{
	if (HasActivePersepectivePlayer() == false)
	{
		L_ERR(HexGrid, "invalid active player index '{0}'", _ActivePerspectivePlayerIndex);
		return;
	}

	RenderPlayerPerspective(_ActivePerspectivePlayerIndex);


	return;
}


void UHexGridFogOfWarComponent::TransformToFitBounds(FBox2D const& TargetBounds)
{
	// Calculate Bounds
	FVector2D Origin;
	FVector2D BoxExtent;
	TargetBounds.GetCenterAndExtents(Origin, BoxExtent);

	// Apply Padding
	// Add the optional padding to the extents to ensure full coverage.
	BoxExtent += FVector2D(FogOfWarProperties.FogBoundsPadding, FogOfWarProperties.FogBoundsPadding);

	// Position the Fog Plane
	// Set our location to match the center of the target actor's bounds.
	// We raise it slightly on the Z-axis to avoid Z-fighting with the grid itself.
	const FVector NewLocation = FVector(Origin.X, Origin.Y, FogOfWarProperties.FogHeight);
	SetWorldLocation(NewLocation);

	// Scale the Fog Plane
	// A default Plane mesh in Unreal is 100x100 world units. The BoxExtent is half the total size.
	// So, the required scale is the total size (BoxExtent * 2) divided by the mesh size (100).
	// This simplifies to BoxExtent / 50.
	const FVector NewScale(BoxExtent.X / 50.f, BoxExtent.Y / 50.f, 1.f);
	SetWorldScale3D(NewScale);


	return;
}


void UHexGridFogOfWarComponent::GetFogOfWarPerspectives(FFogOfWarPerspectiveMap& OutData) const
{
	OutData = FFogOfWarPerspectiveMap(_FogOfWarData.PlayerPerspectives);


	return;
}


void UHexGridFogOfWarComponent::SetFogOfWarPerspectives(FFogOfWarPerspectiveMap const& FogOfWarData)
{
	_FogOfWarData = FFogOfWarPerspectiveMap(FogOfWarData.PlayerPerspectives);


	return;
}


void UHexGridFogOfWarComponent::RegisterPlayerPerspective(const int PlayerIndex)
{
	if (IsValidPlayerIndex(PlayerIndex))
	{
		L_WARN(HexGrid, "player index '{0}' already registered", PlayerIndex);
		return;
	}

	_FogOfWarData.PlayerPerspectives.Add(PlayerIndex, FFogOfWarPerspectiveData());


	return;
}


EHexGridTileVisibility UHexGridFogOfWarComponent::GetTileVisibilityForPlayer(const int PlayerIndex, FHexGridCoordinate const& Coordinate) const
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return EHexGridTileVisibility::hidden;
	}


	return _FogOfWarData.GetTileVisibilityForPlayer(PlayerIndex, Coordinate);
}


EHexGridTileVisibility UHexGridFogOfWarComponent::GetTileVisibilityForActivePlayer(FHexGridCoordinate const& Coordinate) const
{
	const int ActivePerspectivePlayerIndex = GetActivePerspectivePlayerIndex();


	return GetTileVisibilityForPlayer(ActivePerspectivePlayerIndex, Coordinate);
}


void UHexGridFogOfWarComponent::SetTileVisibilityForPlayer(const int PlayerIndex, FHexGridCoordinate const& Coordinate, const EHexGridTileVisibility VisibilityState)
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return;
	}

	const EHexGridTileVisibility OldVisibilityState = _FogOfWarData.GetTileVisibilityForPlayer(PlayerIndex, Coordinate);

	_FogOfWarData.SetTileVisibilityForPlayer(PlayerIndex, Coordinate, VisibilityState);

	if (VisibilityState != OldVisibilityState)
	{
		HandlePlayerTileVisibilityDataChanged(PlayerIndex, Coordinate, OldVisibilityState, VisibilityState);

		if (OnPlayerVisibilityDataChanged.IsBound())
		{
			OnPlayerVisibilityDataChanged.Broadcast(PlayerIndex, Coordinate, OldVisibilityState, VisibilityState);
		}
	}


	return;
}


void UHexGridFogOfWarComponent::HandlePlayerTileVisibilityDataChanged(
	const int PlayerIndex,
	FHexGridCoordinate const& Coordinate,
	const EHexGridTileVisibility OldVisibilityState,
	const EHexGridTileVisibility NewVisibilityState
)
{
	return;
}


FFogOfWarPerspectiveData UHexGridFogOfWarComponent::GetPlayerPerspectiveData(const int PlayerIndex) const
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		L_WARN(HexGrid, "player index '{0}' already registered");
		return FFogOfWarPerspectiveData();
	}


	return _FogOfWarData.PlayerPerspectives[PlayerIndex];
}


void UHexGridFogOfWarComponent::SetPlayerPerspectiveData(const int PlayerIndex, FFogOfWarPerspectiveData const& PerspectiveData)
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		RegisterPlayerPerspective(PlayerIndex);
	}

	_FogOfWarData.PlayerPerspectives[PlayerIndex] = PerspectiveData;


	return;
}


void UHexGridFogOfWarComponent::RenderPlayerPerspective(const int PlayerIndex)
{
	ClearShroud();
	ClearLineOfSight();

	const FFogOfWarPerspectiveData PlayerPerspective = GetPlayerPerspectiveData(PlayerIndex);
	const FHexGridTileVisibilityGroupMap TileVisibilityGroups = FHexGridTileVisibilityGroupMap(PlayerPerspective.TileVisibilityMap);

	RenderShroud(TileVisibilityGroups);
	RenderLineOfSight(TileVisibilityGroups); 

	UpdateFogTextureParameters();


	return;
}


float UHexGridFogOfWarComponent::GetGridTileRadius() const
{
	AHexGrid* OwnerGrid = GetOwnerHexGrid();
	IF_NULL_RETURN_VAL_LERR(OwnerGrid, 0.f, HexGrid, "OwnerGrid is null");


	return OwnerGrid->GetHexGridProperties().TileRadius;
}


void UHexGridFogOfWarComponent::GetListedTilesWithVisibilityForPlayer(
	const int PlayerIndex,
	const EHexGridTileVisibility VisibilityState,
	FHexGridCoordinateList& OutCoordinates
) const
{
	if (IsValidPlayerIndex(PlayerIndex) == false)
	{
		L_ERR(HexGrid, "invalid player index '{0}'", PlayerIndex);
		return;
	}

	_FogOfWarData.GetListedTilesWithVisibilityForPlayer(PlayerIndex, VisibilityState, OutCoordinates);


	return;
}


void UHexGridFogOfWarComponent::ClearLineOfSight()
{
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	UKismetRenderingLibrary::ClearRenderTarget2D(this, _LineOfSightRenderTarget, FLinearColor::Black);


	return;
}


void UHexGridFogOfWarComponent::ClearShroud()
{
	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");

	UKismetRenderingLibrary::ClearRenderTarget2D(this, _FogRenderTarget, FLinearColor::Black);


	return;
}


void UHexGridFogOfWarComponent::RenderShroud(const FHexGridTileVisibilityGroupMap& TileVisibilityGroups)
{
	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");

	UTexture2D* RevealBrushTexture = UMCommon::EnsureSoftObjectPtrLoaded(RevealBrush);
	IF_NULL_RETURN_LERR(RevealBrushTexture, HexGrid, "RevealBrush is null");

	// We need to stamp all tiles that are *not hidden*
	const bool bHasPartial = TileVisibilityGroups.VisibilityGroupMap.Contains(EHexGridTileVisibility::partial);
	const bool bHasVisible = TileVisibilityGroups.VisibilityGroupMap.Contains(EHexGridTileVisibility::visible);

	if (bHasPartial)
	{
		for (const FHexGridCoordinate& Coord : TileVisibilityGroups.VisibilityGroupMap[EHexGridTileVisibility::partial].List)
		{
			// Draw a full, non-faded white stamp
			DrawTextureToTarget(_FogRenderTarget, Coord, FLinearColor::White);
		}
	}

	if (bHasVisible)
	{
		for (const FHexGridCoordinate& Coord : TileVisibilityGroups.VisibilityGroupMap[EHexGridTileVisibility::visible].List)
		{
			// Draw a full, non-faded white stamp
			DrawTextureToTarget(_FogRenderTarget, Coord, FLinearColor::White);
		}
	}


	return;
}


void UHexGridFogOfWarComponent::RenderLineOfSight(const FHexGridTileVisibilityGroupMap& TileVisibilityGroups)
{
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	UTexture2D* RevealBrushTexture = UMCommon::EnsureSoftObjectPtrLoaded(RevealBrush);
	IF_NULL_RETURN_LERR(RevealBrushTexture, HexGrid, "RevealBrush is null");

	// We only need to stamp *visible* tiles
	if (TileVisibilityGroups.VisibilityGroupMap.Contains(EHexGridTileVisibility::visible))
	{
		for (const FHexGridCoordinate& Coord : TileVisibilityGroups.VisibilityGroupMap[EHexGridTileVisibility::visible].List)
		{
			// Draw a full, non-faded white stamp
			DrawTextureToTarget(_LineOfSightRenderTarget, Coord, FLinearColor::White);
		}
	}


	return;
}




void UHexGridFogOfWarComponent::ReadRawFogOfWarPixelVisibilityData(TArray<FColor>& OutShroudPixelData, TArray<FColor>& OutLosPixelData) const
{
	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	ReadRenderTargetRawPixelVisibilityData(this, _FogRenderTarget, OutShroudPixelData);
	ReadRenderTargetRawPixelVisibilityData(this, _LineOfSightRenderTarget, OutLosPixelData);


	return;
}


void UHexGridFogOfWarComponent::ApplyRawFogOfWarPixelVisibilityData(TArray<FColor> const& ShroudPixelData, TArray<FColor> const& LosPixelData)
{
	IF_NULL_RETURN_LERR(_FogRenderTarget, HexGrid, "FogRenderTarget is null");
	IF_NULL_RETURN_LERR(_LineOfSightRenderTarget, HexGrid, "LineOfSightRenderTarget is null");

	ApplyRenderTargetRawPixelVisibilityData(this, _FogRenderTarget, ShroudPixelData);
	ApplyRenderTargetRawPixelVisibilityData(this, _LineOfSightRenderTarget, LosPixelData);


	return;
}


void UHexGridFogOfWarComponent::ReadRenderTargetRawPixelVisibilityData(const UObject* WorldContextObject, UCanvasRenderTarget2D* RenderTarget, TArray<FColor>& OutRawPixelVisibilityData)
{
	IF_NULL_RETURN_LERR(WorldContextObject, HexGrid, "WorldContextObject is null");
	IF_NULL_RETURN_LERR(RenderTarget, HexGrid, "RenderTarget is null");

	// the UKismetRenderingLibrary fails to be const correct, even though it uses the world context in a const-safe way
	UObject* NonConstWorldContextObject = const_cast<UObject*>(WorldContextObject);
	IF_NULL_RETURN_LERR(NonConstWorldContextObject, HexGrid, "NonConstWorldContextObject is null");

	// Read the raw pixel data from the render target into our TArray.
	const bool bResult = UKismetRenderingLibrary::ReadRenderTarget(NonConstWorldContextObject, RenderTarget, OutRawPixelVisibilityData, true);


	return;
}


void UHexGridFogOfWarComponent::ApplyRenderTargetRawPixelVisibilityData(const UObject* WorldContextObject, UCanvasRenderTarget2D* RenderTarget, TArray<FColor> const& RawPixelVisibilityData)
{
	IF_NULL_RETURN_LERR(WorldContextObject, HexGrid, "WorldContextObject is null");
	IF_NULL_RETURN_LERR(RenderTarget, HexGrid, "RenderTarget is null");

	// the UKismetRenderingLibrary fails to be const correct, even though it uses the world context in a const-safe way
	UObject* NonConstWorldContextObject = const_cast<UObject*>(WorldContextObject);
	IF_NULL_RETURN_LERR(NonConstWorldContextObject, HexGrid, "NonConstWorldContextObject is null");

	if (RawPixelVisibilityData.IsEmpty())
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(NonConstWorldContextObject, RenderTarget, FLinearColor::Black);
		return;
	}

	// Create a temporary, transient texture to hold our cached data.
	const int32 TextureWidth = RenderTarget->SizeX;
	const int32 TextureHeight = RenderTarget->SizeY;
	UTexture2D* TempTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_B8G8R8A8);
	IF_NULL_RETURN_LERR(TempTexture, HexGrid, "failed to create transient texture for applying state");

	// Copy Pixel Data to the Transient Texture
	void* TextureData = TempTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RawPixelVisibilityData.GetData(), RawPixelVisibilityData.Num() * sizeof(FColor));
	TempTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	TempTexture->UpdateResource();

	// Draw the Transient Texture to the Render Target
	FDrawToRenderTargetContext Context;
	UCanvas* Canvas;
	FVector2D Size;

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(NonConstWorldContextObject, RenderTarget, Canvas, Size, Context);
	if (Canvas)
	{
		// Draw our temporary texture to completely overwrite the render target.
		Canvas->K2_DrawTexture(
			TempTexture,
			FVector2D::ZeroVector,
			Size,
			FVector2D::ZeroVector,
			FVector2D::UnitVector,
			FLinearColor::White,
			EBlendMode::BLEND_Opaque
		);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(NonConstWorldContextObject, Context);


	return;
}


void UHexGridFogOfWarComponent::OnTick_RenderFadingTiles(float DeltaSeconds)
{
	IF_NULL_RETURN(_FogRenderTarget);
	IF_NULL_RETURN(_LineOfSightRenderTarget);

	UTexture2D* RevealBrushTexture = UMCommon::EnsureSoftObjectPtrLoaded(RevealBrush);
	IF_NULL_RETURN_LERR(RevealBrushTexture, HexGrid, "RevealBrush is null");

	UWorld* World = GetWorld();
	IF_NULL_RETURN(World);

	const float CurrentTime = World->GetTimeSeconds();
	bool bIsStillFading = false;

	// Process Fading-IN Shroud
	for (auto It = _FadingIn_Shroud.CreateIterator(); It; ++It)
	{
		const float Elapsed = CurrentTime - It.Value();
		const float Alpha = FMath::Clamp(Elapsed / FogOfWarProperties.FogFadeDuration, 0.f, 1.f);
		DrawTextureToTarget(_FogRenderTarget, It.Key(), FLinearColor(1, 1, 1, Alpha));

		if (Alpha < 1.f) { bIsStillFading = true; }
		else { It.RemoveCurrent(); }
	}

	// Process Fading-OUT Shroud
	for (auto It = _FadingOut_Shroud.CreateIterator(); It; ++It)
	{
		const float Elapsed = CurrentTime - It.Value();
		const float Alpha = FMath::Clamp(Elapsed / FogOfWarProperties.FogFadeDuration, 0.f, 1.f);
		DrawTextureToTarget(_FogRenderTarget, It.Key(), FLinearColor(0, 0, 0, Alpha));

		if (Alpha < 1.f) { bIsStillFading = true; }
		else { It.RemoveCurrent(); }
	}

	// Process Fading-IN LineOfSight
	for (auto It = _FadingIn_LineOfSight.CreateIterator(); It; ++It)
	{
		const float Elapsed = CurrentTime - It.Value();
		const float Alpha = FMath::Clamp(Elapsed / FogOfWarProperties.FogFadeDuration, 0.f, 1.f);
		DrawTextureToTarget(_LineOfSightRenderTarget, It.Key(), FLinearColor(1, 1, 1, Alpha));

		if (Alpha < 1.f) { bIsStillFading = true; }
		else { It.RemoveCurrent(); }
	}

	// Process Fading-OUT LineOfSight
	for (auto It = _FadingOut_LineOfSight.CreateIterator(); It; ++It)
	{
		const float Elapsed = CurrentTime - It.Value();
		const float Alpha = FMath::Clamp(Elapsed / FogOfWarProperties.FogFadeDuration, 0.f, 1.f);
		DrawTextureToTarget(_LineOfSightRenderTarget, It.Key(), FLinearColor(0, 0, 0, Alpha));

		if (Alpha < 1.f) { bIsStillFading = true; }
		else { It.RemoveCurrent(); }
	}

	// If no more tiles are fading, turn off tick
	if (bIsStillFading == false)
	{
		_FadingIn_Shroud.Empty();
		_FadingOut_Shroud.Empty();
		_FadingIn_LineOfSight.Empty();
		_FadingOut_LineOfSight.Empty();
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}


	return;
}


void UHexGridFogOfWarComponent::SetFogFadeAlpha(float Alpha)
{
	IF_NULL_RETURN_LERR(_FogMaterialInstanceDynamic, HexGrid, "_FogMaterialInstanceDynamic is null");

	_FogMaterialInstanceDynamic->SetScalarParameterValue(c_RenderTargetParameterName_FogFadeAlpha, Alpha);


	return;
}


void UHexGridFogOfWarComponent::DrawTextureToTarget(
	UCanvasRenderTarget2D* Target,
	const FHexGridCoordinate& Coordinate,
	const FLinearColor& TintColor)
{
	IF_NULL_RETURN_LERR(Target, HexGrid, "Target is null");

	UTexture2D* RevealBrushTexture = UMCommon::EnsureSoftObjectPtrLoaded(RevealBrush);
	IF_NULL_RETURN_LERR(RevealBrushTexture, HexGrid, "RevealBrush is null");

	const float GridTileRadius = GetGridTileRadius();
	const FVector WorldLocation = Coordinate.GetCenterWorldOffsetLocation(GridTileRadius);

	FDrawToRenderTargetContext Context;
	UCanvas* Canvas;
	FVector2D Size;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, Target, Canvas, Size, Context);
	if (UMCommon::IsUObjectPointerSafe(Canvas) == false)
	{
		L_ERR(HexGrid, "Canvas is null");
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
		return;
	}

	// Calculations (copied from your old RenderShroud)
	const FVector ComponentLocation = GetComponentLocation();
	const FBoxSphereBounds WorldBounds = CalcBounds(GetComponentTransform());
	const FVector ComponentBoundsExtent = WorldBounds.BoxExtent;
	const float WorldToPixelScale = Target->SizeX / (ComponentBoundsExtent.X * 2.f);
	const float RevealBrushSize = FogOfWarProperties.RevealBrushTileRadiusScalar * GetGridTileRadius();
	const FVector2D DrawSize(RevealBrushSize * WorldToPixelScale, RevealBrushSize * WorldToPixelScale);

	const float U = (WorldLocation.X - (ComponentLocation.X - ComponentBoundsExtent.X)) / (ComponentBoundsExtent.X * 2.f);
	const float V = (WorldLocation.Y - (ComponentLocation.Y - ComponentBoundsExtent.Y)) / (ComponentBoundsExtent.Y * 2.f);
	FVector2D DrawPosition(U * Target->SizeX, V * Target->SizeY);
	DrawPosition -= DrawSize / 2.f;

	// This is the key: we draw the texture with a translucent blend
	// and use the TintColor's Alpha to control the fade.
	Canvas->K2_DrawTexture(
		RevealBrushTexture,
		DrawPosition,
		DrawSize,
		FVector2D::ZeroVector,
		FVector2D::UnitVector,
		TintColor,
		EBlendMode::BLEND_Translucent // <-- This is crucial
	);

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
}


void UHexGridFogOfWarComponent::GetListedTilesWithVisibilityForActivePlayer(
	const EHexGridTileVisibility VisibilityState,
	FHexGridCoordinateList& OutCoordinates
) const
{
	const int ActivePlayerIndex = GetActivePerspectivePlayerIndex();
	GetListedTilesWithVisibilityForPlayer(ActivePlayerIndex, VisibilityState, OutCoordinates);


	return;
}


bool UHexGridFogOfWarComponent::HasActivePersepectivePlayer() const
{
	return IsValidPlayerIndex(_ActivePerspectivePlayerIndex);
}