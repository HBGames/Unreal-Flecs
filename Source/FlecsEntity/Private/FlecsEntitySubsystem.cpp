// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsEntitySubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntitySubsystem)

void UFlecsEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 1) convert once, store in a local so it doesn’t vanish immediately
	auto NameAnsi = StringCast<ANSICHAR>(*GetName());
	// 2) pull out the pointer (still valid until we leave this function)
	ANSICHAR* Argv0 = const_cast<ANSICHAR*>(NameAnsi.Get());
	// 3) build argv
	char* argv[] = {Argv0};
	// now StringCast<ANSICHAR> NameAnsi lives through this call,
	// so flecs::world(1, argv) sees valid memory
	FlecsWorld = FFlecsWorld(1, argv, this);
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
	Super::Tick(DeltaTime);
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
