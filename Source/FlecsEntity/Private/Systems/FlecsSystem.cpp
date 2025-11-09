// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "Systems/FlecsSystem.h"

#include "Phases/FlecsPhase.h"
#include "World/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystem)

UFlecsSystem::UFlecsSystem()
{
	ExecuteInPhase = UFlecsPhase_OnUpdate::StaticClass();
}

UFlecsSystem::UFlecsSystem(const FObjectInitializer& ObjectInitializer)
	: UFlecsSystem()
{
}

bool UFlecsSystem::IsInitialized() const
{
	return bInitialized;
}

void UFlecsSystem::CallInitialize(const TNotNull<UObject*> InOwner, const FFlecsWorld& InFlecsWorld)
{
	if (ensure(HasAnyFlags(RF_ClassDefaultObject) == false && GetClass()->HasAnyClassFlags(CLASS_Abstract) == false))
	{
		check(InFlecsWorld);

#if WITH_FLECSENTITY_DEBUG
		constexpr bool bDebugTraceSystemsEnabled = true;
		if (bDebugTraceSystemsEnabled) // TODO - Move to a config somewhere
		{
			DebugDescription = *GetSystemName();
			FString NetMode = InOwner->GetWorld() ? ToString(InOwner->GetWorld()->GetNetMode()) : TEXT("None");
			//                       DebugDescription       " ("  NetMode        ")"
			DebugDescription.Reserve(DebugDescription.Len() + 2 + NetMode.Len() + 1);
			DebugDescription.Append(TEXT(" ("));
			DebugDescription.Append(*NetMode);
			DebugDescription.Append(TEXT(")"));
		}
		else
		{
			DebugDescription = FString::Printf(TEXT("%s (%s)"), *GetSystemName(), InOwner->GetWorld() ? *ToString(InOwner->GetWorld()->GetNetMode()) : TEXT("No World"));
		}
#endif

		flecs::system_builder<> System = InFlecsWorld.System(GetSystemName());

		System.immediate(bImmediate);
		System.multi_threaded(bMultithreaded);
		System.interval(Interval);
		System.rate(Rate);
		System.priority(Priority);
		System.kind(GetDefault<UFlecsPhase>(ExecuteInPhase)->GetFlecsPhaseId());

		BuildSystem(System);

		InitializeInternal(*InOwner, InFlecsWorld);

		bInitialized = true;
	}
}

ESystemExecutionFlags UFlecsSystem::GetExecutionFlags() const
{
	return static_cast<ESystemExecutionFlags>(ExecutionFlags);
}

bool UFlecsSystem::ShouldExecute(const ESystemExecutionFlags CurrentExecutionFlags) const
{
	return (GetExecutionFlags() & CurrentExecutionFlags) != ESystemExecutionFlags::None;
}

bool UFlecsSystem::ShouldAllowMultipleInstances() const
{
	return bAllowMultipleInstances;
}

void UFlecsSystem::DebugOutputDescription(FOutputDevice& Ar) const
{
	DebugOutputDescription(Ar, 0);
}

void UFlecsSystem::DebugOutputDescription(FOutputDevice& Ar, int32 Indent) const
{
#if WITH_FLECSENTITY_DEBUG
	Ar.Logf(TEXT("%*s%s"), Indent, TEXT(""), *GetSystemName());
#endif
}

FString UFlecsSystem::GetSystemName() const
{
	// By default, use the UObject name as the system name.
	// This prevents duplicate systems from being registered.

	return GetName();
}

TSubclassOf<UFlecsPhase> UFlecsSystem::GetExecuteInPhase() const
{
	return ExecuteInPhase;
}

void UFlecsSystem::SetExecuteInPhase(const TNotNull<TSubclassOf<UFlecsPhase>> InPhaseClass)
{
	checkf(!NotNullGet(InPhaseClass)->HasAnyClassFlags(CLASS_Abstract), TEXT("Cannot set ExecuteInPhase to an abstract class (%s)"), *InPhaseClass->GetName());
	ExecuteInPhase = InPhaseClass;
}

bool UFlecsSystem::IsImmediate() const
{
	return bImmediate;
}

bool UFlecsSystem::IsMultithreaded() const
{
	return bMultithreaded;
}

FFlecsSystemExecutionOrder& UFlecsSystem::GetExecutionOrder()
{
	return ExecutionOrder;
}

int32 UFlecsSystem::GetPriority() const
{
	return Priority;
}

void UFlecsSystem::SetPriority(const int32 NewPriority)
{
	Priority = NewPriority;
}

void UFlecsSystem::MarkAsDynamic()
{
	bIsDynamic = true;
}

bool UFlecsSystem::IsDynamic() const
{
	return bIsDynamic != 0;
}

bool UFlecsSystem::ShouldAutoAddToGlobalList() const
{
	return bAutoRegisterWithSystemPhases;
}

bool UFlecsSystem::ShouldShowUpInSettings() const
{
	return ShouldAutoAddToGlobalList() || bCanShowUpInSettings;
}

void UFlecsSystem::SetShouldAutoRegisterWithGlobalList(const bool bAutoRegister)
{
	if (ensureMsgf(HasAnyFlags(RF_ClassDefaultObject), TEXT("Setting bAutoRegisterWithSystemPhases for non-CDOs has no effect")))
	{
		bAutoRegisterWithSystemPhases = bAutoRegister;
#if WITH_EDITOR
		if (const UClass* Class = GetClass())
		{
			if (const FProperty* AutoRegisterProperty = Class->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ThisClass, bAutoRegisterWithSystemPhases)))
			{
				UpdateSinglePropertyInConfigFile(AutoRegisterProperty, *GetDefaultConfigFilename());
			}
		}
#endif
	}
}

void UFlecsSystem::InitializeInternal(UObject& InOwner, const FFlecsWorld& InFlecsWorld)
{
	// Default implementation does nothing
}

void UFlecsSystem::BuildSystem(flecs::system_builder<>& SystemBuilder)
{
	// TODO
}

void UFlecsSystem::PostInitProperties()
{
	Super::PostInitProperties();

#if CPUPROFILERTRACE_ENABLED
	StatId = GetSystemName();
#endif
}
