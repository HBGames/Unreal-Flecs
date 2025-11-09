// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityView.h"
#include "FlecsSubsystemBase.h"
#include "Misc/MTAccessDetector.h"
#include "FlecsExternalSubsystemTraits.h"

#include "FlecsSignalSubsystem.generated.h"

#define UE_API FLECSSIGNALS_API

namespace UE::FlecsSignal
{
	DECLARE_MULTICAST_DELEGATE_TwoParams(FSignalDelegate, FName /*SignalName*/, TConstArrayView<FFlecsEntityView> /*Entities*/);
}

/**
 * A subsystem for handling Signals in Flecs
 */
UCLASS(MinimalAPI)
class UFlecsSignalSubsystem : public UFlecsTickableSubsystemBase
{
	GENERATED_BODY()

public:
	/** 
	 * Retrieve the delegate dispatcher from the signal name
	 * @param SignalName is the name of the signal to get the delegate dispatcher from
	 */
	UE::FlecsSignal::FSignalDelegate& GetSignalDelegateByName(const FName SignalName)
	{
		return NamedSignals.FindOrAdd(SignalName);
	}

	/**
	 * Inform a single entity of a signal being raised
	 * @param SignalName is the name of the signal raised
	 * @param Entity entity that should be informed that signal 'SignalName' was raised
	 */
	UE_API void SignalEntity(FName SignalName, const FFlecsEntityView Entity);

	/**
	 * Inform multiple entities of a signal being raised
	 * @param SignalName is the name of the signal raised
	 * @param Entities list of entities that should be informed that signal 'SignalName' was raised
	 */
	UE_API void SignalEntities(FName SignalName, TConstArrayView<FFlecsEntityView> Entities);

	/**
	 * Inform a single entity of a signal being raised in a certain amount of seconds
	 * @param SignalName is the name of the signal raised
	 * @param Entity entity that should be informed that signal 'SignalName' was raised
	 * @param DelayInSeconds is the amount of time before signaling the entity
	 */
	UE_API void DelaySignalEntity(FName SignalName, const FFlecsEntityView Entity, const float DelayInSeconds);

	/**
	 * Inform multiple entities of a signal being raised in a certain amount of seconds
	 * @param SignalName is the name of the signal raised
	 * @param Entities being informed of the raised signal
	 * @param DelayInSeconds is the amount of time before signaling the entities
	 */
	UE_API void DelaySignalEntities(FName SignalName, TConstArrayView<FFlecsEntityView> Entities, const float DelayInSeconds);

	/**
	 * Inform single entity of a signal being raised asynchronously using the Flecs Command Buffer
	 * @param FlecsWorld is the Flecs World to push the command
	 * @param SignalName is the name of the signal raised
	 * @param Entity entity that should be informed that signal 'SignalName' was raised
	 */
	UE_API void SignalEntityDeferred(FFlecsWorld& FlecsWorld, FName SignalName, const FFlecsEntityView Entity);

	/**
	 * Inform multiple entities of a signal being raised asynchronously using the Flecs Command Buffer
	 * @param FlecsWorld is the Flecs World to push the command
	 * @param SignalName is the name of the signal raised
	 * @param Entities list of entities that should be informed that signal 'SignalName' was raised
	 */
	UE_API void SignalEntitiesDeferred(FFlecsWorld& FlecsWorld, FName SignalName, TConstArrayView<FFlecsEntityView> Entities);

	/**
	 * Inform single entity of a signal being raised asynchronously using the Flecs Command Buffer
	 * @param FlecsWorld is the Flecs World to push the command
	 * @param SignalName is the name of the signal raised
	 * @param Entity entity that should be informed that signal 'SignalName' was raised
	 * @param DelayInSeconds is the amount of time before signaling the entities
	 */
	UE_API void DelaySignalEntityDeferred(FFlecsWorld& FlecsWorld, FName SignalName, const FFlecsEntityView Entity, const float DelayInSeconds);

	/**
	 * Inform multiple entities of a signal being raised asynchronously using the Flecs Command Buffer
	 * @param FlecsWorld is the Flecs World to push the command
	 * @param SignalName is the name of the signal raised
	 * @param Entities being informed of that signal was raised
	 * @param DelayInSeconds is the amount of time before signaling the entities
	 */
	UE_API void DelaySignalEntitiesDeferred(FFlecsWorld& FlecsWorld, FName SignalName, TConstArrayView<FFlecsEntityView> Entities, const float DelayInSeconds);

protected:
	// USubsystem implementation Begin
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	// USubsystem implementation End	

	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;

	/** Multithreading access detector to validate accesses to the list of delayed signals */
	UE_MT_DECLARE_RW_ACCESS_DETECTOR(DelayedSignalsAccessDetector);

	TMap<FName, UE::FlecsSignal::FSignalDelegate> NamedSignals;

	struct FDelayedSignal
	{
		FName SignalName;
		TArray<FFlecsEntityView> Entities;
		double TargetTimestamp;
	};

	TArray<FDelayedSignal> DelayedSignals;

	UPROPERTY(transient)
	TObjectPtr<UWorld> CachedWorld;
};

template <>
struct TFlecsExternalSubsystemTraits<UFlecsSignalSubsystem> final
{
	enum
	{
		GameThreadOnly = false,
		// @todo this subsystem not being thread-safe when writing is an obstacle in
		// parallelizing multiple systems
		ThreadSafeWrite = false,
	};
};


#undef UE_API
