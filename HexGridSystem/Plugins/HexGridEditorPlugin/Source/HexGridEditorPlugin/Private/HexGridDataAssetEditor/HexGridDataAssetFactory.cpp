#include "HexGridDataAssetEditor/HexGridDataAssetFactory.h"

#include "HexGridEditorPlugin.h"
#include "HexGridDataAsset.h" // The asset class we want to create


UHexGridDataAssetFactory::UHexGridDataAssetFactory()
{
    // This factory is responsible for creating a new asset
    bCreateNew = true;

    // This will open our custom editor immediately after the asset is created
    bEditAfterNew = true;

    // Tell the editor which class this factory creates
    SupportedClass = UHexGridDataAsset::StaticClass();
}


UObject* UHexGridDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    // Create a new instance of our UHexGridDataAsset class and return it
    return NewObject<UHexGridDataAsset>(InParent, InClass, InName, Flags, Context);
}