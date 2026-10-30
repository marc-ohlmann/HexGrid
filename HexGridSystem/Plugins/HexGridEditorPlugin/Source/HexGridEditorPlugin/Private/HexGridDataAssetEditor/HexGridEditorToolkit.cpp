// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/HexGridEditorToolkit.h"

#include "HexGridEditorPlugin.h"
#include "MCommon.h"
#include "HexGridDataAsset.h"
#include "HexGridDataAssetEditor/HexGridEditorMode.h"
#include "HexGridDataAssetEditor/HexGridEditorViewportClient.h"
#include "HexGridDataAssetEditor/SHexGridEditorViewport.h"
#include "HexGridDataAssetEditor/HexGridEditorCommands.h"

#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Editor.h"
#include "SEditorViewport.h"
#include "EditorModeManager.h"
#include "Tools/EdModeInteractiveToolsContext.h"
#include "InteractiveTool.h"


#define LOCTEXT_NAMESPACE "FHexGridEditorToolkit"


const FName FHexGridEditorToolkit::ViewportTabId("HexGridEditor_Viewport");
const FName FHexGridEditorToolkit::DetailsTabId("HexGridEditor_Details");
const FName FHexGridEditorToolkit::ToolsTabId("HexGridEditor_Tools");


FHexGridEditorToolkit::FHexGridEditorToolkit()
{
    PreviewScene = MakeShareable(new FAdvancedPreviewScene(FAdvancedPreviewScene::ConstructionValues()));
}


void FHexGridEditorToolkit::InitEditor(UHexGridDataAsset* GridData)
{
    HexGridAsset = GridData;

    // Define the layout of the editor window
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_HexGridEditor_Layout_v1")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.8f)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.75f)
                    ->AddTab(ViewportTabId, ETabState::OpenedTab)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.25f)
                    ->AddTab(DetailsTabId, ETabState::OpenedTab)
                    ->AddTab(ToolsTabId, ETabState::OpenedTab)
                )
            )
        );

    // call parent Init method
    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = true;
    FAssetEditorToolkit::InitAssetEditor(
        EToolkitMode::Standalone, 
        {}, 
        "HexGridEditor", 
        StandaloneDefaultLayout, 
        bCreateDefaultStandaloneMenu, 
        bCreateDefaultToolbar, 
        HexGridAsset.Get()
    );


    return;
}


void FHexGridEditorToolkit::CreateEditorModeManager()
{
    FAssetEditorToolkit::CreateEditorModeManager();


    return;
}


void FHexGridEditorToolkit::PostInitAssetEditor()
{
    FAssetEditorToolkit::PostInitAssetEditor();

    FEditorModeTools& ModeTools = GetEditorModeManager();

    // activate custom editor mode
    ModeTools.ActivateMode(UHexGridEditorMode::EM_HexGridEditorModeId);

    // Find our active custom editor mode
    UHexGridEditorMode* ActiveMode = Cast<UHexGridEditorMode>(ModeTools.GetActiveScriptableMode(UHexGridEditorMode::EM_HexGridEditorModeId));
    if (!ActiveMode)
    {
        L_ERR(HexGridEditor, "ActiveMode is null");
        return;
    }

    UEditorInteractiveToolsContext* ToolsContext = ActiveMode->GetHexGridInteractiveToolsContext();
    if (!ToolsContext)
    {
        L_ERR(HexGridEditor, "ToolsContext is null");
        return;
    }

    ActiveMode->SetEditorToolkit(SharedThis(this));
    ActiveMode->SetAsset(HexGridAsset.Get());

    // Bind our function to the OnToolActivated delegate
    ToolsContext->ToolManager->OnToolStarted.AddSP(this, &FHexGridEditorToolkit::HandleToolStarted);
    ToolsContext->ToolManager->OnToolEnded.AddSP(this, &FHexGridEditorToolkit::HandleToolEnded);

    UpdateToolsPanel();


    return;
}


void FHexGridEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FHexGridEditorToolkit::SpawnTab_Viewport))
        .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"));

    InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FHexGridEditorToolkit::SpawnTab_Details))
        .SetDisplayName(LOCTEXT("DetailsTab", "Details"));

    InTabManager->RegisterTabSpawner(ToolsTabId, FOnSpawnTab::CreateSP(this, &FHexGridEditorToolkit::SpawnTab_ToolsPanel))
        .SetDisplayName(LOCTEXT("ToolsTab", "Tools"));


    return;
}


TSharedRef<SDockTab> FHexGridEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
    TSharedRef<SHexGridEditorViewport> NewViewport = SNew(SHexGridEditorViewport, SharedThis(this));
    ViewportWidget = NewViewport;

    return SNew(SDockTab)
        .Label(LOCTEXT("ViewportTab_Title", "Viewport"))
        [
            ViewportWidget.ToSharedRef()
        ];
}


TSharedRef<SDockTab> FHexGridEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args)
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsView->SetObject(HexGridAsset.Get()); // Show properties for our main asset

    return SNew(SDockTab)
        .Label(LOCTEXT("DetailsTab_Title", "Details"))
        [
            DetailsView.ToSharedRef()
        ];
}


