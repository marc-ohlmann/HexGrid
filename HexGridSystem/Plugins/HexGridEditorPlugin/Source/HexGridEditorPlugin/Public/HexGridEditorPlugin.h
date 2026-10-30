// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class FHexGridEditorPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TSharedPtr<IAssetTypeActions> HexGridDataAssetActions;

};


// custom log categories
DECLARE_LOG_CATEGORY_EXTERN(HexGridEditor, Log, All);