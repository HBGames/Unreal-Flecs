// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsSubsystemBase.h"
#include "World/FlecsWorld.h"

#include "FlecsEntitySubsystem.generated.h"

#define UE_API FLECSENTITY_API

/**
 * The sole responsibility of this world subsystem class is to host the default instance of FFlecsWorld
 * for a given UWorld. All the gameplay-related use cases of Flecs (found in FlecsGameplay and related plugins) 
 * use this by default.
 */
UCLASS(MinimalAPI)
class UFlecsEntitySubsystem : public UFlecsTickableSubsystemBase
{
	GENERATED_BODY()

	//~ Begin USubsystem interface
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void PostInitialize() override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;
	//~ End of USubsystem interface

public:
	UE_API UFlecsEntitySubsystem();
	UE_API ~UFlecsEntitySubsystem();

	UE_API const FFlecsWorld& GetFlecsWorld() const { return FlecsWorld; }
	UE_API FFlecsWorld& GetMutableFlecsWorld() { return FlecsWorld; }

protected:
	FFlecsWorld FlecsWorld;
};

#undef UE_API
