// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "Tools/UEdMode.h"
#include "HexGridEditorMode.generated.h"


class UHexGridDataAsset;
class FHexGridEditorToolkit;


/*
 *
 */
UCLASS()
class UHexGridEditorMode : public UEdMode
{
    GENERATED_BODY()

public:

    UHexGridEditorMode();

    const static FEditorModeID EM_HexGridEditorModeId;

    // UEdMode interface
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void CreateToolkit() override;
    virtual void OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
    virtual void OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
    virtual void ActivateDefaultTool() override;
    virtual void BindCommands() override;
    virtual bool Select(AActor* InActor, bool bInSelected) override;
    virtual bool IsSelectionAllowed(AActor* InActor, bool bInSelection) const override;
    virtual bool IsSelectionDisallowed(AActor* InActor, bool bInSelection) const override;
    virtual bool IsEditingDisallowed(AActor* InActor) const override;

    // Pointer back to the main editor toolkit
    void SetEditorToolkit(TWeakPtr<FHexGridEditorToolkit> InToolkit) { Toolkit = InToolkit; }

    TSharedPtr<FHexGridEditorToolkit> GetEditorToolkit() { return Toolkit.Pin(); }

    const static FString c_HexPaintToolIdentifier;

    void SetAsset(UHexGridDataAsset* InAsset);
    UHexGridDataAsset* GetAsset() const;

    UEditorInteractiveToolsContext* GetHexGridInteractiveToolsContext() const;

private:

    static const EToolsContextScope c_HexGridToolsContextScope;

    UPROPERTY()
    TObjectPtr<UHexGridDataAsset> HexGridAsset;

    TWeakPtr<FHexGridEditorToolkit> Toolkit;

};