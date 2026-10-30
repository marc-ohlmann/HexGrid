// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "AdvancedPreviewScene.h"
#include "Toolkits/AssetEditorToolkit.h"


class UHexGridDataAsset;
class FHexGridEditorViewportClient;
class SHexGridEditorViewport;


/*
 *
 */
class FHexGridEditorToolkit : public FAssetEditorToolkit, public FGCObject
{

public:

    FHexGridEditorToolkit();

    void InitEditor(UHexGridDataAsset* GridData);

    // IToolkit interface
    virtual void PostInitAssetEditor() override;
    virtual void MapToolkitCommands() override;
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual void CreateEditorModeManager() override;

    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override { return TEXT("FHexGridEditorToolkit"); }

    TSharedPtr<FAdvancedPreviewScene> GetPreviewScene() const;
    TSharedPtr<SHexGridEditorViewport> GetViewportWidget() const;
    TSharedPtr<FHexGridEditorViewportClient> GetViewportClient() const;
    UHexGridDataAsset* GetAsset() const { return HexGridAsset.Get(); }



protected:

    TObjectPtr<UHexGridDataAsset> HexGridAsset;
    TSharedPtr<class SHexGridEditorViewport> ViewportWidget;
    TSharedPtr<FAdvancedPreviewScene> PreviewScene;
    TSharedPtr<IDetailsView> ToolDetailsView;

    static const FName ViewportTabId;
    static const FName DetailsTabId;
    static  const FName ToolsTabId;

    TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_ToolsPanel(const FSpawnTabArgs& Args);

private:

    void UpdateToolsPanel();

    void OnMyToolClicked();

    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

    void HandleToolStarted(UInteractiveToolManager* InToolManager, UInteractiveTool* InTool);
    void HandleToolEnded(UInteractiveToolManager* InToolManager, UInteractiveTool* InTool);

};