// Copyright 2026 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DisplayDataTypes.generated.h"


class UTexture2D;
struct FSoftObjectPath;


// a generic set of display data
USTRUCT(BlueprintType)
struct MEOHELPERSPLUGIN_API FDisplayData
{
	GENERATED_BODY()

public:

	FDisplayData()
		: Name()
		, Description()
		, Icon(nullptr)
	{
	}

	FDisplayData(
		FText const& InName,
		FText const& InDescription,
		TSoftObjectPtr<UTexture2D> const& InIcon
	)
		: Name(InName)
		, Description(InDescription)
		, Icon(InIcon)
	{
	}

	// clean display identifying text
	UPROPERTY(EditAnywhere)
	FText Name;

	// clean display descriptive text
	UPROPERTY(EditAnywhere)
	FText Description;

	// display icon image
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;

	// collect assets for preloading
	void CollectDisplayPreloadAssets(TArray<FSoftObjectPath>& OutAssets);

};


/**
 * Functions and constants for display data types
 */
UCLASS()
class MEOHELPERSPLUGIN_API UDisplayDataTypes : public UObject
{
	GENERATED_BODY()

public:


};


