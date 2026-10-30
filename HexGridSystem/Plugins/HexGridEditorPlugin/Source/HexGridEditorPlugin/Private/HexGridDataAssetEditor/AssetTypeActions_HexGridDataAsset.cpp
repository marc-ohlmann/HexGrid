// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/AssetTypeActions_HexGridDataAsset.h"

#include "HexGridEditorPlugin.h"
#include "HexGridDataAsset.h"
#include "HexGridDataAssetEditor/HexGridEditorToolkit.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions_HexGridDataAsset"


FText FAssetTypeActions_HexGridDataAsset::GetName() const
{
    return LOCTEXT("AssetName", "Hex Grid Data");
}


FColor FAssetTypeActions_HexGridDataAsset::GetTypeColor() const
{
    return FColor::Cyan;
}


UClass* FAssetTypeActions_HexGridDataAsset::GetSupportedClass() const
{
    return UHexGridDataAsset::StaticClass();
}


void FAssetTypeActions_HexGridDataAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    // This function is called when the user double-clicks the asset
    for (UObject* Obj : InObjects)
    {
        if (UHexGridDataAsset* GridAsset = Cast<UHexGridDataAsset>(Obj))
        {
            // Create and initialize our custom editor
            MakeShared<FHexGridEditorToolkit>()->InitEditor(GridAsset);
        }
    }
}


uint32 FAssetTypeActions_HexGridDataAsset::GetCategories()
{
    // This places your asset in the "Miscellaneous" category in the Content Browser
    return EAssetTypeCategories::Misc;
}


#undef LOCTEXT_NAMESPACE