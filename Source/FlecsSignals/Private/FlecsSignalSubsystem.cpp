// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsSignalSubsystem.h"

#include "FlecsSignalTypes.h"
#include "World/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSignalSubsystem)

CSV_DEFINE_CATEGORY(FlecsSignalsCounters, true);

void UFlecsSignalSubsystem::SignalEntity(const FName SignalName, const FFlecsEntityView Entity)
{
	checkf(Entity.IsSet(), TEXT("Expecting a valid entity to signal"));
	SignalEntities(SignalName, MakeArrayView(&Entity, 1));
}

void UFlecsSignalSubsystem::SignalEntities(FName SignalName, TConstArrayView<FFlecsEntityView> Entities)
{
	checkf(Entities.Num() > 0, TEXT("Expecting entities to signal"));
	const UE::FlecsSignal::FSignalDelegate& SignalDelegate = GetSignalDelegateByName(SignalName);
	SignalDelegate.Broadcast(SignalName, Entities);

#if CSV_PROFILER_STATS
	FCsvProfiler::RecordCustomStat(*SignalName.ToString(), CSV_CATEGORY_INDEX(FlecsSignalsCounters), Entities.Num(), ECsvCustomStatOp::Accumulate);
#endif

	UE_CVLOG(Entities.Num() == 1, this, LogFlecsSignals, Log, TEXT("Raising signal [%s] to entity [%s]"), *SignalName.ToString(), *Entities[0].DebugGetDescription());
	UE_CVLOG(Entities.Num() > 1, this, LogFlecsSignals, Log, TEXT("Raising signal [%s] to %d entities"), *SignalName.ToString(), Entities.Num());
}

void UFlecsSignalSubsystem::DelaySignalEntity(FName SignalName, const FFlecsEntityView Entity, const float DelayInSeconds)
{
	checkf(Entity.IsSet(), TEXT("Expecting a valid entity to signal"));
	DelaySignalEntities(SignalName, MakeArrayView(&Entity, 1), DelayInSeconds);
}

void UFlecsSignalSubsystem::DelaySignalEntities(FName SignalName, TConstArrayView<FFlecsEntityView> Entities, const float DelayInSeconds)
{
	// If you hit this ensure
	// - With another thread trying to delay signal then you can use DelaySignalEntityDeferred/DelaySignalEntitiesDeferred
	//   if you have access to a FFlecsExecutionContext.
	// - With the game thread executing UFlecsSignalSubsystem::Tick then you need to reorganize your tasks to prevent senders from executing
	//   at the same time as the subsystem tick.
	UE_MT_SCOPED_WRITE_ACCESS(DelayedSignalsAccessDetector);

	FDelayedSignal& DelayedSignal = DelayedSignals.Emplace_GetRef();
	DelayedSignal.SignalName = SignalName;
	DelayedSignal.Entities = Entities;

	check(CachedWorld);
	DelayedSignal.TargetTimestamp = CachedWorld->GetTimeSeconds() + DelayInSeconds;

	UE_CVLOG(Entities.Num() == 1, this, LogFlecsSignals, Log, TEXT("Delay signal [%s] to entity [%s] in %.2f"), *SignalName.ToString(), *Entities[0].DebugGetDescription(), DelayInSeconds);
	UE_CVLOG(Entities.Num() > 1, this, LogFlecsSignals, Log, TEXT("Delay signal [%s] to %d entities in %.2f"), *SignalName.ToString(), Entities.Num(), DelayInSeconds);
}

void UFlecsSignalSubsystem::SignalEntityDeferred(FFlecsWorld& FlecsWorld, FName SignalName, const FFlecsEntityView Entity)
{
	checkf(Entity.IsSet(), TEXT("Expecting a valid entity to signal"));
	SignalEntitiesDeferred(FlecsWorld, SignalName, MakeArrayView(&Entity, 1));
}

