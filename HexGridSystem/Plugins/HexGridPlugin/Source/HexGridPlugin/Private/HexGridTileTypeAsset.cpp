// Copyright 2025 Marc Ohlmann, All Rights Reserved.


#include "HexGridTileTypeAsset.h"

#include "HexGridPlugin.h"
#include "MCommon.h"

#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


// five floats used per instance to animate tile materials
// 1) highlight color R
// 2) highlight color G
// 3) highlight color B
// 4) highlight color A
// 5) highlight intensity - alpha value used to blend between base texture and highlight color
// 6) highlight rainbow blend - alpha value used to blend the highlight color between the base color and rainbow color
const int UHexGridTileTypeAsset::c_NumCustomFloatsPerIsmInstance = 6;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightColor_R = 0;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightColor_G = 1;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightColor_B = 2;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightColor_A = 3;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightIntensity = 4;
const int UHexGridTileTypeAsset::c_CustomFloatsDataIndex_HighlightRainbowBlend = 5;

const int UHexGridTileTypeAsset::c_TileTypeIsmcMaterialIndex = 0;
const FName UHexGridTileTypeAsset::c_MaterialParameterName_BaseTexture = FName("BaseTexture");


UHexGridTileTypeAsset::UHexGridTileTypeAsset()
	: Mesh(nullptr)
	, MeshMaterial(nullptr)
	, MeshTexture(nullptr)
	, IconImage(nullptr)
	, DisplayName()
{
}


void UHexGridTileTypeAsset::CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const
{
    IAssetProviderInterface::CollectPreloadAsset(Mesh, OutAssets);
    IAssetProviderInterface::CollectPreloadAsset(MeshMaterial, OutAssets);
    IAssetProviderInterface::CollectPreloadAsset(MeshTexture, OutAssets);
    IAssetProviderInterface::CollectPreloadAsset(IconImage, OutAssets);


    return;
}


UMaterialInstanceDynamic* UHexGridTileTypeAsset::SetupTileIsmComponentMaterials(UObject* Outer, UInstancedStaticMeshComponent* Ismc, const UHexGridTileTypeAsset* TileType)
{
    IF_NULL_RETURN_VAL_LERR(Outer, nullptr, HexGridEditor, "Outer is null");
    IF_NULL_RETURN_VAL_LERR(Ismc, nullptr, HexGridEditor, "Ismc is null");
    IF_NULL_RETURN_VAL_LERR(TileType, nullptr, HexGridEditor, "TileType is null");

    UMaterialInterface* BaseMaterial = UMCommon::EnsureSoftObjectPtrLoaded(TileType->MeshMaterial);
    IF_NULL_RETURN_VAL_LERR(BaseMaterial, nullptr, HexGridEditor, "BaseMaterial is null");

    UMaterialInstanceDynamic* NewMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(BaseMaterial, Outer);
    IF_NULL_RETURN_VAL_LERR(NewMaterialInstanceDynamic, nullptr, HexGridEditor, "NewMaterialInstanceDynamic is null");

    UStaticMesh* StaticMesh = UMCommon::EnsureSoftObjectPtrLoaded(TileType->Mesh);
    IF_NULL_RETURN_VAL_LERR(StaticMesh, nullptr, HexGridEditor, "StaticMesh is null");

    UTexture2D* BaseTexture = UMCommon::EnsureSoftObjectPtrLoaded(TileType->MeshTexture);
    IF_NULL_RETURN_VAL_LERR(BaseTexture, nullptr, HexGridEditor, "BaseTexture is null");

    // Set the per-tile-type BaseTexture parameter on the material
    NewMaterialInstanceDynamic->SetTextureParameterValue(UHexGridTileTypeAsset::c_MaterialParameterName_BaseTexture, BaseTexture);

    // setup the ismc
    Ismc->SetStaticMesh(StaticMesh);
    Ismc->SetMaterial(c_TileTypeIsmcMaterialIndex, NewMaterialInstanceDynamic);
    Ismc->SetNumCustomDataFloats(UHexGridTileTypeAsset::c_NumCustomFloatsPerIsmInstance);


    return NewMaterialInstanceDynamic;
}


void UHexGridTileTypeAsset::SetTileInstanceMaterialHighlightData(
    UInstancedStaticMeshComponent* Ismc, 
    const int InstanceIndex, 
    FLinearColor const& Color, 
    const float Intensity, 
    const float RainbowBlend
)
{
    IF_NULL_RETURN_LERR(Ismc, HexGrid, "Ismc is null");

    if (Ismc->IsValidInstance(InstanceIndex) == false)
    {
        L_ERR(HexGrid, "Ismc {0} invalid instance id {1}", Ismc, InstanceIndex);
        return;
    }

    if (Ismc->NumCustomDataFloats != c_NumCustomFloatsPerIsmInstance)
    {
        L_ERR(HexGrid, "Ismc {0} not initialized", Ismc);
        return;
    }

    TArray<float> CustomData;
    CustomData.SetNumUninitialized(c_NumCustomFloatsPerIsmInstance);

    CustomData[c_CustomFloatsDataIndex_HighlightColor_R] = Color.R;
    CustomData[c_CustomFloatsDataIndex_HighlightColor_G] = Color.G;
    CustomData[c_CustomFloatsDataIndex_HighlightColor_B] = Color.B;
    CustomData[c_CustomFloatsDataIndex_HighlightColor_A] = Color.A;
    CustomData[c_CustomFloatsDataIndex_HighlightIntensity] = Intensity;
    CustomData[c_CustomFloatsDataIndex_HighlightRainbowBlend] = RainbowBlend;

    Ismc->SetCustomData(InstanceIndex, CustomData, true);


    return;
}