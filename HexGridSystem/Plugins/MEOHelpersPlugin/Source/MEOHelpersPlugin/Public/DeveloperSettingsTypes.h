// Copyright 2026 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeveloperSettingsTypes.generated.h"


/**
 * Functions and constants for developer settings types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UDeveloperSettingsTypes : public UObject
{
	GENERATED_BODY()

public:

	// category name for custom developer settings
	static const FName c_CustomPluginDeveloperSettingsCategoryName;

};


