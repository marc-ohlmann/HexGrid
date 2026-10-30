// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "ExtendedUserWidget.h"

#include "MEOHelpersPlugin.h"
#include "MCommon.h"

#include "Blueprint/WidgetTree.h"


void UExtendedUserWidget::GetChildrenOfClass(UClass* Class, TArray<UWidget*>& OutArray) const
{
	if (UMCommon::IsUObjectPointerSafe(Class) == false)
	{
		L_ERR(MEOHelpers, "class '{0}' is null", Class);

		return;
	}

	if (UMCommon::IsUObjectPointerSafe(WidgetTree) == false)
	{
		L_ERR(MEOHelpers, "widget tree is null for widget '{0}' (search class '{1}')", GetName(), Class);
		return;
	}

	TArray<UWidget*> Children;
	WidgetTree->GetAllWidgets(Children);
	FOR_FORWARD(i, Children.Num())
	{
		if (UMCommon::IsUObjectPointerSafe(Children[i]) == false) continue;

		if (Children[i]->IsA(Class) == true)
		{
			OutArray.Add(Children[i]);
		}
	}


	return;
}


void UExtendedUserWidget::GetChildrenThatImplementInterface(UClass* InterfaceClass, TArray<UWidget*>& OutArray) const
{
	if (UMCommon::IsUObjectPointerSafe(InterfaceClass) == false)
	{
		L_ERR(MEOHelpers, "class is null");

		return;
	}

	if (UMCommon::IsUObjectPointerSafe(WidgetTree) == false)
	{
		L_ERR(MEOHelpers, "widget tree is null for widget '{0}' (search class '{1}')", GetName(), InterfaceClass);
		return;
	}

	TArray<UWidget*> Children;
	WidgetTree->GetAllWidgets(Children);
	FOR_FORWARD(i, Children.Num())
	{
		if (UMCommon::IsUObjectPointerSafe(Children[i]) == false) continue;

		UClass* ChildClass = Children[i]->GetClass();
		if (UMCommon::IsUObjectPointerSafe(ChildClass) == false) continue;

		if (ChildClass->ImplementsInterface(InterfaceClass) == true)
		{
			OutArray.Add(Children[i]);
		}
	}


	return;
}


void UExtendedUserWidget::GetChildrenThatImplementInterfaceRecursive(UClass* InterfaceClass, TArray<UWidget*>& OutArray) const
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
			TryExtendedUserWidget->GetChildrenThatImplementInterfaceRecursive(InterfaceClass, OutArray);
		}
	}

	// get focused children of this user widget
	GetChildrenThatImplementInterface(InterfaceClass, OutArray);


	return;
}


ENetRole UExtendedUserWidget::GetOwningPlayerLocalRole() const
{
	const APlayerController* OwnerPlayerController = GetOwningPlayer();
	if (UMCommon::IsUObjectPointerSafe(OwnerPlayerController) == false)
	{
		return ENetRole::ROLE_None;
	}


	return OwnerPlayerController->GetLocalRole();
}