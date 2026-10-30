// Copyright 2026 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


/**
 *	A base class for creating singleton tag registry.
 * 
 *	Helper for linking editor assets to C++ defined FNames.
 */
class MEOHELPERSPLUGIN_API FTagRegistryBase
{
public:

	virtual ~FTagRegistryBase() = default;

	// Adds a tag to the global array
	void RegisterTag(FName const& TagName)
	{
		_RegisteredTags.AddUnique(TagName);
	}

	// Retrieves all registered tags
	const TArray<FName>& GetRegisteredTags() const
	{
		return _RegisteredTags;
	}

protected:

	// registered tags
	TArray<FName> _RegisteredTags;

};


// Generates the GetLocalTags function for ANY given registry singleton
#define GENERATE_LOCAL_TAG_FUNCTIONS(RegistryClass) \
	static TArray<FName>& GetLocalTags() \
	{ \
		static TArray<FName> LocalTags; \
		volatile int32 ForceInstantiation = RegistryClass::Get().GetRegisteredTags().Num(); \
		return LocalTags; \
	}


// Base macro to register a tag with a specific registry
#define REGISTER_TAG(StructName, TagName, RegistryClass) \
	static inline FName TagName = []() \
	{ \
		FName NewTag = FName(#TagName); \
		StructName::GetLocalTags().Add(NewTag); \
		RegistryClass::Get().RegisterTag(NewTag); \
		return NewTag; \
	}();