// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsModuleSettings.generated.h"

#define UE_API FLECSENTITY_API

/** 
 * A common parent for Flecs's per-module settings.
 * Classes extending this class will automatically get registered
 * with the Flecs Settings and show under its section in Project Settings.
 */
UCLASS(MinimalAPI, Abstract, Config=Flecs, DefaultConfig, CollapseCategories)
class UFlecsModuleSettings : public UObject
{
	GENERATED_BODY()

protected:
	UE_API virtual void PostInitProperties() override;
};

/**
 *
 */
UCLASS(MinimalAPI, Config="Flecs", DefaultConfig, DisplayName="Flecs", AutoExpandCategories="Flecs")
class UFlecsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UE_API void RegisterModuleSettings(UFlecsModuleSettings& SettingsCDO);

public:
	UPROPERTY(VisibleAnywhere, Category="Flecs", NoClear, EditFixedSize, meta=(EditInline))
	TMap<FName, TObjectPtr<UFlecsModuleSettings>> ModuleSettings;
};

#undef UE_API
