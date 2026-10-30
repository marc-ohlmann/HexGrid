// Copyright 2022 Marc Ohlmann, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EngineUtils.h"
#include "Logging/LogVerbosity.h"
#include "Logging/StructuredLog.h"
#include "MathTypes.h"

#include "MCommon.generated.h"


class UObject;
class UClass;
class UWorld;
class AActor;
class UWorldSubsystem;
class UAISystem;
class AGameStateBase;
class AGameModeBase;
class UGameInstance;
class UGameInstanceSubsystem;
class UGameViewportClient;
class APlayerState;
class APlayerController;
class ULocalPlayer;
class ULocalPlayerSubsystem;
class AAIController;
class AHUD;
class APawn;
class UWidget;
class UUserWidget;
class UEngine;
class UEngineSubsystem;
class UPrimitiveComponent;
class UGameUserSettings;


// generic for loop that starts with the first element and works forward.
// use a reverse loop if the data structure will be collapsing (removing elements on each iteration).
#define FOR_FORWARD(iterator, listSize) for(int iterator = 0; iterator < listSize; iterator++)

// generic for loop that starts with the last element and works backward.
// Useful when collapsing a data structure by removing elements on each iteration.
#define FOR_REVERSE(iterator, listSize) for(int iterator = listSize - 1; iterator >= 0; iterator--)

// Helper to iterate efficiently over a map in-place. 
// Read-only iteration (Fastest, use this 99% of the time)
#define FOR_MAP_CONST(PairName, Map) for (const auto& PairName : (Map))

// Helper to iterate efficiently over a map in-place. 
// Mutable iteration (Use only if you need to modify PairName.Value)
#define FOR_MAP(PairName, Map) for (auto& PairName : (Map))

// Helper to iterate efficiently over a map in-place
// Safe-Removal Iteration (Exposes the Iterator so you can call .RemoveCurrent())
#define FOR_MAP_SAFE_REMOVAL(IteratorName, Map) for (auto IteratorName = (Map).CreateIterator(); IteratorName; ++IteratorName)

// generic for loop that iterates over every actor of a given type, in the given world.
// this can be an incredibly expensive operation, and should generally only be done once upon loading a world
#define FOR_EVERY_ACTOR_OF_TYPE_IN_WORLD(iterator, type, world) for (TActorIterator<type> iterator(world); iterator; ++iterator)


// convert a type / variable / function name to a string literal
#define STRINGIFY(s) #s

// convert a type / variable / function name to an fstring
#define TO_FSTRING(s) FString(TEXT(STRINGIFY(s))


// system independent macro for getting the current function name
#ifdef _MSC_VER
#define __CLASS_FUNCTION__ (__FUNCTION__)
#endif
#ifdef __GNUG__ 
#define __CLASS_FUNCTION__ (__FILE__) 
#endif

// system independent macro for getting the current function name (with class name)
#define __CURRENT_CLASS_FUNCTION__ FString(__CLASS_FUNCTION__)

// system independent macro for getting the current line number
#define __CURRENT_LINE__  (FString::FromInt(__LINE__))


#if NO_LOGGING || UE_BUILD_SHIPPING
// disable logging macros
#define L_FATAL(LogType, Text, ...) static_assert(true)
#define L_ERR(LogType, Text, ...) static_assert(true)
#define L_WARN(LogType, Text, ...) static_assert(true)
#define L_DBG(LogType, Text, ...) static_assert(true)
#define L_VERBOSE(LogType, Text, ...) static_assert(true)
#define L_VERY_VERBOSE(LogType, Text, ...) static_assert(true)
#else // NO_LOGGING || UE_BUILD_SHIPPING

// template SFINAE that forwards parameters for logging
template<typename... T>
void custom_logger_fill_args(FStringFormatOrderedArguments& Args, T&&... Params)
{
	// iterate over all of the parameters
	([&]<typename N>(N && Next)
	{

		// types that can be directly constructed as an FString
		if constexpr (requires { FStringFormatArg(Forward<N>(Next)); })
			Args.Add(Forward<N>(Next));

		// types with a .ToString() method
		else if constexpr (requires { Next.ToString(); })
			Args.Add(Next.ToString());

		// pointer types with ->GetName(), likely a UObject
		else if constexpr (requires { Next->GetName(); })
			Args.Add(IsValid(Next) ? Next->GetName() : FString(TEXT("nullptr")));

		// Overloaded LexToString() that can accept the type
		else if constexpr (requires { LexToString(Forward<N>(Next)); })
			Args.Add(LexToString(Forward<N>(Next)));

		// UEnums that can be formatted with StaticEnum<T>()->GetValueAsString()
		else if constexpr (requires { StaticEnum<std::decay_t<N>>()->GetValueAsString(Next); })
			Args.Add(StaticEnum<std::decay_t<N>>()->GetValueAsString(Next));

		// else if constexpr (requires { })
			// Args.Add();

		// ^^^
		// continue adding more formattable types above here ^^^
		// ====================================================
		// unformattable type (this is an error)
		else
			static_assert(requires { Next._error___cannot_format_type_as_string_for_logging_; });


		// iterate over the rest of the parameters
	}(Forward<T>(Params)), ...);
}


/*************************
* As of UE 5.3 2023-09-08
* type-checked logging does not support passing log categories because they arent real variables, they are names glued by macros
* So it will glue the name of the template parameter instead of the value of the template parameter, because this is resolved by
* the preprocessor long before
*
* So instead, we will abandon UE log categories and just make up our own with simple strings
*************************/
#define DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES false // this is false until log categories can actually be passed around by templates


