// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 	
#include "MCommon.h"
#include "DataTableTypes.generated.h"

/**
 * Commonly used function library
 */
UCLASS()
class MEOHELPERSPLUGIN_API UDataTableTypes : public UObject
{
	GENERATED_BODY()

public:

	// returns true if the datatable points to rows of the given type
	// will return false if the datatable is empty (contains no rows)
	template <class T>
	static bool IsDataTableOfType(const UDataTable* DataTable)
	{
		if (UMCommon::IsUObjectPointerSafe(DataTable) == false)
			return false;


		return DataTable->RowStruct == T::StaticStruct();
	}

	// fetches data of a given type from a data table row handle
	template <class T>
	static T GetDataTableRowData(FDataTableRowHandle const& RowHandle, FString const& DebugContext)
	{
		if (RowHandle.IsNull() == true) { return T(); }
		if (IsDataTableOfType<T>(RowHandle.DataTable) == false) { return T(); }

		T* t_ref = RowHandle.GetRow<T>(DebugContext);
		if (t_ref == nullptr)
		{
			return T();
		}


		return *t_ref;
	}

	// fetches data from a data table row handle and places it in the given container
	// returns false if the data table row handle does not point to data of the given type
	template <class T>
	static bool FindDataTableRowData(FDataTableRowHandle const& RowHandle, FString const& DebugContext, T& OutData)
	{
		if (RowHandle.IsNull() == true) return false;
		if (IsDataTableOfType<T>(RowHandle.DataTable) == false) return false;

		T* t_ref = RowHandle.GetRow<T>(DebugContext);
		if (t_ref == nullptr)
		{
			return false;
		}

		OutData = *t_ref;


		return true;
	}

	// returns true if the row handle points to a valid datatable row of the given type
	template <class T>
	static bool IsDataTableRowTypeValid(FDataTableRowHandle const& RowHandle, FString const& DebugContext)
	{
		if (RowHandle.IsNull() == true) return false;
		if (IsDataTableOfType<T>(RowHandle.DataTable) == false) return false;

		T* RowDataReference = RowHandle.GetRow<T>(DebugContext);


		return RowDataReference != nullptr;
	}

	// returns true if a DataTableRowHandle is not null and is invalid for a given type
	// optionally logging a warning message
	template <class T>
	static bool IsDataTableRowConfiguredAndInvalid(FDataTableRowHandle const& RowHandle, FString const& DebugContext)
	{
		if (RowHandle.IsNull() == false && UDataTableTypes::IsDataTableRowTypeValid<T>(RowHandle, DebugContext) == false)
		{
			return true;
		}


		return false;
	}

};