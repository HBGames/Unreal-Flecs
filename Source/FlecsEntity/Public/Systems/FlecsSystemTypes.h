// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsSystemTypes.generated.h"

/**
 * 
 */
UENUM(meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ESystemExecutionFlags : uint8
{
	None = 0 UMETA(Hidden),
	Standalone = 1 << 0,
	Server = 1 << 1,
	Client = 1 << 2,
	Editor = 1 << 3,
	EditorWorld = 1 << 4,
	AllNetModes = Standalone | Server | Client UMETA(Hidden),
	AllWorldModes = Standalone | Server | Client | EditorWorld UMETA(Hidden),
	All = Standalone | Server | Client | Editor | EditorWorld UMETA(Hidden)
};

ENUM_CLASS_FLAGS(ESystemExecutionFlags);
