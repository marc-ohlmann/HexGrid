// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "MCommon.h"
#include "ExtendedUserWidget.generated.h"


/**
 * An extended version of UUserWidget that provides helper functions.
 */
UCLASS()
class MEOHELPERSPLUGIN_API UExtendedUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// get the owner player controller's local network role
	ENetRole GetOwningPlayerLocalRole() const;

	// get a list of all child widgets that implement a given interface class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	void GetChildrenThatImplementInterface(UClass* InterfaceClass, TArray<UWidget*>& OutArray) const;

	// templated version to get all child widgets of a particular class
	// provide the UInterface class, NOT the IInterface class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	template <class WidgetClass>
	void GetChildrenThatImplementInterface(TArray<UWidget*>& OutArray) const
	{
		GetChildrenThatImplementInterface(WidgetClass::StaticClass(), OutArray);
	}

	// get a list of all child widgets, recursively, that implement a given interface class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	void GetChildrenThatImplementInterfaceRecursive(UClass* InterfaceClass, TArray<UWidget*>& OutArray) const;

	// templated version to get all child widgets of a particular class
	// provide the UInterface class, NOT the IInterface class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	template <class WidgetClass>
	void GetChildrenThatImplementInterfaceRecursive(TArray<UWidget*>& OutArray) const
	{
		GetChildrenThatImplementInterfaceRecursive(WidgetClass::StaticClass(), OutArray);
	}

	// get a list of all child widgets of a particular class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	void GetChildrenOfClass(UClass* Class, TArray<UWidget*>& OutArray) const;
	
	// templated version to get all child widgets of a particular class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	template <class WidgetClass>
	void GetChildrenOfClass(UClass* Class, TArray<WidgetClass*>& OutArray) const
	{
		TArray<UWidget*> Children;

		GetChildrenOfClass(Class, Children);

		for (int i = 0; i < Children.Num(); i++)
		{
			WidgetClass* WidgetRef = Cast<WidgetClass>(Children[i]);
			if (UMCommon::IsUObjectPointerSafe(WidgetRef) == true)
			{
				OutArray.Add(WidgetRef);
			}
		}


		return;
	}

	// templated version to get all child widgets of a particular class
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	template <class WidgetClass>
	void GetChildrenOfClass(TArray<WidgetClass*>& OutArray) const
	{
		GetChildrenOfClass<WidgetClass>(WidgetClass::StaticClass(), OutArray);
	}

	// templated version to get all child widgets of a particular class, recursively on all contained extended user widgets
	// iterates over ALL child widgets. This should generally be called once and the result cached.
	template <class WidgetClass>
	void GetChildrenOfClassRecursive(TArray<WidgetClass*>& OutArray) const
	{
		if (UMCommon::IsUObjectPointerSafe(WidgetTree) == false)
		{
			//L_ERR(PCaGUI, "widget tree is null");
			return;
		}

		// make recursive call to children that are also user widgets
		TArray<UWidget*> Children;
		WidgetTree->GetAllWidgets(Children);
		for (int i = 0; i < Children.Num(); i++)
		{
			if (UMCommon::IsUObjectPointerSafe(Children[i]) == false) continue;

			UExtendedUserWidget* TryExtendedUserWidget = Cast<UExtendedUserWidget>(Children[i]);
			if (UMCommon::IsUObjectPointerSafe(TryExtendedUserWidget) == true)
			{
				TryExtendedUserWidget->GetChildrenOfClassRecursive<WidgetClass>(OutArray);
			}
		}

		// get focused children of this user widget
		GetChildrenOfClass<WidgetClass>(OutArray);


		return;
	}
	
};
