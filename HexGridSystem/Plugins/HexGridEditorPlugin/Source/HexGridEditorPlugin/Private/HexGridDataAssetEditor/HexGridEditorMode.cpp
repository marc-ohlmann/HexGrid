// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/HexGridEditorMode.h"

#include "HexGridEditorPlugin.h"
#include "MCommon.h"
#include "HexGridDataAsset.h"
#include "HexGridDataAssetEditor/HexGridPaintTool.h"
#include "HexGridDataAssetEditor/HexGridPaintToolBuilder.h"
#include "HexGridDataAssetEditor/HexGridEditorCommands.h"

#include "InteractiveToolManager.h"
#include "Tools/EdModeInteractiveToolsContext.h"


#define LOCTEXT_NAMESPACE "UHexGridEditorMode"


const FEditorModeID UHexGridEditorMode::EM_HexGridEditorModeId(TEXT("EM_HexGridEditor"));
const FString UHexGridEditorMode::c_HexPaintToolIdentifier(TEXT("HexPaintTool"));
const EToolsContextScope UHexGridEditorMode::c_HexGridToolsContextScope(EToolsContextScope::Editor);


UHexGridEditorMode::UHexGridEditorMode()
{
    HexGridAsset = nullptr;

    // Load any icons or set info for the mode here if needed
    Info = FEditorModeInfo(
        EM_HexGridEditorModeId,
        LOCTEXT("HexGridEditorModeName", "Hex Grid Editor"),
        FSlateIcon(), // Add your icon here
        false,
        1
    );
    
}


void UHexGridEditorMode::CreateToolkit()
{
    Super::CreateToolkit();

    // We are creating the toolkit panels as separate tabs in FHexGridEditorToolkit,
    // so we don't need to create a default toolkit here.


    return;
}


void UHexGridEditorMode::OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
    Super::OnToolStarted(Manager, Tool);

    UHexGridDataAsset* DataAsset = GetAsset();
    IF_NULL_RETURN_LERR(DataAsset, HexGridEditor, "DataAsset is null");

    UHexGridPaintTool* PaintTool = Cast<UHexGridPaintTool>(Tool);
    IF_NULL_RETURN_LERR(PaintTool, HexGridEditor, "PaintTool is null");
     
    PaintTool->SetEditAsset(DataAsset);


    return;
}


void UHexGridEditorMode::OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
    Super::OnToolEnded(Manager, Tool);


    return;
}


void UHexGridEditorMode::ActivateDefaultTool()
{
    Super::ActivateDefaultTool();


    return;
}


void UHexGridEditorMode::BindCommands()
{
    Super::BindCommands();


    return;
}


bool UHexGridEditorMode::Select(AActor* InActor, bool bInSelected)
{
    return 0;
}


bool UHexGridEditorMode::IsSelectionAllowed(AActor* InActor, bool bInSelection) const
{
    return true;
}


bool UHexGridEditorMode::IsSelectionDisallowed(AActor* InActor, bool bInSelection) const
{
    return false;
}


bool UHexGridEditorMode::IsEditingDisallowed(AActor* InActor) const
{
    return false;
}


void UHexGridEditorMode::Enter()
{
    Super::Enter();

    const FHexGridEditorCommands& Commands = FHexGridEditorCommands::Get();

    UEditorInteractiveToolsContext* ToolsContext = GetHexGridInteractiveToolsContext();
    if (!ToolsContext)
    {
        L_ERR(HexGridEditor, "ToolsContext is null");
        return;
    }
    if (!ToolsContext->ToolManager)
    {
        L_ERR(HexGridEditor, "ToolManager is null");
        return;
    }

    RegisterTool(
        Commands.MyToolCommand, 
        c_HexPaintToolIdentifier,
        NewObject<UHexGridPaintToolBuilder>(),
        c_HexGridToolsContextScope
    );

    const bool bSelectResult = ToolsContext->ToolManager->SelectActiveToolType(EToolSide::Left, c_HexPaintToolIdentifier);
    if (!bSelectResult)
    {
        L_ERR(HexGridEditor, "failed to select tool");
        return;
    }

    // Set the default tool to be active
    const bool bActivateResult = ToolsContext->ToolManager->ActivateTool(EToolSide::Left);
    if (!bActivateResult)
    {
        L_ERR(HexGridEditor, "failed to activate tool");
        return;
    }

    ToolsContext->StartTool(c_HexPaintToolIdentifier);


    return;
}


void UHexGridEditorMode::Exit()
{
    // Get the tools context and register our tool builder. The string "HexPaintTool" is a unique identifier.
    UEditorInteractiveToolsContext* ToolsContext = GetHexGridInteractiveToolsContext();
    if (!ToolsContext)
    {
        L_ERR(HexGridEditor, "ToolsContext is null");
        return;
    }
    if (!ToolsContext->ToolManager)
    {
        L_ERR(HexGridEditor, "ToolManager is null");
        return;
    }

    // Deactivate any active tools
    ToolsContext->ToolManager->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);


    Super::Exit();
    return;
}


void UHexGridEditorMode::SetAsset(UHexGridDataAsset* InAsset)
{
    HexGridAsset = InAsset;
}


UHexGridDataAsset* UHexGridEditorMode::GetAsset() const
{
    return HexGridAsset;
}


UEditorInteractiveToolsContext* UHexGridEditorMode::GetHexGridInteractiveToolsContext() const
{
    return GetInteractiveToolsContext(c_HexGridToolsContextScope);
}


#undef LOCTEXT_NAMESPACE
