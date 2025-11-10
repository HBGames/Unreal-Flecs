// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsEntityUtils.h"

#include "FlecsEntitySubsystem.h"

namespace UE::Flecs::Utils
{
	EFlecsSystemExecutionFlags GetSystemExecutionFlagsForWorld(const UWorld& World)
	{
#if WITH_EDITOR
		if (World.IsEditorWorld() && !World.IsGameWorld())
		{
			return EFlecsSystemExecutionFlags::EditorWorld;
		}
#endif

		switch (const ENetMode NetMode = World.GetNetMode())
		{
		case NM_ListenServer:
			return EFlecsSystemExecutionFlags::Client | EFlecsSystemExecutionFlags::Server;
		case NM_DedicatedServer:
			return EFlecsSystemExecutionFlags::Server;
		case NM_Client:
			return EFlecsSystemExecutionFlags::Client;
		case NM_Standalone:
			return EFlecsSystemExecutionFlags::Standalone;
		default:
			checkf(false, TEXT("Unsupported ENetMode type (%i) found while determining Flecs system execution flags."), NetMode);
			return EFlecsSystemExecutionFlags::None;
		}
	}

	EFlecsSystemExecutionFlags DetermineSystemExecutionFlags(const UWorld* World, EFlecsSystemExecutionFlags ExecutionFlagsOverride)
	{
		if (ExecutionFlagsOverride != EFlecsSystemExecutionFlags::None)
		{
			return ExecutionFlagsOverride;
		}
		if (World)
		{
			return GetSystemExecutionFlagsForWorld(*World);
		}

#if WITH_EDITOR
		if (GEditor)
		{
			return EFlecsSystemExecutionFlags::Editor;
		}
#endif
		return EFlecsSystemExecutionFlags::All;
	}

	uint8 DetermineSystemSupportedTickTypes(const UWorld* World)
	{
#if WITH_EDITOR
		if (World != nullptr && GetSystemExecutionFlagsForWorld(*World) == EFlecsSystemExecutionFlags::EditorWorld)
		{
			return MAX_uint8;
		}
#endif // WITH_EDITOR
		return (1 << LEVELTICK_All) | (1 << LEVELTICK_TimeOnly);
	}


	FFlecsWorld* GetFlecsWorld(const UObject* WorldContextObject)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (!World) [[unlikely]] return nullptr;
		return GetFlecsWorld(World);
	}

	FFlecsWorld* GetFlecsWorld(const UWorld* World)
	{
		UFlecsEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UFlecsEntitySubsystem>(World);
		if (!EntitySubsystem) [[unlikely]] return nullptr;
		return &EntitySubsystem->GetMutableFlecsWorld();
	}

	FFlecsWorld& GetFlecsWorldChecked(const TNotNull<const UWorld*>& World)
	{
		UFlecsEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UFlecsEntitySubsystem>(NotNullGet(World));
		checkf(EntitySubsystem, TEXT("No FlecsEntitySubsystem found in UWorld %s"), *World->GetName());
		return EntitySubsystem->GetMutableFlecsWorld();
	}
}
