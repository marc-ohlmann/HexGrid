// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/HexGridPaintToolBuilder.h"

#include "HexGridEditorPlugin.h"
#include "MCommon.h"
#include "HexGridDataAssetEditor/HexGridPaintTool.h"
#include "HexGridDataAssetEditor/HexGridEditorMode.h"

#include "InteractiveToolManager.h"
#include "EditorModeManager.h"


bool UHexGridPaintToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
    // In more complex tools, you might check if the correct type of object is selected, etc.
    // For our editor mode, we always want our paint tool to be available.
    return true;
}


UInteractiveTool* UHexGridPaintToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
    // Create a new instance of the tool and return it.
    UHexGridPaintTool* NewTool = NewObject<UHexGridPaintTool>(SceneState.ToolManager);


    return NewTool;
}