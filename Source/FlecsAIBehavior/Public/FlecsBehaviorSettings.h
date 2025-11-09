// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Settings/FlecsModuleSettings.h"
#include "FlecsLODTypes.h"

#include "FlecsBehaviorSettings.generated.h"

#define UE_API FLECSAIBEHAVIOR_API

class UFlecsSystem_StateTree;

/**
 * 
 */
UCLASS(MinimalAPI, Config="Flecs", DefaultConfig, meta=(DisplayName="Flecs Behavior"))
class UFlecsBehaviorSettings : public UFlecsModuleSettings
{
	GENERATED_BODY()

public:
	UE_API UFlecsBehaviorSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category="Flecs|LOD", Config)
	int32 MaxActivationsPerLOD[EFlecsLOD::Max];

	/**
	 * Class to use when creating dynamic systems to handle given StateTree assets.
	 * Can be also set via DefaultFlecs.ini file.
	 */
	UPROPERTY(Config, EditAnywhere, Category="Flecs|StateTree", NoClear)
	TSoftClassPtr<UFlecsSystem_StateTree> DynamicStateTreeSystemClass;
};

#undef UE_API
