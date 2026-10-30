// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"


class FHexGridEditorViewportClient;
class FHexGridEditorToolkit;
class UHexGridDataAsset;


/*
 *
 */
class SHexGridEditorViewport : public SEditorViewport
{
public:
    SLATE_BEGIN_ARGS(SHexGridEditorViewport) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TSharedRef<FHexGridEditorToolkit> InToolkit);

    TSharedPtr<FHexGridEditorViewportClient> GetHexGridViewportClient() const;

    // ensure the widget can receive focus
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

protected:
    /** SEditorViewport interface */
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
    TWeakPtr<FHexGridEditorToolkit> ToolkitPtr;
};