// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AssetProviderInterface.h"
#include "HexGridTileTypeAsset.generated.h"


class UTexture2D;
class UStaticMesh;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UInstancedStaticMeshComponent;


/**
 * An object that defines a type of hex tile that can be used in a hex grid and its properties
 */
UCLASS()
class HEXGRIDPLUGIN_API UHexGridTileTypeAsset : public UDataAsset, public IAssetProviderInterface
{
	GENERATED_BODY()

public:

	UHexGridTileTypeAsset();

	// IAssetProviderInterface implementation
protected:

	virtual void CollectAssetsToPreload(TArray<FSoftObjectPath>& OutAssets) const override;

	//

public:

	// the mesh used to render the tile type
	UPROPERTY(EditAnywhere, Category = "Hex Tile Data")
	TSoftObjectPtr<UStaticMesh> Mesh;

	// the material applied to the tile mesh
	UPROPERTY(EditAnywhere, Category = "Hex Tile Data")
	TSoftObjectPtr<UMaterialInterface> MeshMaterial;

	// the texture applied to the tile material as a base
	UPROPERTY(EditAnywhere, Category = "Hex Tile Data")
	TSoftObjectPtr<UTexture2D> MeshTexture;

	// the texture used to display the tile as an icon
	UPROPERTY(EditAnywhere, Category = "Hex Tile Data")
	TSoftObjectPtr<UTexture2D> IconImage;

	// clean display name for this tile type
	UPROPERTY(EditAnywhere, Category = "Hex Tile Data")
	FText DisplayName;

	// number of custom floats needed per instanced-static-mesh instance for animating tile materials
	static const int c_NumCustomFloatsPerIsmInstance;

	// index of the custom float data for highlight color - R
	static const int c_CustomFloatsDataIndex_HighlightColor_R;

	// index of the custom float data for highlight color - G
	static const int c_CustomFloatsDataIndex_HighlightColor_G;

	// index of the custom float data for highlight color - B
	static const int c_CustomFloatsDataIndex_HighlightColor_B;

	// index of the custom float data for highlight color - A
	static const int c_CustomFloatsDataIndex_HighlightColor_A;

	// index of the custom float data for highlight intensity
	static const int c_CustomFloatsDataIndex_HighlightIntensity;

	// index of the custom float data for highlight rainbow blend
	static const int c_CustomFloatsDataIndex_HighlightRainbowBlend;

	// material parameter name for the base texture
	static const FName c_MaterialParameterName_BaseTexture;

	// material index used for tile type materials
	static const int c_TileTypeIsmcMaterialIndex;

	// setup a given ism component to render the given tile type's material and returns the created dynamic material instance
	static UMaterialInstanceDynamic* SetupTileIsmComponentMaterials(UObject* Outer, UInstancedStaticMeshComponent* Ismc, const UHexGridTileTypeAsset* TileType);

	// set custom data for color and intensity on the given tile instance for the given instanced static mesh component
	// sends all floats to the GPU in one efficient call
	static void SetTileInstanceMaterialHighlightData(
		UInstancedStaticMeshComponent* Ismc,
		const int InstanceIndex, 
		FLinearColor const& Color, 
		const float Intensity, 
		const float RainbowBlend
	);
};
