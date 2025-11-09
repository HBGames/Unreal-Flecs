// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsSubsystemBase.h"

#include "FlecsSimulationSubsystem.generated.h"

#define UE_API FLECSSIMULATION_API

/**
 * 
 */
UCLASS(Config="Game", DefaultConfig, MinimalAPI)
class UFlecsSimulationSubsystem : public UFlecsSubsystemBase
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationStarted, UWorld* /*World*/);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationPauseEvent, TNotNull<UFlecsSimulationSubsystem*> /*this*/);

	UE_API UFlecsSimulationSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

#undef UE_API
