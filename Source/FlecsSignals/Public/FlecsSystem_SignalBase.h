// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Systems/FlecsSystem.h"
#include "FlecsSignalTypes.h"
#include "FlecsEntityView.h"
#include "Containers/StaticArray.h"
#include "Misc/TransactionallySafeRWLock.h"
#include "FlecsSystem_SignalBase.generated.h"

#define UE_API FLECSSIGNALS_API

class UFlecsSignalSubsystem;

/**
 * System for executing signals on each targeted entities
 * The derived classes only need to implement the method SignalEntities to actually received the raised signals for the entities they subscribed to 
 */
UCLASS(MinimalAPI, Abstract)
class UFlecsSystem_SignalBase : public UFlecsSystem
{
	GENERATED_BODY()

public:
	UE_API UFlecsSystem_SignalBase(const FObjectInitializer& ObjectInitializer);

protected:
	UE_API virtual void BuildSystem(flecs::system_builder<>& SystemBuilder) override;

	UE_API virtual void Run(flecs::iter& Iterator) override;

	UE_API virtual void BeginDestroy() override;

	/**
	 * Actual method that derived class needs to implement to act on a signal that is raised for that frame
	 * @param FlecsWorld is the flecs world to retrieve entities from
	 * @param EntitySignals Look up to retrieve for each entities their raised signal via GetSignalsForEntity
	 */
	virtual void SignalEntities(FFlecsWorld& FlecsWorld, FFlecsSignalNameLookup& EntitySignals) PURE_VIRTUAL(UFlecsSystem_SignalBase::SignalEntities,);

	/**
	 * Callback that is being called when new signal is raised
	 * @param SignalName is the name of the signal being raised
	 * @param Entities are the targeted entities for this signal
	 */
	UE_API virtual void OnSignalReceived(FName SignalName, TConstArrayView<FFlecsEntityView> Entities);

	/**
	 * To receive notification about a particular signal, you need to subscribe to it.
	 * @param SignalName is the name of the signal to receive notification about
	 */
	UE_API void SubscribeToSignal(UFlecsSignalSubsystem& SignalSubsystem, const FName SignalName);

private:
	/** Stores a range of indices in the SignaledEntities TArray of Entities and the associated signal name */
	struct FEntitySignalRange
	{
		FName SignalName;
		int32 Begin = 0;
		int32 End = 0;
		bool bProcessed = false;
	};

	struct FFrameReceivedSignals
	{
		/** Received signals are double buffered as we can receive new one while processing them */
		TArray<FEntitySignalRange> ReceivedSignalRanges;

		/** the list of all signaled entities, can contain duplicates */
		TArray<FFlecsEntityView> SignaledEntities;
	};

	static constexpr int BuffersCount = 2;

	/** Double buffer frame received signal as we can receive new signals as we are processing them */
	TStaticArray<FFrameReceivedSignals, BuffersCount> FrameReceivedSignals;

	/** Current frame buffer index of FrameReceivedSignals */
	int32 CurrentFrameBufferIndex = 0;
	/** List of all the registered signal names. */
	TArray<FName> RegisteredSignals;

	FTransactionallySafeRWLock ReceivedSignalLock;
};

#undef UE_API
