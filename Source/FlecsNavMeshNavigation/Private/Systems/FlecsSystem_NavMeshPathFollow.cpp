// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "Systems/FlecsSystem_NavMeshPathFollow.h"

#include "Phases/FlecsPhase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystem_NavMeshPathFollow)

UFlecsSystem_NavMeshPathFollow::UFlecsSystem_NavMeshPathFollow()
{
	ExecutionFlags = (int32)ESystemExecutionFlags::AllNetModes;
	ExecuteInPhase = UFlecsPhase_OnUpdate::StaticClass();
}

void UFlecsSystem_NavMeshPathFollow::InitializeInternal(UObject& InOwner, const FFlecsWorld& InFlecsWorld)
{
	Super::InitializeInternal(InOwner, InFlecsWorld);
	SignalSubsystem = UWorld::GetSubsystem<UFlecsSignalSubsystem>(InOwner.GetWorld());
}

void UFlecsSystem_NavMeshPathFollow::BuildSystem(flecs::system_builder<>& SystemBuilder)
{
	Super::BuildSystem(SystemBuilder);
}

void UFlecsSystem_NavMeshPathFollow::Run(flecs::iter& Iterator)
{
	while (Iterator.next())
	{
		// TODO - Implement NavMesh Path Follow System
	}
}
