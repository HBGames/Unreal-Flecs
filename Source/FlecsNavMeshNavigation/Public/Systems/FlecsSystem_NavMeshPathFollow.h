// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Systems/FlecsSystem.h"

#include "FlecsSystem_NavMeshPathFollow.generated.h"

#define UE_API FLECSNAVMESHNAVIGATION_API

class UFlecsSignalSubsystem;

/**
 * System for updating move target on a navmesh short path.
 */
UCLASS(MinimalAPI)
class UFlecsSystem_NavMeshPathFollow : public UFlecsSystem
{
	GENERATED_BODY()

protected:
	UE_API UFlecsSystem_NavMeshPathFollow();

	virtual void InitializeInternal(UObject& InOwner, const FFlecsWorld& InFlecsWorld) override;
	UE_API virtual void BuildSystem(flecs::system_builder<>& SystemBuilder) override;
	UE_API virtual void Run(flecs::iter& Iterator) override;

	UPROPERTY(Transient)
	TObjectPtr<UFlecsSignalSubsystem> SignalSubsystem;
};

#undef UE_API
