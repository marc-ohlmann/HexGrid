// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#include "MEOHelpersPlugin.h"

#define LOCTEXT_NAMESPACE "FMEOHelpersPluginModule"

DEFINE_LOG_CATEGORY(MEOHelpers);

void FMEOHelpersPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMEOHelpersPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMEOHelpersPluginModule, MEOHelpersPlugin)