// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsEntityModule.h"
#include "FlecsOSAPIInitializer.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "Flecs"

void FFlecsEntityModule::StartupModule()
{
	UE::Flecs::FFlecsOSAPIInitializer::Initialize();
}

void FFlecsEntityModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FFlecsEntityModule, FlecsEntity)

#undef LOCTEXT_NAMESPACE
