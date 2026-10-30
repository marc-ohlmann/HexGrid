// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "HexGridPaintToolBuilder.generated.h"


class UInteractiveTool;


/*
 *
 */
UCLASS()
class HEXGRIDEDITORPLUGIN_API UHexGridPaintToolBuilder : public UInteractiveToolBuilder
{
    GENERATED_BODY()

public:
    /**
     * Checks if a tool can be built. For our simple case, it's always true.
     */
    virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;

    /**
     * Creates a new instance of our UHexGridPaintTool.
     */
    virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};