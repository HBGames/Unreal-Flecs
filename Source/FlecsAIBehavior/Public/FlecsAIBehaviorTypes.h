// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

#define UE_API FLECSAIBEHAVIOR_API

UE_API DECLARE_LOG_CATEGORY_EXTERN(LogFlecsAIBehavior, Warning, All);

/**
 * Helper macros that could be used inside FFlecsStateTreeEvaluators and FFlecsStateTreeTasks.
 * Requirements is a property or parameters with the following declaration: FStateTreeExecutionContext& Context
 * These macros should be used to standardize the output format and simplify code at call site.
 * They could also easily be changed from UE_(C)VLOG to UE_(C)VLOG_UELOG (or any other implementation) in one go.
 *  e.g. the following:
 *		#if WITH_FLECSGAMEPLAY_DEBUG
 *			const FFlecsStateTreeExecutionContext& FlecsContext = static_cast<FFlecsStateTreeExecutionContext&>(Context);
 *			UE_VLOG(FlecsContext.GetOwner(), LogFlecsAIBehavior, Log, TEXT("Entity [%s]: Starting action: %s"), *FlecsContext.GetEntity().DebugGetDescription(), *StaticEnum<ESomeActionEnum>()->GetValueAsString(SomeActionEnumValue));
 *		#endif // WITH_FLECSGAMEPLAY_DEBUG
 *
 *	could be replaced by:
 *		FLECSBEHAVIOR_CLOG(bDisplayDebug, Log, TEXT("Starting action: %s"), *StaticEnum<ESomeActionEnum>()->GetValueAsString(SomeActionEnumValue));
 */
#if WITH_FLECSGAMEPLAY_DEBUG
#define FLECSBEHAVIOR_LOG(Verbosity, Format, ...) UE_VLOG_UELOG(static_cast<const FFlecsStateTreeExecutionContext&>(Context).GetOwner(), LogFlecsBehavior, Verbosity, \
TEXT("Entity [%s][%s] ") Format, *static_cast<const FFlecsStateTreeExecutionContext&>(Context).GetEntity().DebugGetDescription(), *StaticStruct()->GetName(), ##__VA_ARGS__)
#define FLECSBEHAVIOR_CLOG(Condition, Verbosity, Format, ...) UE_CVLOG_UELOG((Condition), static_cast<const FFlecsStateTreeExecutionContext&>(Context).GetOwner(), LogFlecsBehavior, Verbosity, \
TEXT("Entity [%s][%s] ") Format, *static_cast<const FFlecsStateTreeExecutionContext&>(Context).GetEntity().DebugGetDescription(), *StaticStruct()->GetName(), ##__VA_ARGS__)
#else
#define FLECSBEHAVIOR_LOG(Verbosity, Format, ...)
#define FLECSBEHAVIOR_CLOG(Condition, Verbosity, Format, ...)
#endif

#undef UE_API
