// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsSystem_SignalBase.h"
#include "FlecsSignalSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystem_SignalBase)

UFlecsSystem_SignalBase::UFlecsSystem_SignalBase(const FObjectInitializer& ObjectInitializer)
{
	ExecutionFlags = (int32)EFlecsSystemExecutionFlags::AllNetModes;
}

void UFlecsSystem_SignalBase::Run(flecs::iter& Iterator)
{
	QUICK_SCOPE_CYCLE_COUNTER(SignalEntities);

	const int32 ProcessingFrameBufferIndex = CurrentFrameBufferIndex;
	{
		// we only need to lock the part where we change the current buffer index. Once that's done the incoming signals will end up 
		// in the other buffer
		UE::TRWScopeLock Lock(ReceivedSignalLock, SLT_Write);
		CurrentFrameBufferIndex = (CurrentFrameBufferIndex + 1) % BuffersCount;
	}
}

void UFlecsSystem_SignalBase::BeginDestroy()
{
	if (UFlecsSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UFlecsSignalSubsystem>(GetWorld()))
	{
		for (const FName& SignalName : RegisteredSignals)
		{
			SignalSubsystem->GetSignalDelegateByName(SignalName).RemoveAll(this);
		}
	}

	Super::BeginDestroy();
}

void UFlecsSystem_SignalBase::OnSignalReceived(FName SignalName, TConstArrayView<FFlecsEntityView> Entities)
{
	FEntitySignalRange Range;
	Range.SignalName = SignalName;

	UE::TRWScopeLock Lock(ReceivedSignalLock, SLT_Write);

	FFrameReceivedSignals& CurrentFrameBuffer = FrameReceivedSignals[CurrentFrameBufferIndex];

	Range.Begin = CurrentFrameBuffer.SignaledEntities.Num();
	CurrentFrameBuffer.SignaledEntities.Append(Entities.GetData(), Entities.Num());
	Range.End = CurrentFrameBuffer.SignaledEntities.Num();
	CurrentFrameBuffer.ReceivedSignalRanges.Add(MoveTemp(Range));
}

void UFlecsSystem_SignalBase::SubscribeToSignal(UFlecsSignalSubsystem& SignalSubsystem, const FName SignalName)
{
	check(!RegisteredSignals.Contains(SignalName));
	RegisteredSignals.Add(SignalName);
	SignalSubsystem.GetSignalDelegateByName(SignalName).AddUObject(this, &ThisClass::OnSignalReceived);
}