TSharedRef<SDockTab> FHexGridEditorToolkit::SpawnTab_ToolsPanel(const FSpawnTabArgs& Args)
{
    // Create a standard details view widget
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.bAllowSearch = false;

    ToolDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    return SNew(SDockTab)
        .Label(LOCTEXT("ToolsTab_Title", "Tools"))
        [
            ToolDetailsView.ToSharedRef()
        ];
}


void FHexGridEditorToolkit::HandleToolStarted(UInteractiveToolManager* InToolManager, UInteractiveTool* InTool)
{
    UpdateToolsPanel();


    return;
}


void FHexGridEditorToolkit::HandleToolEnded(UInteractiveToolManager* InToolManager, UInteractiveTool* InTool)
{
    if (ToolDetailsView.IsValid() == false)
    {
        L_ERR(HexGridEditor, "ToolDetailsView is null");
        return;
    }

    ToolDetailsView->SetObject(nullptr);


    return;
}


TSharedPtr<SHexGridEditorViewport> FHexGridEditorToolkit::GetViewportWidget() const
{
    return ViewportWidget;
}


TSharedPtr<FHexGridEditorViewportClient> FHexGridEditorToolkit::GetViewportClient() const
{
    if (!ViewportWidget.IsValid())
    {
        L_ERR(HexGridEditor, "ViewportWidget is null");
        return TSharedPtr<FHexGridEditorViewportClient>();
    }


    return StaticCastSharedPtr<FHexGridEditorViewportClient>(ViewportWidget->GetViewportClient());
}


TSharedPtr<FAdvancedPreviewScene> FHexGridEditorToolkit::GetPreviewScene() const
{
    return PreviewScene;
}


FName FHexGridEditorToolkit::GetToolkitFName() const
{
    // A unique name for this toolkit.
    return FName("HexGridEditor");
}


FText FHexGridEditorToolkit::GetBaseToolkitName() const
{
    // The name that appears on the editor's tab.
    return LOCTEXT("AppLabel", "Hex Grid Editor");
}


FString FHexGridEditorToolkit::GetWorldCentricTabPrefix() const
{
    // A prefix for tabs, can just be the editor's name.
    return TEXT("HexGridEditor");
}


FLinearColor FHexGridEditorToolkit::GetWorldCentricTabColorScale() const
{
    // The color of the editor's tab.
    return FLinearColor::White;
}


void FHexGridEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(HexGridAsset);

    if (PreviewScene)
    {
        PreviewScene->AddReferencedObjects(Collector);
    }


    return;
}


void FHexGridEditorToolkit::MapToolkitCommands()
{
    FAssetEditorToolkit::MapToolkitCommands();

    // Get a reference to the static Commands instance
    const FHexGridEditorCommands& Commands = FHexGridEditorCommands::Get();

    // Now, map the action for your custom tool command
    ToolkitCommands->MapAction(
        Commands.MyToolCommand,
        FExecuteAction::CreateSP(this, &FHexGridEditorToolkit::OnMyToolClicked),
        FCanExecuteAction()
    );


    return;
}


void FHexGridEditorToolkit::UpdateToolsPanel()
{
    FEditorModeTools& ModeTools = GetEditorModeManager();

    // Find our active custom editor mode
    UHexGridEditorMode* ActiveMode = Cast<UHexGridEditorMode>(ModeTools.GetActiveScriptableMode(UHexGridEditorMode::EM_HexGridEditorModeId));
    if (!ActiveMode)
    {
        L_ERR(HexGridEditor, "ActiveMode is null");
        return;
    }

    UEditorInteractiveToolsContext* ToolsContext = ActiveMode->GetHexGridInteractiveToolsContext();
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

    // Get the active tool from the mode's tool manager
    UInteractiveTool* ActiveTool = ToolsContext->ToolManager->GetActiveTool(EToolSide::Left);
    if (!ActiveTool)
    {
        L_ERR(HexGridEditor, "ActiveTool is null");
        return;
    }

    if (ToolDetailsView.IsValid() == false)
    {
        L_ERR(HexGridEditor, "ToolDetailsView is null");
        return;
    }

    // Tell the details view to show the properties of our active tool
    const TArray<UObject*> ToolPropertyObjects = ActiveTool->GetToolProperties(false);
    ToolDetailsView->SetObjects(ToolPropertyObjects);


    return;
}


void FHexGridEditorToolkit::OnMyToolClicked()
{
    UHexGridEditorMode* EditorMode = Cast<UHexGridEditorMode>(GetEditorModeManager().GetActiveScriptableMode(UHexGridEditorMode::EM_HexGridEditorModeId));
    if (EditorMode)
    {
        UInteractiveToolManager* ToolManager = EditorMode->GetToolManager();
        if (ToolManager)
        {
            ToolManager->SelectActiveToolType(EToolSide::Left, UHexGridEditorMode::c_HexPaintToolIdentifier);
            ToolManager->ActivateTool(EToolSide::Left);
        }
    }


    return;
}


#undef LOCTEXT_NAMESPACE