void UFlecsSignalSubsystem::SignalEntitiesDeferred(FFlecsWorld& FlecsWorld, FName SignalName, TConstArrayView<FFlecsEntityView> Entities)
{
	checkf(Entities.Num() > 0, TEXT("Expecting entities to signal"));
	FlecsWorld.Defer([SignalName, InEntities = TArray<FFlecsEntityView>(Entities), &FlecsWorld]()
	{
		UFlecsSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UFlecsSignalSubsystem>(FlecsWorld.GetWorld());
		SignalSubsystem->SignalEntities(SignalName, InEntities);
	});

	UE_CVLOG(Entities.Num() == 1, this, LogFlecsSignals, Log, TEXT("Raising deferred signal [%s] to entity [%s]"), *SignalName.ToString(), *Entities[0].DebugGetDescription());
	UE_CVLOG(Entities.Num() > 1, this, LogFlecsSignals, Log, TEXT("Raising deferred signal [%s] to %d entities"), *SignalName.ToString(), Entities.Num());
}

void UFlecsSignalSubsystem::DelaySignalEntityDeferred(FFlecsWorld& FlecsWorld, FName SignalName, const FFlecsEntityView Entity, const float DelayInSeconds)
{
	checkf(Entity.IsSet(), TEXT("Expecting a valid entity to signal"));
	DelaySignalEntitiesDeferred(FlecsWorld, SignalName, MakeArrayView(&Entity, 1), DelayInSeconds);
}

void UFlecsSignalSubsystem::DelaySignalEntitiesDeferred(FFlecsWorld& FlecsWorld, FName SignalName, TConstArrayView<FFlecsEntityView> Entities, const float DelayInSeconds)
{
	checkf(Entities.Num() > 0, TEXT("Expecting entities to signal"));

	FlecsWorld.Defer([SignalName, InEntities = TArray<FFlecsEntityView>(Entities), &FlecsWorld, &DelayInSeconds]()
	{
		UFlecsSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UFlecsSignalSubsystem>(FlecsWorld.GetWorld());
		SignalSubsystem->DelaySignalEntities(SignalName, InEntities, DelayInSeconds);
	});

	UE_CVLOG(Entities.Num() == 1, this, LogFlecsSignals, Log, TEXT("Delay deferred signal [%s] to entity [%s] in %.2f"), *SignalName.ToString(), *Entities[0].DebugGetDescription(), DelayInSeconds);
	UE_CVLOG(Entities.Num() > 1, this, LogFlecsSignals, Log, TEXT("Delay deferred signal [%s] to %d entities in %.2f"), *SignalName.ToString(), Entities.Num(), DelayInSeconds);
}

void UFlecsSignalSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CachedWorld = &GetWorldRef();
	checkf(CachedWorld, TEXT("UFlecsSignalSubsystem instances are expected to always be tied to a valid UWorld instance"));

	OverrideSubsystemTraits<UFlecsSignalSubsystem>(Collection);
}

void UFlecsSignalSubsystem::Deinitialize()
{
	CachedWorld = nullptr;
	Super::Deinitialize();
}

void UFlecsSignalSubsystem::Tick(float DeltaTime)
{
	// If we're in an AutoRTFM Transaction, exit the TickableObject Tick (which is top-level) and run again outside the transaction
	if (AutoRTFM::IsTransactional())
	{
		AutoRTFM::OnCommit([WeakThis = TWeakObjectPtr(this), DeltaTime]()
		{
			if (ensure(WeakThis.IsValid()))
			{
				WeakThis->Tick(DeltaTime);
			}
		});

		return;
	}

	CA_ASSUME(CachedWorld);
	const double CurrentTime = CachedWorld->GetTimeSeconds();

	UE_MT_SCOPED_WRITE_ACCESS(DelayedSignalsAccessDetector);

	for (int i = 0; i < DelayedSignals.Num();)
	{
		FDelayedSignal& DelayedSignal = DelayedSignals[i];
		if (DelayedSignal.TargetTimestamp <= CurrentTime)
		{
			SignalEntities(DelayedSignal.SignalName, MakeArrayView(DelayedSignal.Entities));
			DelayedSignals.RemoveAtSwap(i, EAllowShrinking::No);
		}
		else
		{
			i++;
		}
	}
}

TStatId UFlecsSignalSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsSignalSubsystem, STATGROUP_Tickables);
}
