// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#define LOCTEXT_NAMESPACE "FHexGridEditorCommands"


class FHexGridEditorCommands : public TCommands<FHexGridEditorCommands>
{
public:
    FHexGridEditorCommands()
        : TCommands<FHexGridEditorCommands>(
            "HexGridEditor",
            LOCTEXT("HexGridEditor", "Hex Grid Editor"),
            NAME_None,
            FAppStyle::GetAppStyleSetName()
        ) {
    }

    TSharedPtr<FUICommandInfo> MyToolCommand;

    virtual void RegisterCommands() override
    {
        UI_COMMAND(MyToolCommand, "My Tool", "Activates the primary tool.", EUserInterfaceActionType::Button, FInputChord());
    }
};

#undef LOCTEXT_NAMESPACE