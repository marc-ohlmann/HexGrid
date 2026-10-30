// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FHexGridPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};


// custom log categories
DECLARE_LOG_CATEGORY_EXTERN(HexGrid, Log, All);