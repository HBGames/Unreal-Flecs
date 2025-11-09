// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsBehaviorSettings.h"
#include "FlecsSystem_StateTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsBehaviorSettings)

UFlecsBehaviorSettings::UFlecsBehaviorSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Default values.
	MaxActivationsPerLOD[EFlecsLOD::High] = 100;
	MaxActivationsPerLOD[EFlecsLOD::Medium] = 100;
	MaxActivationsPerLOD[EFlecsLOD::Low] = 100;
	MaxActivationsPerLOD[EFlecsLOD::Off] = 100;

	DynamicStateTreeSystemClass = UFlecsSystem_StateTree::StaticClass();
}
