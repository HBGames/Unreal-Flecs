﻿// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsGameFrameworkModule.generated.h"

UCLASS(BlueprintType, DisplayName = "Flecs Game Framework Module")
class UNREALFLECS_API UFlecsGameFrameworkModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return "Flecs GameFramework Module";
	}


}; // class UFlecsGameFrameworkModule
