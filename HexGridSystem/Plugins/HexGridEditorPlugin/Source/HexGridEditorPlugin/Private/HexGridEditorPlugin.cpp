// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridEditorPlugin.h"

#include "HexGridDataAssetEditor/AssetTypeActions_HexGridDataAsset.h"
#include "HexGridDataAssetEditor/HexGridEditorMode.h"
#include "HexGridDataAssetEditor/HexGridEditorCommands.h"

#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "EditorModes.h"
#include "EditorStyleSet.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "FHexGridEditorPluginModule"

DEFINE_LOG_CATEGORY(HexGridEditor);

void FHexGridEditorPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Register Asset Type Actions
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	HexGridDataAssetActions = MakeShared<FAssetTypeActions_HexGridDataAsset>();
    AssetTools.RegisterAssetTypeActions(HexGridDataAssetActions.ToSharedRef());

    // register editor ui commands
    FHexGridEditorCommands::Register();


}

void FHexGridEditorPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

    // Unregister Asset Type Actions
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        if (HexGridDataAssetActions.IsValid())
        {
            AssetTools.UnregisterAssetTypeActions(HexGridDataAssetActions.ToSharedRef());
        }
    }

    // unregister editor ui commands
    FHexGridEditorCommands::Unregister();

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHexGridEditorPluginModule, HexGridEditorPlugin)