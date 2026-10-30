// Copyright 2025 Marc Ohlmann, All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexGridTypes.h"
#include "ExampleHexGridWidget.generated.h"


class UHexGridMinimapWidget;


/**
 * 
 */
UCLASS()
class HEXGRIDSYSTEM_API UExampleHexGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UExampleHexGridWidget(const FObjectInitializer& ObjectInitializer);

	UHexGridMinimapWidget* GetMinimap() const { return Minimap; }

protected:

	UPROPERTY(meta = (BindWidget))
	UHexGridMinimapWidget* Minimap;


};
