// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsEntitySubsystem.h"

#include "FlecsEntityTypes.h"
#include "FlecsEntityUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Settings/FlecsEntitySettings.h"
#include "Systems/FlecsSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntitySubsystem)

void UFlecsEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeFlecsWorld();
}

void UFlecsEntitySubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UFlecsEntitySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UFlecsEntitySubsystem::Tick(float DeltaTime)
{
	// TODO - Should Tick
	if (FlecsWorld)
	{
		FlecsWorld.Progress(DeltaTime);
	}
}

TStatId UFlecsEntitySubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsEntitySubsystem, STATGROUP_Tickables);
}

UFlecsEntitySubsystem::UFlecsEntitySubsystem()
{
}

UFlecsEntitySubsystem::~UFlecsEntitySubsystem()
{
}

bool UFlecsEntitySubsystem::HasSystemOfExactClass(const TNotNull<TSubclassOf<UFlecsSystem>> InSystemClass) const
{
	UClass* TestClass = NotNullGet(InSystemClass).Get();
	const auto* FoundSystem = Systems.FindByPredicate([TestClass](const UFlecsSystem* System) { return System && System->GetClass() == TestClass; });
	return !!FoundSystem;
}

void UFlecsEntitySubsystem::AppendUniqueRuntimeSystemCopies(TConstArrayView<const UFlecsSystem*> InSystems, const TNotNull<UObject*> InOwner, const FFlecsWorld& InFlecsWorld)
{
	check(FlecsWorld);

	const TNotNull<const UWorld*> World = InOwner->GetWorld();
	constexpr EFlecsSystemExecutionFlags TEMP_ExecutionFlags = EFlecsSystemExecutionFlags::All;
	const EFlecsSystemExecutionFlags WorldExecutionFlags = UE::Flecs::Utils::DetermineSystemExecutionFlags(World, TEMP_ExecutionFlags);
	const int32 StartingCount = Systems.Num();

	for (const UFlecsSystem* System : InSystems)
	{
		if (System && System->ShouldExecute(WorldExecutionFlags) && !HasSystemOfExactClass(System->GetClass()))
		{
			// Unfortunately the const cast is required since NewObject doesn't support const Template object
			UFlecsSystem* SystemCopy = NewObject<UFlecsSystem>(InOwner, System->GetClass(), FName(), RF_NoFlags, const_cast<UFlecsSystem*>(System));
			Systems.Add(SystemCopy);
		}
#if WITH_FLECSENTITY_DEBUG
		else if (System)
		{
			if (System->ShouldExecute(WorldExecutionFlags) == false)
			{
				UE_VLOG(InOwner, LogFlecs, Log, TEXT("Skipping %s due to ExecutionFlags"), *System->GetName());
			}
			else if (System->ShouldAllowMultipleInstances() == false)
			{
				UE_VLOG(InOwner, LogFlecs, Log, TEXT("Skipping %s due to it being a duplicate"), *System->GetName());
			}
		}
#endif
	}

	for (int32 NewSystemIndex = StartingCount; NewSystemIndex < Systems.Num(); ++NewSystemIndex)
	{
		const TNotNull<UFlecsSystem*> System = Systems[NewSystemIndex];

		if (!System->IsInitialized())
		{
			REDIRECT_OBJECT_TO_VLOG(System, InOwner);
			System->CallInitialize(InOwner, InFlecsWorld);
		}
	}
}

void UFlecsEntitySubsystem::AppendSystem(const TNotNull<UFlecsSystem*> InSystem)
{
	Systems.Add(InSystem);
}

void UFlecsEntitySubsystem::AppendSystems(TArrayView<UFlecsSystem*> InSystems)
{
	Systems.Append(InSystems);
}

void UFlecsEntitySubsystem::AppendSystems(TArray<UFlecsSystem*>&& InSystems)
{
	if (Systems.Num())
	{
		Systems.Append(MoveTemp(InSystems));
	}
	else
	{
		Systems = MoveTemp(InSystems);
	}
}

bool UFlecsEntitySubsystem::AppendUniqueSystem(const TNotNull<UFlecsSystem*> InSystem)
{
	const int32 PreviousCount = Systems.Num();
	Systems.AddUnique(InSystem);
	return PreviousCount != Systems.Num();
}

void UFlecsEntitySubsystem::AppendSystem(const TNotNull<TSubclassOf<UFlecsSystem>> SystemClass, const TNotNull<UObject*> InOwner)
{
	UFlecsSystem* SystemInstance = NewObject<UFlecsSystem>(InOwner, NotNullGet(SystemClass));
	AppendSystem(SystemInstance);
}

bool UFlecsEntitySubsystem::RemoveSystem(const TNotNull<const UFlecsSystem*> InSystem)
{
	const int32 NumRemoved = Systems.RemoveAll([System = InSystem](const TObjectPtr<UFlecsSystem>& Element)
	{
		return Element == System;
	});
	return NumRemoved > 0;
}

void UFlecsEntitySubsystem::SortByExecutionPriority()
{
	if (Systems.IsEmpty()) return;

	Systems.RemoveAllSwap([](const UFlecsSystem* System)
	{
		return System == nullptr;
	});
	Systems.Sort([](const UFlecsSystem& ProcessorA, const UFlecsSystem& ProcessorB)
	{
		return ProcessorA.GetPriority() > ProcessorB.GetPriority();
	});
}

void UFlecsEntitySubsystem::InitializeFlecsWorld()
{
	// 1) convert once, store in a local so it doesn’t vanish immediately
	auto NameAnsi = StringCast<ANSICHAR>(*GetName());
	// 2) pull out the pointer (still valid until we leave this function)
	ANSICHAR* Argv0 = const_cast<ANSICHAR*>(NameAnsi.Get());
	// 3) build argv
	char* argv[] = {Argv0};
	// now StringCast<ANSICHAR> NameAnsi lives through this call,
	// so flecs::world(1, argv) sees valid memory
	FlecsWorld = FFlecsWorld(1, argv, this);

	if (GetWorld()->IsGameWorld())
	{
		FlecsWorld.Set<flecs::Rest>(flecs::Rest{.port = ECS_REST_DEFAULT_PORT});
#ifdef FLECS_STATS
		FlecsWorld.Import<flecs::stats>();
#endif
	}

	RegisterSystems();
}

void UFlecsEntitySubsystem::RegisterSystems()
{
	check(FlecsWorld);

	AppendUniqueRuntimeSystemCopies(GetDefault<UFlecsEntitySettings>()->SystemCDOs, this, FlecsWorld);
}
