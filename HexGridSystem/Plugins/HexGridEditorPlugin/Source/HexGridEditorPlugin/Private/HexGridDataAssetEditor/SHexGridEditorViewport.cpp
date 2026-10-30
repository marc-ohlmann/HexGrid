// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#include "HexGridDataAssetEditor/SHexGridEditorViewport.h"

#include "MCommon.h"
#include "HexGridDataAssetEditor/HexGridEditorViewportClient.h"
#include "HexGridDataAssetEditor/HexGridEditorToolkit.h"


void SHexGridEditorViewport::Construct(const FArguments& InArgs, TSharedRef<FHexGridEditorToolkit> InToolkit)
{
    ToolkitPtr = InToolkit;
    SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<FEditorViewportClient> SHexGridEditorViewport::MakeEditorViewportClient()
{
    // Safely get a shared pointer to the toolkit.
    TSharedPtr<FHexGridEditorToolkit> Toolkit = ToolkitPtr.Pin();
    check(Toolkit.IsValid());

    // Get the preview scene and check that it's valid.
    TSharedPtr<FAdvancedPreviewScene> PinnedPreviewScene = Toolkit->GetPreviewScene();
    check(PinnedPreviewScene.IsValid());

    Toolkit->CreateEditorModeManager();

    return MakeShared<FHexGridEditorViewportClient>(
        Toolkit.ToSharedRef(),
        PinnedPreviewScene.ToSharedRef() // Pass the raw pointer to the client
    );
}


FReply SHexGridEditorViewport::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    SEditorViewport::OnFocusReceived(MyGeometry, InFocusEvent);


    return FReply::Handled();
}


TSharedPtr<FHexGridEditorViewportClient> SHexGridEditorViewport::GetHexGridViewportClient() const
{
    return StaticCastSharedPtr<FHexGridEditorViewportClient, FEditorViewportClient>(Client);
}