// variadic template that can take any number of arguments but still preserve their type-info
// T&& parameters enable perfect forwarding to process types (especially r-values) as they were passed in
template <typename TLogCategory, typename... TArgs>
void custom_logger(
	const FString ClassFuncName,
	const FString LineNumber,
	const TLogCategory LogCategoryType,
	const ELogVerbosity::Type LogVerbosity,
	const TCHAR* Format,
	TArgs&&... Args
)
{

	// forward the args so r-values are processed as they were passed in (ie, FString(), 5, etc.)
	FStringFormatOrderedArguments OrderedArgs;
	custom_logger_fill_args(OrderedArgs, Forward<TArgs>(Args)...);

	const FString Message = FString::Format(Format, MoveTemp(OrderedArgs));

#if DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

#define __GLOBAL_LOG_CATEGORY__ MyLog
#define __TEXT_FORMAT__ "{0}({1}): {2}", ClassFuncName, LineNumber, Message

#else // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

	const FString Category = LogCategoryType;
#define __GLOBAL_LOG_CATEGORY__ MyLog
#define __TEXT_FORMAT__ "[{0}] {1}({2}): {3}", Category, ClassFuncName, LineNumber, Message
	DEFINE_LOG_CATEGORY_STATIC(MyLog, Display, Display);

#endif // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

	// DO UE LOGGING
#if DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES
	switch (LogVerbosity)
	{

	case ELogVerbosity::Type::NoLogging:
		// do nothing
		break;

	case ELogVerbosity::Type::Fatal:
		UE_LOGFMT(TLogCategory, Fatal, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Error:
		UE_LOGFMT(TLogCategory, Error, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Warning:
		UE_LOGFMT(TLogCategory, Warning, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Display:
		UE_LOGFMT(TLogCategory, Display, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Verbose:
		UE_LOGFMT(TLogCategory, Verbose, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::VeryVerbose:
		UE_LOGFMT(TLogCategory, VeryVerbose, __TEXT_FORMAT__);
		break;

	default:
		UE_LOGFMT(TLogCategory, Error, "default on log verbosity");
		break;
	}
#else // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES
	switch (LogVerbosity)
	{

	case ELogVerbosity::Type::NoLogging:
		// do nothing
		break;

	case ELogVerbosity::Type::Fatal:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Fatal, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Error:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Error, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Warning:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Warning, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Display:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Display, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::Verbose:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Verbose, __TEXT_FORMAT__);
		break;

	case ELogVerbosity::Type::VeryVerbose:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, VeryVerbose, __TEXT_FORMAT__);
		break;

	default:
		UE_LOGFMT(__GLOBAL_LOG_CATEGORY__, Error, "default on log verbosity");
		break;
	}
#endif // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

#undef __TEXT_FORMAT__
#undef __GLOBAL_LOG_CATEGORY__

	return;
}

#if DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

// unguarded implementation macro to log a fatal error
#define _L_FATAL_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::Fatal, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log an error
#define _L_ERR_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::Error, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log an warning
#define _L_WARN_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::Warning, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a standard display message
#define _L_DBG_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::Display, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a verbose message
#define _L_VERBOSE_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::Verbose, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a very verbose message
#define _L_VERY_VERBOSE_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, LogType, ELogVerbosity::Type::VeryVerbose, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

#else // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

// unguarded implementation macro to log a fatal error
#define _L_FATAL_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::Fatal, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log an error
#define _L_ERR_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::Error, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log an warning
#define _L_WARN_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::Warning, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a standard display message
#define _L_DBG_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::Display, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a verbose message
#define _L_VERBOSE_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::Verbose, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

// unguarded implementation macro to log a very verbose message
#define _L_VERY_VERBOSE_IMPLEMENTATION(LogType, Text, ...) custom_logger(__CURRENT_CLASS_FUNCTION__, __CURRENT_LINE__, FString(TEXT(#LogType)), ELogVerbosity::Type::VeryVerbose, TEXT(Text) __VA_OPT__(,) __VA_ARGS__)

#endif // DOES_TYPE_CHECKED_LOGGING_SUPPORT_UE_LOG_CATEGORIES

/***********************
*	Note:
*	__VA_OPT__(,) __VA_ARGS__ is used here as of UE 5.3 (2023-09-07)
* This causes intellisense errors if the BuildConfiguration.xml does not set the following:
*
*	<WindowsPlatform>
*      <bStrictPreprocessorConformance >true</bStrictPreprocessorConformance >
*   </WindowsPlatform>
*
***********************/

// macro to log a fatal error
#define L_FATAL(LogType, Text, ...) _L_FATAL_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

// macro to log an error
#define L_ERR(LogType, Text, ...) _L_ERR_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

// macro to log an warning
#define L_WARN(LogType, Text, ...) _L_WARN_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

// macro to log a debug message
#define L_DBG(LogType, Text, ...) _L_DBG_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

// macro to log a verbose message
#define L_VERBOSE(LogType, Text, ...) _L_VERBOSE_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

// macro to log a very verbose message
#define L_VERY_VERBOSE(LogType, Text, ...) _L_VERY_VERBOSE_IMPLEMENTATION(LogType, Text __VA_OPT__(,) __VA_ARGS__)

#endif // NO_LOGGING || UE_BUILD_SHIPPING


// macro for logging a warning that indicates the current function is not implemented
#define NOT_IMPLEMENTED(LogType) L_WARN(LogType, "function not implemented")

// macro for logging and intentionally crashing out of the program
#define INTENTIONAL_CRASH(LogType) \
{ \
L_ERR(LogType, "intentionally crashing out of the program"); \
abort(); \
}


// macro that checks if the given pointer is null. if true, executes provided code and then returns a value
#define IF_NULL_RETURN(Ptr) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { return; }

// macro that checks if the given pointer is null. if true, executes provided code and then returns a value
#define IF_NULL_RETURN_VAL(Ptr, retValue) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { return retValue; }

// macro that checks if the given pointer is null. if true, executes provided code and then returns a value
#define IF_NULL_DO_THEN_RETURN(Ptr, ThingToDo, retValue) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { ThingToDo; return; }

// macro that checks if the given pointer is null. if true, executes provided code and then returns a value
#define IF_NULL_DO_THEN_RETURN_VAL(Ptr, ThingToDo, retValue) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { ThingToDo; return retValue; }


// validates a pointer and returns nullptr if the pointer is null
#define IF_NULL_RETURN_NULL(Ptr) IF_NULL_RETURN_VAL(Ptr, nullptr);

// validates a pointer and returns a default construction for the given type if the pointer is null
#define IF_NULL_RETURN_TYPE(Ptr, returnType) IF_NULL_RETURN_VAL(Ptr, returnType());


// validates a pointer and returns a given value if the pointer is null, additionally logging a given fatal message 
#define IF_NULL_RETURN_VAL_LFATAL(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_FATAL(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given fatal message 
#define IF_NULL_RETURN_LFATAL(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LFATAL(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given fatal message 
#define IF_NULL_RETURN_NULL_LFATAL(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LFATAL(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given fatal message 
#define IF_NULL_RETURN_TYPE_LFATAL(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LFATAL(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer and returns a given value if the pointer is null, additionally logging a given error message 
#define IF_NULL_RETURN_VAL_LERR(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_ERR(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given error message 
#define IF_NULL_RETURN_LERR(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LERR(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given error message 
#define IF_NULL_RETURN_NULL_LERR(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LERR(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given error message 
#define IF_NULL_RETURN_TYPE_LERR(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LERR(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer and returns a given value if the pointer is null, additionally logging a given warning message 
#define IF_NULL_RETURN_VAL_LWARN(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_WARN(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given warning message 
#define IF_NULL_RETURN_LWARN(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LWARN(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given warning message 
#define IF_NULL_RETURN_NULL_LWARN(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LWARN(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given warning message 
#define IF_NULL_RETURN_TYPE_LWARN(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LWARN(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer and returns a given value if the pointer is null, additionally logging a given debug message 
#define IF_NULL_RETURN_VAL_LDBG(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_DBG(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given debug message 
#define IF_NULL_RETURN_LDBG(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LDBG(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given debug message 
#define IF_NULL_RETURN_NULL_LDBG(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LDBG(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given debug message 
#define IF_NULL_RETURN_TYPE_LDBG(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LDBG(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer and returns a given value if the pointer is null, additionally logging a given verbose message 
#define IF_NULL_RETURN_VAL_LVERBOSE(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_VERBOSE(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given verbose message 
#define IF_NULL_RETURN_LVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERBOSE(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given verbose message 
#define IF_NULL_RETURN_NULL_LVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERBOSE(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given verbose message 
#define IF_NULL_RETURN_TYPE_LVERBOSE(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERBOSE(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer and returns a given value if the pointer is null, additionally logging a given very verbose message 
#define IF_NULL_RETURN_VAL_LVERYVERBOSE(Ptr, retValue, LogType, nullMsg, ...) IF_NULL_DO_THEN_RETURN_VAL(Ptr, L_VERY_VERBOSE(LogType, nullMsg, __VA_ARGS__), retValue);

// validates a pointer and returns (void) if the pointer is null, additionally logging a given very verbose message 
#define IF_NULL_RETURN_LVERYVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERYVERBOSE(Ptr, (void)0, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns nullptr if the pointer is null, additionally logging a given very verbose message 
#define IF_NULL_RETURN_NULL_LVERYVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERYVERBOSE(Ptr, nullptr, LogType, nullMsg, __VA_ARGS__);

// validates a pointer and returns a default construction for the given type if the pointer is null, additionally logging a given very verbose message 
#define IF_NULL_RETURN_TYPE_LVERYVERBOSE(Ptr, returnType, LogType, nullMsg, ...) IF_NULL_RETURN_VAL_LVERYVERBOSE(Ptr, returnType(), LogType, nullMsg, __VA_ARGS__);


// validates a pointer, runs a given command, and then breaks within a loop if the given pointer is null
#define IF_NULL_DO_THEN_BREAK(Ptr, ThingToDo) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { ThingToDo; break; }

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK(Ptr) IF_NULL_DO_THEN_BREAK(Ptr, ;);

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LFATAL(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_FATAL(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LERR(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_ERR(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LWARN(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_WARN(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LDBG(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_DBG(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_VERBOSE(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then breaks within a loop if the given pointer is null
#define IF_NULL_BREAK_LVERYVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_BREAK(Ptr, L_VERY_VERBOSE(LogType, nullMsg, __VA_ARGS__));


// validates a pointer, runs a given command, and then continues within a loop if the given pointer is null
#define IF_NULL_DO_THEN_CONTINUE(Ptr, ThingToDo) if (UMCommon::IsUObjectPointerSafe(Ptr) == false) { ThingToDo; continue; }

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE(Ptr) IF_NULL_DO_THEN_CONTINUE(Ptr, ;);

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LFATAL(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_FATAL(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LERR(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_ERR(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LWARN(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_WARN(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LDBG(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_DBG(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_VERBOSE(LogType, nullMsg, __VA_ARGS__));

// validates a pointer and then continues within a loop if the given pointer is null
#define IF_NULL_CONTINUE_LVERYVERBOSE(Ptr, LogType, nullMsg, ...) IF_NULL_DO_THEN_CONTINUE(Ptr, L_VERY_VERBOSE(LogType, nullMsg, __VA_ARGS__));



// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo();
#define DECLARE_STATIC_VOID(FuncName) \
static void FuncName(const UObject* WorldContextObject); \
void FuncName()


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo();
#define DEFINE_STATIC_VOID(SingletonClass, LogType, FuncName) \
void SingletonClass::FuncName(const UObject* WorldContextObject) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName);
// with one parameter
#define DECLARE_STATIC_VOID_OneParam(FuncName, FirstParamType, FirstParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName); \
void FuncName(FirstParamType FirstParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName);
// with one parameter
#define DEFINE_STATIC_VOID_OneParam(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with two parameters
#define DECLARE_STATIC_VOID_TwoParams(FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName); \
void FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with two parameters
#define DEFINE_STATIC_VOID_TwoParams(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName, SecondParamName); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with three parameters
#define DECLARE_STATIC_VOID_ThreeParams(FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName); \
void FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName);
// with three parameters
#define DEFINE_STATIC_VOID_ThreeParams(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with four parameters
#define DECLARE_STATIC_VOID_FourParams(FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName); \
void FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with four parameters
#define DEFINE_STATIC_VOID_FourParams(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with five parameters
#define DECLARE_STATIC_VOID_FiveParams(FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName); \
void FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with five parameters
#define DEFINE_STATIC_VOID_FiveParams(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName); \
	return; \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with six parameters
#define DECLARE_STATIC_VOID_SixParams(FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
static void FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName); \
void FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	void foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with six parameters
#define DEFINE_STATIC_VOID_SixParams(SingletonClass, LogType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
void SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName, SixthParamName); \
	return; \
}



// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo();
#define DECLARE_STATIC_PTR(PtrType, FuncName) \
static PtrType* FuncName(const UObject* WorldContextObject); \
PtrType* FuncName()


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo();
#define DEFINE_STATIC_PTR(SingletonClass, LogType, FuncName, PtrType) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName);
// with one parameter
#define DECLARE_STATIC_PTR_OneParam(PtrType, FuncName, FirstParamType, FirstParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName); \
PtrType* FuncName(FirstParamType FirstParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName);
// with one parameter
#define DEFINE_STATIC_PTR_OneParam(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName);
// with two parameters
#define DECLARE_STATIC_PTR_TwoParams(PtrType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName); \
PtrType* FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with two parameters
#define DEFINE_STATIC_PTR_TwoParams(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName);
// with three parameters
#define DECLARE_STATIC_PTR_ThreeParams(PtrType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName); \
PtrType* FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with three parameters
#define DEFINE_STATIC_PTR_ThreeParams(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName);
// with four parameters
#define DECLARE_STATIC_PTR_FourParams(PtrType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName); \
PtrType* FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with four parameters
#define DEFINE_STATIC_PTR_FourParams(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with five parameters
#define DECLARE_STATIC_PTR_FiveParams(PtrType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName); \
PtrType* FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with five parameters
#define DEFINE_STATIC_PTR_FiveParams(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with six parameters
#define DECLARE_STATIC_PTR_SixParams(PtrType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
static PtrType* FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName); \
PtrType* FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	PtrType* foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with six parameters
#define DEFINE_STATIC_PTR_SixParams(SingletonClass, LogType, FuncName, PtrType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
PtrType* SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_NULL_LERR(Singleton, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName, SixthParamName); \
}



// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo();
// with five parameters
#define DECLARE_STATIC_TYPE(ReturnType, FuncName) \
static ReturnType FuncName(const UObject* WorldContextObject); \
ReturnType FuncName()


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo();
#define DEFINE_STATIC_TYPE(SingletonClass, LogType, FuncName, ReturnType) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName);
// with one parameter
#define DECLARE_STATIC_TYPE_OneParam(ReturnType, FuncName, FirstParamType, FirstParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName); \
ReturnType FuncName(FirstParamType FirstParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName);
// with one parameter
#define DEFINE_STATIC_TYPE_OneParam(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName);
// with two parameters
#define DECLARE_STATIC_TYPE_TwoParams(ReturnType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName); \
ReturnType FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with two parameters
#define DEFINE_STATIC_TYPE_TwoParams(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName, SecondParamType, SecondParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName);
// with three parameters
#define DECLARE_STATIC_TYPE_ThreeParams(ReturnType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName); \
ReturnType FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with three parameters
#define DEFINE_STATIC_TYPE_ThreeParams(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName);
// with four parameters
#define DECLARE_STATIC_TYPE_FourParams(ReturnType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName); \
ReturnType FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with four parameters
#define DEFINE_STATIC_TYPE_FourParams(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo();
// with five parameters
#define DECLARE_STATIC_TYPE_FiveParams(ReturnType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName); \
ReturnType FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with five parameters
#define DEFINE_STATIC_TYPE_FiveParams(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName); \
}


// macro for declaring a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo();
// with six parameters
#define DECLARE_STATIC_TYPE_SixParams(ReturnType, FuncName, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
static ReturnType FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName); \
ReturnType FuncName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName)


// macro for defining a static version of a given function for a singleton/subsystem-like class with a static Get() function
// for functions of the form:	ReturnType foo(FirstParamType FirstParamName, SecondParamType SecondParamName);
// with six parameters
#define DEFINE_STATIC_TYPE_SixParams(SingletonClass, LogType, FuncName, ReturnType, FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName, FourthParamType, FourthParamName, FifthParamType, FifthParamName, SixthParamType, SixthParamName) \
ReturnType SingletonClass::FuncName(const UObject* WorldContextObject, FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName, FourthParamType FourthParamName, FifthParamType FifthParamName, SixthParamType SixthParamName) \
{ \
	SingletonClass* Singleton = SingletonClass::Get(WorldContextObject); \
	IF_NULL_RETURN_TYPE_LERR(Singleton, ReturnType, LogType, "could not get singleton class '{0}'", SingletonClass::StaticClass()); \
	return Singleton->FuncName(FirstParamName, SecondParamName, ThirdParamName, FourthParamName, FifthParamName, SixthParamName); \
}


// type that represents a function pointer that modifies a given object and returns nothing
using SetupObjectFunctionSignature = void (*)(UObject*);

// templated type that represents a function pointer that modifies a given object and returns nothing
template <class T>
using TSetupObjectFunctionSignature = void (*)(T*);


// type that represents a function pointer that validates a given object's properties
using ValidateObjectFunctionSignature = bool (*)(const UObject*);

// templated type that represents a function pointer that validates a given object's properties
template <class T>
using TValidateObjectFunctionSignature = bool (*)(const T*);


// function signature for a Tick() function belonging to some class object
// ex: void ObjectClass::Tick(float DeltaTime)
template <typename ObjectClass>
using TObjectTickFunctionSignature = void (ObjectClass::*)(float);


// macro used to declare a tick function, a float buffer, and a static const float for the stagger max buffer duration
// the user must provide a definition for the function and call it inside the object's Tick() function using CALL_STAGGERED_TICK_FUNCTION
// by providing the deltatime value parameter
#define DECLARE_STAGGERED_TICK_FUNCTION(TickFuncName, StaggerMaxBufferDuration) \
inline static const float c_##TickFuncName##BufferSize = StaggerMaxBufferDuration; \
float _##TickFuncName##Buffer = 0.f; \
void TickFuncName(float DeltaTime)


// macro used to call a staggered tick function that was declared with DECLARE_STAGGERED_TICK_FUNCTION
#define CALL_STAGGERED_TICK_FUNCTION(TickFuncName, DeltaTime) \
if (UMCommon::AccumulateBuffer(_##TickFuncName##Buffer, (DeltaTime), c_##TickFuncName##BufferSize)) \
{ TickFuncName(DeltaTime); }


/**
 * Commonly used function library
 */
UCLASS()
class MEOHELPERSPLUGIN_API UMCommon : public UObject
{
	GENERATED_BODY()

public:

	// strict check to see if a uobject pointer is safe to de-reference
	static bool IsUObjectPointerSafe(const UObject* UObjectPointer);

	// check to see if a uobject pointer is a valid world context object
	static bool IsValidWorldContextObject(const UObject* WorldContextObject);

	// static templated helper for checking if the given index fits within the given array
	template <class T>
	static bool IsValidIndex(const TArray<T>& Array, const int Index)
	{
		return Index >= 0 && Index < Array.Num();
	}

	// get the name of a given uobject
	static FName GetUObjectName(const UObject* Object);

	// get the name of a given uobject
	static FString GetUObjectNameString(const UObject* Object)
	{
		return GetUObjectName(Object).ToString();
	}

	// get the class of a given uobject
	static UClass* GetUObjectClass(const UObject* Object);

	// get the name of the class of a given uobject
	static FName GetUObjectClassName(const UObject* Object);

	// get the name of the class of a given uobject
	static FString GetUObjectClassNameString(const UObject* Object)
	{
		return GetUObjectClassName(Object).ToString();
	}

	// check if the given uobject's name is equal to the given object name
	static bool DoesUObjectHaveName(
		const UObject* Object,
		FName const& ObjectName,
		ENameCase CompareMethod = ENameCase::IgnoreCase,
		bool bCompareNumber = true
	);

	// create a duplicate of the given uobject under the given outer object
	static UObject* DuplicateUObject(UObject* SourceObject, UObject* Outer = nullptr, FName const& DuplicateObjectName = FName());

	// templated create a duplicate of the given uobject under the given outer object
	template <typename TUObjectClass>
	static TUObjectClass* DuplicateUObject(UObject* SourceObject, UObject* Outer = nullptr, FName const& DuplicateObjectName = FName())
	{
		return Cast<TUObjectClass>(DuplicateUObject(SourceObject, Outer, DuplicateObjectName));
	}


	// create a duplicate of the given actor under the given outer object
	static AActor* DuplicateActor(AActor* SourceActor, AActor* OwnerActor = nullptr, FName const& DuplicateActorName = FName());

	// templated create a duplicate of the given actor under the given outer object
	template <typename TActorClass>
	static TActorClass* DuplicateActor(AActor* SourceActor, AActor* OwnerActor = nullptr, FName const& DuplicateActorName = FName())
	{
		return Cast<TActorClass>(DuplicateActor(SourceActor, OwnerActor, DuplicateActorName));
	}


	// randomizes the given array by swapping all elements around for a given number of iterations
	template <class T>
	static void ShuffleArray(TArray<T>& Array, const int NumberOfShuffles, FRandomStream const& RandomStream)
	{
		FOR_FORWARD(i, NumberOfShuffles)
		{
			UMathTypes::RandomizeArray(Array, RandomStream);
		}


		return;
	}


	// randomizes the given array by swapping all elements around for a given number of iterations
	template <class T>
	static void ShuffleArray(TArray<T>& Array, const int NumberOfShuffles, FName const& Seed = FName())
	{
		const FRandomStream RandomStream = UMathTypes::GenerateSeededRandomStream(Seed);
		ShuffleArray<T>(Array, NumberOfShuffles, RandomStream);


		return;
	}


	// accumulates the provided delta time into the given buffer
	// returns true if the buffer overflowed
	static bool AccumulateBuffer(float& Buffer, float DeltaTime, float BufferSize)
	{
		Buffer += DeltaTime;
		if (Buffer >= BufferSize)
		{
			Buffer -= BufferSize;
			return true;
		}


		return false;
	}


	// check if the given world is starting up after a load
	static bool IsWorldStartingUp(UWorld* World);

	// check if the world containing the given context object is starting up after a load
	static bool IsWorldStartingUp(const UObject* WorldContextObject);

	// check if the given world context object is in the given world
	static bool IsInWorld(const UObject* WorldContextObject, UWorld* World);

	// check if the given world context object is in the given world
	static bool IsInWorldSoftly(const UObject* WorldContextObject, TSoftObjectPtr<UWorld> const& World);

	// check if the given world context object is in the given world
	static bool IsInWorldName(const UObject* WorldContextObject, FName const& WorldName);

	// check if the given world context object is in the transition world
	static bool IsInTransitionWorld(const UObject* WorldContextObject);

	// check if the given world context object is in a temporary / untitled world
	// worlds may be temporary / untitled because they were not saved or named
	// the first world loaded on mobile (Android) platforms is Untitled
	static bool IsInTemporaryUntitledWorld(const UObject* WorldContextObject);

	// check if the given world is an editor world
	static bool IsEditorWorld(const UWorld* World);

	// check if the given world context object is in an editor world
	static bool IsInEditorWorld(const UObject* WorldContextObject);

	// check if the given world is an editor world
	static bool IsGameWorld(const UWorld* World);

	// check if the given world context object is in a game world
	static bool IsInGameWorld(const UObject* WorldContextObject);

	// name used for temporary / unsaved worlds
	static const FName c_TemporaryUntitledWorldName;


	// make an array that contains only the given item
	template <typename T>
	static TArray<T> MakeSingleItemArray(T const& Item)
	{
		TArray<T> Array;
		Array.Add(Item);
		return Array;
	}


	// get number of possible values in the given enum type
	template <class EnumType>
	static int GetEnumSize()
	{
		const UEnum* Enum = StaticEnum<EnumType>();
		if (UMCommon::IsUObjectPointerSafe(Enum) == false)
		{
			return 0;
		}


		return Enum->ContainsExistingMax() ? Enum->NumEnums() - 1 : Enum->NumEnums();
	}

	// get an array containing all possible values for the given enum type
	template <class EnumType>
	static TArray<EnumType> GetAllValuesForEnum()
	{
		const UEnum* Enum = StaticEnum<EnumType>();
		if (UMCommon::IsUObjectPointerSafe(Enum) == false)
		{
			return TArray<EnumType>();
		}

		TArray<EnumType> Out;
		const int NumberOfEnumValues = UMCommon::GetEnumSize<EnumType>();
		FOR_FORWARD(i, NumberOfEnumValues)
		{
			const EnumType EnumValueIterator = (EnumType)Enum->GetValueByIndex(i);
			Out.Add(EnumValueIterator);
		}


		return Out;
	}

	// get the first value for the given enum type
	template <class EnumType>
	static EnumType GetFirstValueForEnum()
	{
		TArray<EnumType> AllValues = GetAllValuesForEnum<EnumType>();
		if (AllValues.Num() <= 0)
		{
			return static_cast<EnumType>(0);
		}


		return AllValues[0];
	}

	// get the middle (median) value for the given enum type
	template <class EnumType>
	static EnumType GetMedianValueForEnum()
	{
		TArray<EnumType> AllValues = GetAllValuesForEnum<EnumType>();
		if (AllValues.Num() <= 0)
		{
			return static_cast<EnumType>(0);
		}


		return AllValues[AllValues.Num() / 2];
	}

	// get the last value for the given enum type
	template <class EnumType>
	static EnumType GetLastValueForEnum()
	{
		TArray<EnumType> AllValues = GetAllValuesForEnum<EnumType>();
		if (AllValues.Num() <= 0)
		{
			return static_cast<EnumType>(0);
		}


		return AllValues[AllValues.Num() - 1];
	}

	// iterate the given enum value to the next possible value with wrapping
	template <typename TEnumType>
	static TEnumType GetNextValueForEnum(const TEnumType EnumVal)
	{
		TArray<TEnumType> AllEnumValues = GetAllValuesForEnum<TEnumType>();
		if (AllEnumValues.Num() <= 0)
		{
			return static_cast<TEnumType>(0);
		}
		return AllEnumValues[
			UMathTypes::OffsetValueClamped<int>(
				static_cast<int>(EnumVal),
				1,
				0,
				AllEnumValues.Num() - 1,
				true
			)
		];
	}

	// iterate the given enum value to the next possible value with wrapping
	template <typename TEnumType>
	static TEnumType GetPreviousValueForEnum(const TEnumType EnumVal)
	{
		TArray<TEnumType> AllEnumValues = GetAllValuesForEnum<TEnumType>();
		if (AllEnumValues.Num() <= 0)
		{
			return static_cast<TEnumType>(0);
		}
		return AllEnumValues[
			UMathTypes::OffsetValueClamped<int>(
				static_cast<int>(EnumVal),
				-1,
				0,
				AllEnumValues.Num() - 1,
				true
			)
		];
	}

	// get a random value of the given enum type
	template <class EnumType>
	static EnumType GetRandomValueForEnum(FRandomStream const& RandomStream)
	{
		const UEnum* Enum = StaticEnum<EnumType>();
		if (UMCommon::IsUObjectPointerSafe(Enum) == false)
		{
			return EnumType();
		}

		const int NumberOfEnumValues = UMCommon::GetEnumSize<EnumType>();
		if (NumberOfEnumValues - 1 <= 0)
		{
			return EnumType();
		}

		const int RandomIndex = UMathTypes::GetRandomInt(0, NumberOfEnumValues, RandomStream);


		return static_cast<EnumType>(Enum->GetValueByIndex(RandomIndex));
	}

	// get a random value of the given enum type
	template <class EnumType>
	static EnumType GetRandomValueForEnum(FName Seed = FName())
	{
		const FRandomStream RandomStream = UMathTypes::GenerateSeededRandomStream(Seed);


		return GetRandomValueForEnum<EnumType>(RandomStream);
	}

	// convert an enum value symbol into a string
	template <class T>
	static FString EnumToString(const int64 EnumValue)
	{
		if (StaticEnum<T>()->IsValidEnumValue(EnumValue))
		{
			return StaticEnum<T>()->GetValueAsString(static_cast<T>(EnumValue));
		}


		return FString::Printf(TEXT("InvalidEnum::%lld"), EnumValue);
	}

	// convert an enum value symbol into a string
	template <class T>
	static FString EnumToString(const T EnumValue)
	{
		return EnumToString<T>(static_cast<int32>(EnumValue));
	}

	// one-line safe dereference of a TWeakObjectPtr
	template <class T>
	static T* SafelyDereferenceWeakObjectPtr(TWeakObjectPtr<T> const& InPtr)
	{
		if (InPtr.IsExplicitlyNull() == true ||
			InPtr.IsStale() == true ||
			InPtr.IsValid() == false)
		{
			return nullptr;
		}

		return InPtr.Get(false);
	}

	// dereference an array of weak pointers into a list of plain pointers
	template <class T>
	static void SafelyDereferenceWeakObjectTArray(TArray<TWeakObjectPtr<T>> const& InArray, TArray<T*>& OutArray)
	{
		for (int i = 0; i < InArray.Num(); i++)
		{
			T* StrongPtr = UMCommon::SafelyDereferenceWeakObjectPtr<T>(InArray[i]);
			if (UMCommon::IsUObjectPointerSafe(StrongPtr))
			{
				OutArray.Add(StrongPtr);
			}
		}
	}

	// demote an array of uobject pointers to an array of weak object pointers
	template <class T>
	static void MakeWeakObjectTArray(TArray<T*> const& InArray, TArray <TWeakObjectPtr<T>>& OutWeakArray)
	{
		for (int i = 0; i < InArray.Num(); i++)
		{
			if (UMCommon::IsUObjectPointerSafe(InArray[i]))
			{
				TWeakObjectPtr<T> WeakPtr = TWeakObjectPtr<T>(InArray[i]);
				OutWeakArray.Add(WeakPtr);
			}
		}
	}

	// get the name of the object pointed to by the soft pointer type
	template <typename TSoftPtr>
	static FString GetSoftPtrNameString(TSoftPtr const& SoftPtr)
	{
		FString SoftObjectName = FString(TEXT("nullptr"));
		if (!SoftPtr.IsNull())
		{
			if (SoftPtr.IsValid())
			{
				SoftObjectName = GetUObjectNameString(SoftPtr.Get());
			}
			else
			{
				SoftObjectName = SoftPtr.GetAssetName();
			}
		}
		return SoftObjectName;
	}

	// one line retreiver for a soft asset's name as an FName
	template <class T>
	static FName GetSoftPtrName(T const& SoftAsset)
	{
		if (SoftAsset.IsNull())
		{
			return FName();
		}

		return FName(SoftAsset.GetAssetName());
	}

	// DEPRECATED - use EsnureSoftPtrLoaded and properly load your assets
	// one-line safe dereference of a TSoftObjectPtr
	// immediately loads the asset (synchronously), or gets it if it is already valid
	template <class T>
	static T* ForceLoadSoftObjectPtr(TSoftObjectPtr<T> const& ObjectPtr)
	{
		L_WARN(LoadSync, "Forcing a LoadSynchronous call on asset '{0}' -- this can flush all pending async load calls which can cause hitches", GetSoftPtrNameString(ObjectPtr));
		T* t = nullptr;
		if (ObjectPtr.IsNull()) return nullptr;
		else if (ObjectPtr.IsValid()) t = ObjectPtr.Get();
		else t = ObjectPtr.LoadSynchronous();
		return t;
	}

	// DEPRECATED - use EsnureSoftPtrLoaded and properly load your assets
	// one-line safe dereference of a TArray of TSoftObjectPtrs
	// immediately loads the assets (synchronously), or gets them if they are already valid
	template <class T>
	static void ForceLoadSoftObjects(TArray<TSoftObjectPtr<T>> const& AssetArray, TArray<T*>& OutArray)
	{
		for (int i = 0; i < AssetArray.Num(); i++)
		{
			T* t = UMCommon::ForceLoadSoftObjectPtr<T>(AssetArray[i]);
			if (UMCommon::IsUObjectPointerSafe(t))
			{
				OutArray.Add(t);
			}
		}


		return;
	}

	// DEPRECATED - use EsnureSoftPtrLoaded and properly load your assets
	// one-line safe dereference of a TSoftClassPtr
	// immediately loads the asset (synchronously), or gets it if it is already valid
	template <class T>
	static UClass* ForceLoadSoftClassPtr(TSoftClassPtr<T> const& ClassPtr)
	{
		L_WARN(LoadSync, "Forcing a LoadSynchronous call on class '{0}' -- this can flush all pending async load calls which can cause hitches", GetSoftPtrNameString(ClassPtr));
		UClass* t = nullptr;
		if (ClassPtr.IsNull()) return nullptr;
		else if (ClassPtr.IsValid()) t = ClassPtr.Get();
		else t = ClassPtr.LoadSynchronous();
		return t;
	}

	// DEPRECATED - use EsnureSoftPtrLoaded and properly load your assets
	// one-line safe dereference of a TArray of TSoftClassPtrs
	// immediately loads the assets (synchronously), or gets them if they are already valid
	template <class T>
	static void ForceLoadSoftClasses(TArray<TSoftClassPtr<T>> const& AssetArray, TArray<UClass*>& OutArray)
	{
		for (int i = 0; i < AssetArray.Num(); i++)
		{
			UClass* t = UMCommon::ForceLoadSoftClassPtr<T>(AssetArray[i]);
			if (UMCommon::IsUObjectPointerSafe(t))
			{
				OutArray.Add(t);
			}
		}


		return;
	}

	// one-line safe dereference of a TSoftObjectPtr
	// gets the asset if it is valid or prints error if not loaded
	template <class T>
	static T* EnsureSoftObjectPtrLoaded(TSoftObjectPtr<T> const& ObjectPtr)
	{
		T* t = nullptr;
		if (ObjectPtr.IsValid())
		{
			t = ObjectPtr.Get();
		}
		else
		{
			L_ERR(LoadSync, "soft object ptr not loaded: '{0}'", GetSoftPtrNameString(ObjectPtr));
		}


		return t;
	}

	// one-line safe dereference of an array of TSoftObjectPtrs
	// gets the asset if it is valid or prints error if not loaded
	template <class T>
	static void EnsureSoftObjectPtrsLoaded(TArray<TSoftObjectPtr<T>> const& AssetArray, TArray<T*>& OutArray)
	{
		for (int i = 0; i < AssetArray.Num(); i++)
		{
			T* t = UMCommon::EnsureSoftObjectPtrLoaded<T>(AssetArray[i]);
			if (UMCommon::IsUObjectPointerSafe(t))
			{
				OutArray.Add(t);
			}
		}


		return;
	}

	// one-line safe dereference of a TSoftClassPtr
	// gets the asset if it is valid or prints error if not loaded
	template <class T>
	static UClass* EnsureSoftClassPtrLoaded(TSoftClassPtr<T> const& ClassPtr)
	{
		UClass* t = nullptr;
		if (ClassPtr.IsValid())
		{
			t = ClassPtr.Get();
		}
		else
		{
			L_ERR(LoadSync, "soft class ptr not loaded: '{0}'", GetSoftPtrNameString(ClassPtr));
		}


		return t;
	}

	// one-line safe dereference of a TArray of TSoftClassPtrs
	// immediately loads the assets (synchronously), or gets them if they are already valid
	template <class T>
	static void EnsureSoftClassPtrsLoaded(TArray<TSoftClassPtr<T>> const& AssetArray, TArray<UClass*>& OutArray)
	{
		for (int i = 0; i < AssetArray.Num(); i++)
		{
			UClass* t = UMCommon::EnsureSoftClassPtrLoaded<T>(AssetArray[i]);
			if (UMCommon::IsUObjectPointerSafe(t))
			{
				OutArray.Add(t);
			}
		}


		return;
	}

	// convert a soft object ptr from one type to another
	template <class ToType, class FromType>
	static TSoftObjectPtr<ToType> CastSoftObjectPtr(TSoftObjectPtr<FromType> const& SoftPtr)
	{
		return TSoftObjectPtr<ToType>(SoftPtr.ToSoftObjectPath());
	}

	// convert a soft class ptr from one type to another
	template <class ToType, class FromType>
	static TSoftClassPtr<ToType> CastSoftClassPtr(TSoftClassPtr<FromType> const& SoftPtr)
	{
		return TSoftClassPtr<ToType>(SoftPtr.ToSoftObjectPath());
	}

	// convert an array soft object ptrs from one type to another
	template <class ToType, class FromType>
	static void CastSoftObjectPtrs(TArray<TSoftObjectPtr<FromType>> const& SoftPtrs, TArray<TSoftObjectPtr<ToType>>& OutArray)
	{
		for (int i = 0; i < SoftPtrs.Num(); i++)
		{
			OutArray.Add(CastSoftObjectPtr<ToType, FromType>(SoftPtrs[i]));
		}
	}

	// convert an array soft class ptrs from one type to another
	template <class ToType, class FromType>
	static void CastSoftClassPtrs(TArray<TSoftClassPtr<FromType>> const& SoftPtrs, TArray<TSoftClassPtr<ToType>>& OutArray)
	{
		for (int i = 0; i < SoftPtrs.Num(); i++)
		{
			OutArray.Add(CastSoftClassPtr<ToType, FromType>(SoftPtrs[i]));
		}
	}

	// convert an array of soft pointers into an array of soft object paths
	// used to load sets of assets without needing their type info
	template <typename T>
	static void MakeSoftObjectPathsArray(TArray<T> const& SoftPtrArray, TArray<FSoftObjectPath>& OutPathsArray)
	{
		for (int i = 0; i < SoftPtrArray.Num(); i++)
		{
			OutPathsArray.Add(SoftPtrArray[i].ToSoftObjectPath());
		}


		return;
	}

	// casts a tarray of uobject-based pointers to another uobject-type
	template <class ToType, class FromType>
	static void CastTArrayOfPointers(TArray<FromType*> const& InArray, TArray<ToType*>& OutArray)
	{
		for (int i = 0; i < InArray.Num(); i++)
		{
			ToType* CastedType = Cast<ToType>(InArray[i]);
			if (UMCommon::IsUObjectPointerSafe(CastedType))
			{
				OutArray.Add(CastedType);
			}
		}
	}

	// casts a tarray of a given type to another type using the ToType's constructor
	template <class ToType, class FromType>
	static void CastTArrayOfTypes(TArray<FromType> const& InArray, TArray<ToType>& OutArray)
	{
		for (int i = 0; i < InArray.Num(); i++)
		{
			OutArray.Add(ToType(InArray[i]));
		}
	}


	// removes a component from an actor at runtime
	// unregisters and removes the instance component from the component array for the actor
	static UActorComponent* RemoveComponentFromActorAtRuntime(
		AActor* Actor,
		UActorComponent* Component
	)
	{
		if (UMCommon::IsUObjectPointerSafe(Actor) == false) return nullptr;
		if (UMCommon::IsUObjectPointerSafe(Component) == false) return nullptr;

		Component->UnregisterComponent();
		Actor->RemoveInstanceComponent(Component);


		return Component;
	}

	// destroys a component after removing a component from an actor at runtime
	// unregisters and removes the instance component from the component array for the actor
	static void DestroyComponentFromActorAtRuntime(
		AActor* Actor,
		UActorComponent* Component
	)
	{
		if (UMCommon::IsUObjectPointerSafe(Actor) == false) return;
		if (UMCommon::IsUObjectPointerSafe(Component) == false) return;

		RemoveComponentFromActorAtRuntime(Actor, Component);
		Component->DestroyComponent();


		return;
	}

	// adds a component to an actor at runtime
	// registers and adds the instance component to the component array for the actor
	static void AddComponentToActorAtRuntime(
		AActor* Actor,
		UActorComponent* Component
	)
	{
		if (UMCommon::IsUObjectPointerSafe(Actor) == false) return;
		if (UMCommon::IsUObjectPointerSafe(Component) == false) return;

		Component->RegisterComponent();
		Actor->AddInstanceComponent(Component);


		return;
	}

	// creates and adds a component to an actor at runtime
	// registers and adds the instance component to the component array for the actor
	static UActorComponent* CreateComponentForActorAtRuntime(
		AActor* Actor,
		TSubclassOf<UActorComponent> ComponentClass,
		FName Name = NAME_None,
		EObjectFlags Flags = RF_NoFlags,
		UObject* Template = nullptr,
		bool bCopyTransientsFromClassDefaults = false,
		FObjectInstancingGraph* InInstanceGraph = nullptr,
		UPackage* ExternalPackage = nullptr
	)
	{
		if (UMCommon::IsUObjectPointerSafe(Actor) == false) return nullptr;
		if (UMCommon::IsUObjectPointerSafe(ComponentClass) == false) return nullptr;

		UActorComponent* NewComponent = NewObject<UActorComponent>(
			Actor,
			ComponentClass,
			Name,
			Flags,
			Template,
			bCopyTransientsFromClassDefaults,
			InInstanceGraph,
			ExternalPackage
		);

		AddComponentToActorAtRuntime(Actor, NewComponent);

		return NewComponent;
	}

	// creates and adds a component to an actor at runtime
	// registers and adds the instance component to the component array for the actor
	template <class T>
	static T* CreateComponentForActorAtRuntime(
		AActor* Actor,
		TSubclassOf<UActorComponent> ComponentClass,
		FName Name = NAME_None,
		EObjectFlags Flags = RF_NoFlags,
		UObject* Template = nullptr,
		bool bCopyTransientsFromClassDefaults = false,
		FObjectInstancingGraph* InInstanceGraph = nullptr,
		UPackage* ExternalPackage = nullptr
	)
	{
		return Cast<T>(CreateComponentForActorAtRuntime(
			Actor,
			ComponentClass,
			Name,
			Flags,
			Template,
			bCopyTransientsFromClassDefaults,
			InInstanceGraph,
			ExternalPackage
		)
		);
	}

	// creates and adds a component to an actor at runtime
	// registers and adds the instance component to the component array for the actor
	template <class T>
	static T* CreateComponentForActorAtRuntime(
		AActor* Actor,
		FName Name = NAME_None,
		EObjectFlags Flags = RF_NoFlags,
		UObject* Template = nullptr,
		bool bCopyTransientsFromClassDefaults = false,
		FObjectInstancingGraph* InInstanceGraph = nullptr,
		UPackage* ExternalPackage = nullptr
	)
	{
		return CreateComponentForActorAtRuntime<T>(
			Actor,
			T::StaticClass(),
			Name,
			Flags,
			Template,
			bCopyTransientsFromClassDefaults,
			InInstanceGraph,
			ExternalPackage
		);
	}


	// get the first collision component on the given actor
	static UPrimitiveComponent* GetActorColliderComponent(AActor* Actor);


	// get the world from a given outer uobject
	static UWorld* GetWorldFromOuter(UObject* Outer);

	// get the world from a context uobject
	static UWorld* GetWorldFromContext(const UObject* WorldContextObject);

	// get the world from a context uobject without logging anything even if it fails
	static UWorld* GetWorldFromContextWithSilentFailure(const UObject* WorldContextObject);

	// get the world from a context uobject
	static UWorldSubsystem* GetWorldSubsystem(
		const UObject* WorldContextObject,
		const TSubclassOf<UWorldSubsystem> WorldSubsystemClass
	);

	// get a world subsystem from a world context uobject
	template <class TWorldSubsystemClass>
	static TWorldSubsystemClass* GetWorldSubsystem(const UObject* WorldContextObject)
	{
		return Cast<TWorldSubsystemClass>(GetWorldSubsystem(WorldContextObject, TWorldSubsystemClass::StaticClass()));
	}


	// get the world frame delta time (seconds used to process last frame)
	static float GetWorldDeltaSeconds(const UObject* WorldContextObject);


	// get the game timer manager from a context uobject
	// for deferred function calls that should not be tied to a particular world (might occur during a transition)
	static FTimerManager* GetGameTimerManager(const UObject* WorldContextObject);


	// get the world timer manager from a context uobject
	static FTimerManager* GetWorldTimerManager(const UObject* WorldContextObject);


	static UAISystemBase* GetAISystem(const UObject* WorldContextObject);


	// get the game instance from a world context uobject
	static UGameInstance* GetGameInstance(const UObject* WorldContextObject);

	// get a game instance subsystem from a world context uobject
	static UGameInstanceSubsystem* GetGameInstanceSubsystem(
		const UObject* WorldContextObject,
		const TSubclassOf<UGameInstanceSubsystem> GameInstanceSubsystemClass
	);

	// get a game instance subsystem from a world context uobject
	template <class TGameInstanceSubsystemClass>
	static TGameInstanceSubsystemClass* GetGameInstanceSubsystem(const UObject* WorldContextObject)
	{
		return Cast<TGameInstanceSubsystemClass>(GetGameInstanceSubsystem(WorldContextObject, TGameInstanceSubsystemClass::StaticClass()));
	}

	// get reference to the engine
	static UEngine* GetEngine();

	// get an engine subsystem from the engine
	static UEngineSubsystem* GetEngineSubsystem(TSubclassOf<UEngineSubsystem> const& EngineSubsystemClass);

	// get an engine subsystem from the engine
	template <class TEngineSubsystem>
	static TEngineSubsystem* GetEngineSubsystem()
	{
		return Cast<TEngineSubsystem>(GetEngineSubsystem(TEngineSubsystem::StaticClass()));
	}

	// get the elapsed time since the game was launched
	//static float GetGameElapsedTime(const UObject* WorldContextObject);

	// get the elapsed time since the current world was brought into play
	// this time is paused when the game is paused
	static double GetWorldElapsedTime(const UObject* WorldContextObject);

	// get the elapsed time since the current world was brought into play
	// this time is not effected by pause
	static double GetWorldElapsedRealTime(const UObject* WorldContextObject);

	// casts the game instance from a world context uobject
	template <class T>
	static T* GetGameInstance(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetGameInstance(WorldContextObject));
	}

	// get the game viewport client from a world context uobject
	static UGameViewportClient* GetGameViewportClient(const UObject* WorldContextObject);

	// get the game mode base from a world context uobject
	static AGameModeBase* GetGameMode(const UObject* WorldContextObject);

	// casts the game mode base from a world context uobject
	template <class T>
	static T* GetGameMode(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetGameMode(WorldContextObject));
	}

	// get a component from the game mode from a world context uobject
	static UActorComponent* GetGameModeComponent(const UObject* WorldContextObject, const TSubclassOf<UActorComponent> ComponentClass);

	// casts the component from the game mode from a world context uobject
	template <class T>
	static T* GetGameModeComponent(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetGameModeComponent(WorldContextObject, T::StaticClass()));
	}

	// get the game state from a world context uobject
	static AGameStateBase* GetGameState(const UObject* WorldContextObject);

	// casts the game state from a world context uobject
	template <class T>
	static T* GetGameState(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetGameState(WorldContextObject));
	}

	// get a component from the game state from a world context uobject
	static UActorComponent* GetGameStateComponent(const UObject* WorldContextObject, const TSubclassOf<UActorComponent> ComponentClass);

	// casts a component from the game state from a world context uobject
	template <class T>
	static T* GetGameStateComponent(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetGameStateComponent(WorldContextObject, T::StaticClass()));
	}


	// get all controllers from a world context uobject
	static TArray<AController*> GetControllers(const UObject* WorldContextObject);

	// casts all controllers from a world context uobject
	template <class T>
	static TArray <T*> GetControllers(const UObject* WorldContextObject)
	{
		TArray<AController*> Types = UMCommon::GetControllers(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get all pawns possessed by a controller from a world context uobject
	static TArray<APawn*> GetControllerPawns(const UObject* WorldContextObject);

	// casts all pawns possessed by a controller from a world context uobject
	template <class T>
	static TArray <T*> GetControllerPawns(const UObject* WorldContextObject)
	{
		TArray<APawn*> Types = UMCommon::GetControllerPawns(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get all ai controllers from a world context uobject
	static TArray<AAIController*> GetAIControllers(const UObject* WorldContextObject);

	// casts all ai controllers from a world context uobject
	template <class T>
	static TArray <T*> GetAIControllers(const UObject* WorldContextObject)
	{
		TArray<AAIController*> Types = UMCommon::GetAIControllers(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all ai player states from a world context uobject
	static TArray<APlayerState*> GetAIPlayerStates(const UObject* WorldContextObject);

	// casts all ai player states from a world context uobject
	template <class T>
	static TArray <T*> GetAIPlayerStates(const UObject* WorldContextObject)
	{
		TArray<APlayerState*> Types = UMCommon::GetAIPlayerStates(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all ai player pawns from a world context uobject
	static TArray<APawn*> GetAIPlayerPawns(const UObject* WorldContextObject);

	// casts all ai player pawns from a world context uobject
	template <class T>
	static TArray <T*> GetAIPlayerPawns(const UObject* WorldContextObject)
	{
		TArray<APawn*> Types = UMCommon::GetAIPlayerPawns(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get all player controllers from a world context uobject
	static TArray<APlayerController*> GetPlayerControllers(const UObject* WorldContextObject);

	// casts all player controllers from a world context uobject
	template <class T>
	static TArray <T*> GetPlayerControllers(const UObject* WorldContextObject)
	{
		TArray<APlayerController*> Types = UMCommon::GetPlayerControllers(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get the player state that has the given unique network id number
	static APlayerState* GetPlayerStateWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex);

	// casts the player state with the given network index
	template <class T>
	static T* GetPlayerStateWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(UMCommon::GetPlayerStateWithNetworkIndex(WorldContextObject, NetworkIndex));
	}

	// get a component for the player state that has the given unique network id number
	static UActorComponent* GetComponentForPlayerStateWithNetworkIndex(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int NetworkIndex
	);

	// get a component for the player state that has the given unique network id number
	template <class T>
	static T* GetComponentForPlayerStateWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(GetComponentForPlayerStateWithNetworkIndex(WorldContextObject, NetworkIndex, T::StaticClass()));
	}

	// get the player controller that has the given unique network id number
	static APlayerController* GetPlayerControllerWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex);

	// casts the player controller with the given network index
	template <class T>
	static T* GetPlayerControllerWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(UMCommon::GetPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex));
	}

	// get a component for the player controller that has the given unique network id number
	static UActorComponent* GetComponentForPlayerControllerWithNetworkIndex(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int NetworkIndex
	);

	// get a component for the player controller that has the given unique network id number
	template <class T>
	static T* GetComponentForPlayerControllerWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(GetComponentForPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex, T::StaticClass()));
	}

	// get the pawn for the player controller that has the given unique network id number
	static APawn* GetPawnForPlayerControllerWithNetworkIndex(
		const UObject* WorldContextObject,
		const int NetworkIndex
	);

	// get the pawn for the player controller that has the given unique network id number
	template <class T>
	static T* GetPawnForPlayerControllerWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(GetPawnForPlayerControllerWithNetworkIndex(WorldContextObject, NetworkIndex));
	}

	// get a component for the player pawn that has the given unique network id number
	static UActorComponent* GetComponentForPlayerPawnWithNetworkIndex(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int NetworkIndex
	);

	// get a component for the player pawn that has the given unique network id number
	template <class T>
	static T* GetComponentForPlayerPawnWithNetworkIndex(const UObject* WorldContextObject, const int NetworkIndex)
	{
		return Cast<T>(GetComponentForPlayerPawnWithNetworkIndex(WorldContextObject, NetworkIndex, T::StaticClass()));
	}

	// get a network index from the given player state
	// Unique net id number. Actual value varies based on current online subsystem, use it only as a guaranteed unique number per player.
	static int GetNetworkIndexFromPlayerState(const APlayerState* PlayerState);

	// get a network index from the given player controller
	// Unique net id number. Actual value varies based on current online subsystem, use it only as a guaranteed unique number per player.
	static int GetNetworkIndexFromPlayerController(const APlayerController* PlayerController);

	// get a controller index from the given player controller
	// a valid index means the player is local to this client
	static int GetControllerIndexFromPlayerController(const APlayerController* PlayerController);

	// get a controller index from the given local player
	static int GetControllerIndexFromLocalPlayer(const ULocalPlayer* LocalPlayer);

	// set a controller index for the given player controller
	// if another player controller has this controller index, they will be swapped
	static void SetControllerIndexForPlayerController(APlayerController* PlayerController, const int ControllerIndex);

	// check if the given controller index is valid
	static bool IsPlayerControllerIndexValid(const int PlayerControllerIndex) { return PlayerControllerIndex >= 0; }

	// check if the given player controller is local to this client (player index zero, or a splitscreen player)
	static bool IsPlayerControllerLocalToThisClient(const APlayerController* PlayerController)
	{
		return IsPlayerControllerIndexValid(GetControllerIndexFromPlayerController(PlayerController));
	}


	// get the first player controller for this game instance
	// best to use this when playing a local multiplayer enabled game with a changing main player index
	static APlayerController* GetFirstLocalPlayerController(const UObject* WorldContextObject);

	// templated version to get a local player controller of a given type
	template <class T>
	static T* GetFirstLocalPlayerController(const UObject* WorldContextObject)
	{
		return Cast<T>(UMCommon::GetFirstLocalPlayerController(WorldContextObject));
	}

	// get the player controller for this game instance with the given controller index
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject, const int ControllerIndex = 0);

	// get the player controller for this game instance with the given local player
	// needs a world context to provide the world to search within
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject, const ULocalPlayer* LocalPlayer);

	// templated version to get a local player controller of a given type
	template <class T>
	static T* GetLocalPlayerController(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(UMCommon::GetLocalPlayerController(WorldContextObject, ControllerIndex));
	}

	// get the local player for this game instance with the given controller
	static ULocalPlayer* GetLocalPlayer(const APlayerController* LocalPlayerController);

	// get the local player for this game instance with the given controller index
	static ULocalPlayer* GetLocalPlayer(const UObject* WorldContextObject, const int ControllerIndex = 0);

	// get a local player subsystem for the given local player
	static ULocalPlayerSubsystem* GetLocalPlayerSubsystem(
		const ULocalPlayer* LocalPlayer,
		const TSubclassOf<ULocalPlayerSubsystem> LocalPlayerSubsystemClass
	);

	// get a local player subsystem for the given local player
	static ULocalPlayerSubsystem* GetLocalPlayerSubsystem(
		const APlayerController* LocalPlayerController,
		const TSubclassOf<ULocalPlayerSubsystem> LocalPlayerSubsystemClass
	)
	{
		return GetLocalPlayerSubsystem(
			GetLocalPlayer(LocalPlayerController),
			LocalPlayerSubsystemClass
		);
	}

	// get a local player subsystem for the given local player
	static ULocalPlayerSubsystem* GetLocalPlayerSubsystem(
		const UObject* WorldContextObject,
		const TSubclassOf<ULocalPlayerSubsystem> LocalPlayerSubsystemClass,
		const int ControllerIndex = 0
	)
	{
		return GetLocalPlayerSubsystem(
			GetLocalPlayer(WorldContextObject, ControllerIndex),
			LocalPlayerSubsystemClass
		);
	}

	// templated version to get a local player subsystem for the given local player
	template <class TLocalPlayerSubsystemClass>
	static TLocalPlayerSubsystemClass* GetLocalPlayerSubsystem(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<TLocalPlayerSubsystemClass>(
			UMCommon::GetLocalPlayerSubsystem(
				WorldContextObject,
				TLocalPlayerSubsystemClass::StaticClass(),
				ControllerIndex
			)
		);
	}

	// templated version to get a local player subsystem for the given local player
	template <class TLocalPlayerSubsystemClass>
	static TLocalPlayerSubsystemClass* GetLocalPlayerSubsystem(const APlayerController* LocalPlayerController)
	{
		return Cast<TLocalPlayerSubsystemClass>(
			UMCommon::GetLocalPlayerSubsystem(
				LocalPlayerController,
				TLocalPlayerSubsystemClass::StaticClass()
			)
		);
	}

	// templated version to get a local player subsystem for the given local player
	template <class TLocalPlayerSubsystemClass>
	static TLocalPlayerSubsystemClass* GetLocalPlayerSubsystem(const ULocalPlayer* LocalPlayer)
	{
		return Cast<TLocalPlayerSubsystemClass>(
			UMCommon::GetLocalPlayerSubsystem(
				LocalPlayer,
				TLocalPlayerSubsystemClass::StaticClass()
			)
		);
	}

	// standard method for getting a component from an actor
	static UActorComponent* GetComponentFromActor(const AActor* ComponentOwner, const TSubclassOf<UActorComponent> ComponentClass);

	// standard method for getting a component from an actor
	template <class T>
	static T* GetComponentFromActor(const AActor* ComponentOwner)
	{
		return Cast<T>(GetComponentFromActor(ComponentOwner, T::StaticClass()));
	}

	// get a component for the player controller that has the given controller index
	static UActorComponent* GetComponentForLocalPlayerController(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int ControllerIndex = 0
	);

	// get a component for the player controller that has the given controller index
	template <class T>
	static T* GetComponentForLocalPlayerController(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(GetComponentForLocalPlayerController(WorldContextObject, T::StaticClass(), ControllerIndex));
	}

	// get all player controllers that are local to this game instance (player index zero and splitscreen players)
	static TArray<APlayerController*> GetLocalPlayerControllers(const UObject* WorldContextObject);

	// templated version to get local player controllers of a given type
	template <class T>
	static TArray <T*> GetLocalPlayerControllers(const UObject* WorldContextObject)
	{
		TArray<APlayerController*> Types = UMCommon::GetLocalPlayerControllers(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get number of local player controllers that exist
	static int GetNumberOfLocalPlayerControllers(const UObject* WorldContextObject)
	{
		return UMCommon::GetLocalPlayerControllers(WorldContextObject).Num();
	}

	// check if local splitscreen multiplayer controllers exist
	static bool HasLocalSplitscreenMultiplayerControllers(const UObject* WorldContextObject)
	{
		return UMCommon::GetNumberOfLocalPlayerControllers(WorldContextObject) > 1;
	}

	// get all player controller indices for players that are local to this game instance (player index zero and splitscreen players)
	static TArray<int> GetLocalPlayerControllerIndices(const UObject* WorldContextObject);

	// get non-local player controllers
	static TArray<APlayerController*> GetClientPlayerControllers(const UObject* WorldContextObject);

	// templated version to get non-local player controllers of a given type
	template <class T>
	static TArray <T*> GetClientPlayerControllers(const UObject* WorldContextObject)
	{
		TArray<APlayerController*> Types = UMCommon::GetClientPlayerControllers(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// check if the local machine is hosting client player controllers
	// non-local player controllers only exist on a server
	static bool IsHostingClientPlayers(const UObject* WorldContextObject)
	{
		return UMCommon::GetClientPlayerControllers(WorldContextObject).Num() > 0;
	}


	// get the player hud for this game instance with the given controller index
	static AHUD* GetLocalPlayerHUD(const UObject* WorldContextObject, const int ControllerIndex = 0);

	// templated version to get a local player hud of a given type
	template <class T>
	static T* GetLocalPlayerHUD(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(UMCommon::GetLocalPlayerHUD(WorldContextObject, ControllerIndex));
	}

	// get a component for the player state that has the given controller index
	static UActorComponent* GetComponentForLocalPlayerHUD(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int ControllerIndex = 0
	);

	// get a component for the player state that has the given controller index
	template <class T>
	static T* GetComponentForLocalPlayerHUD(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(GetComponentForLocalPlayerHUD(WorldContextObject, T::StaticClass(), ControllerIndex));
	}

	// get all player huds that are local to this game instance (player index zero and splitscreen players)
	static TArray<AHUD*> GetLocalPlayerHUDs(const UObject* WorldContextObject);

	// templated version to get local player huds of a given type
	template <class T>
	static TArray <T*> GetLocalPlayerHUDs(const UObject* WorldContextObject)
	{
		TArray<AHUD*> Types = UMCommon::GetLocalPlayerHUDs(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get the player state for this game instance with the given controller index
	static APlayerState* GetLocalPlayerState(const UObject* WorldContextObject, const int ControllerIndex = 0);

	// templated version to get a local player state of a given type
	template <class T>
	static T* GetLocalPlayerState(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(UMCommon::GetLocalPlayerState(WorldContextObject, ControllerIndex));
	}

	// get a component for the player state that has the given controller index
	static UActorComponent* GetComponentForLocalPlayerState(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int ControllerIndex = 0
	);

	// get a component for the player state that has the given controller index
	template <class T>
	static T* GetComponentForLocalPlayerState(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(GetComponentForLocalPlayerState(WorldContextObject, T::StaticClass(), ControllerIndex));
	}

	// get all player states that are local to this game instance (player index zero and splitscreen players)
	static TArray<APlayerState*> GetLocalPlayerStates(const UObject* WorldContextObject);

	// templated version to get local player states of a given type
	template <class T>
	static TArray <T*> GetLocalPlayerStates(const UObject* WorldContextObject)
	{
		TArray<APlayerState*> Types = UMCommon::GetLocalPlayerStates(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all player states from a world context uobject
	static TArray<APlayerState*> GetPlayerStates(const UObject* WorldContextObject);

	// casts all player states from a world context uobject
	template <class T>
	static TArray <T*> GetPlayerStates(const UObject* WorldContextObject)
	{
		TArray<APlayerState*> Types = UMCommon::GetPlayerStates(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all player states associated with each APlayerController from a world context uobject
	static TArray<APlayerState*> GetHumanPlayerStates(const UObject* WorldContextObject);

	// casts all player states associated with each APlayerController from a world context uobject
	template <class T>
	static TArray <T*> GetHumanPlayerStates(const UObject* WorldContextObject)
	{
		TArray<APlayerState*> Types = UMCommon::GetHumanPlayerStates(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all player states from a world context uobject
	static TArray<APlayerState*> GetClientPlayerStates(const UObject* WorldContextObject);

	// casts all player states from a world context uobject
	template <class T>
	static TArray <T*> GetClientPlayerStates(const UObject* WorldContextObject)
	{
		TArray<APlayerState*> Types = UMCommon::GetClientPlayerStates(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get the local player's possessed pawn for this game instance at the given index
	static APawn* GetLocalPlayerPawn(const UObject* WorldContextObject, const int ControllerIndex = 0);

	// templated version to get a local player pawn of a given type
	template <class T>
	static T* GetLocalPlayerPawn(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(UMCommon::GetLocalPlayerPawn(WorldContextObject, ControllerIndex));
	}

	// get a component for the player state that has the given controller index
	static UActorComponent* GetComponentForLocalPlayerPawn(
		const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> ActorComponentClass,
		const int ControllerIndex = 0
	);

	// get a component for the player state that has the given controller index
	template <class T>
	static T* GetComponentForLocalPlayerPawn(const UObject* WorldContextObject, const int ControllerIndex = 0)
	{
		return Cast<T>(GetComponentForLocalPlayerPawn(WorldContextObject, T::StaticClass(), ControllerIndex));
	}

	// get all player pawns that are local to this game instance (player index zero and splitscreen players)
	static TArray<APawn*> GetLocalPlayerPawns(const UObject* WorldContextObject);

	// templated version to get local player pawns of a given type
	template <class T>
	static TArray <T*> GetLocalPlayerPawns(const UObject* WorldContextObject)
	{
		TArray<APawn*> Types = UMCommon::GetLocalPlayerPawns(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all player pawns from a world context uobject
	static TArray<APawn*> GetPlayerPawns(const UObject* WorldContextObject);

	// casts all player pawns from a world context uobject
	template <class T>
	static TArray <T*> GetPlayerPawns(const UObject* WorldContextObject)
	{
		TArray<APawn*> Types = UMCommon::GetPlayerPawns(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}

	// get all player pawns from a world context uobject
	static TArray<APawn*> GetClientPlayerPawns(const UObject* WorldContextObject);

	// casts all player pawns from a world context uobject
	template <class T>
	static TArray <T*> GetClientPlayerPawns(const UObject* WorldContextObject)
	{
		TArray<APawn*> Types = UMCommon::GetClientPlayerPawns(WorldContextObject);
		TArray<T*> CastedTypes;
		UMCommon::CastTArrayOfPointers<T>(Types, CastedTypes);
		return CastedTypes;
	}


	// get a player controller from the given widget
	static APlayerController* GetPlayerControllerFromWidget(const UWidget* Widget);

	// get a controller index from the given widget
	static int GetControllerIndexFromWidget(const UWidget* Widget);

	// find the first widget of a given class owned by the given player controller
	// optionally at the top level only
	// this can be an expensive operation and should generally be done once-only during initialization phases
	static UUserWidget* FindUserWidgetOfClassForPlayerController(
		const APlayerController* Player,
		const TSubclassOf<UUserWidget> UserWidgetClass,
		bool bTopLevelOnly = true
	);

	// find the first widget of a given class owned by the given player controller index
	// optionally at the top level only
	// this can be an expensive operation and should generally be done once-only during initialization phases
	static UUserWidget* FindUserWidgetOfClassForPlayerController(
		const UObject* WorldContextObject,
		const int LocalPlayerId,
		const TSubclassOf<UUserWidget> UserWidgetClass,
		bool bTopLevelOnly = true
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location, 
	// guaranteed to be between the inner and outer radius
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UWorld* World,
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FRandomStream const& RandomStream,
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location, 
	// guaranteed to be between the inner and outer radius
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UWorld* World,
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FName const& Seed = FName(),
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UWorld* World,
		FVector const& SearchLocation,
		const float SearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FRandomStream const& RandomStream,
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UWorld* World,
		FVector const& SearchLocation,
		const float SearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FName const& Seed = FName(),
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location, 
	// guaranteed to be between the inner and outer radius
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UObject* WorldContextObject,
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FRandomStream const& RandomStream,
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location, 
	// guaranteed to be between the inner and outer radius
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UObject* WorldContextObject,
		FVector const& SearchLocation,
		const float InnerSearchRadius,
		const float OuterSearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FName const& Seed = FName(),
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UObject* WorldContextObject,
		FVector const& SearchLocation,
		const float SearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FRandomStream const& RandomStream,
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);

	// returns a randomized location on a nearby blocking surface within a given radius to the given location
	// returns true if it succeeded to find a location
	static bool GetRandomGroundLocationWithinRadiusOfLocation(
		const UObject* WorldContextObject,
		FVector const& SearchLocation,
		const float SearchRadius,
		const float SearchHeight,
		FVector& OutVector,
		FName const& Seed = FName(),
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic,
		FCollisionQueryParams const& CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams const& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam,
		const int MaxSearchAttempts = 10
	);
};