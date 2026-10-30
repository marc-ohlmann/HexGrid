// Copyright 2025 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "HexGridDataAssetFactory.generated.h"


/*
 *
 */
UCLASS()
class HEXGRIDEDITORPLUGIN_API UHexGridDataAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UHexGridDataAssetFactory();

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};