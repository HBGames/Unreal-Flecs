// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityElementTypes.h"
#include "FlecsSystem_SignalBase.h"

#include "FlecsSystem_StateTree.generated.h"

#define UE_API FLECSAIBEHAVIOR_API

/**
 * Special tag to know if the state tree has been activated
 */
USTRUCT()
struct FFlecsStateTreeActivatedTag : public FFlecsTag
{
	GENERATED_BODY()
};

/**
 * 
 */
UCLASS(MinimalAPI)
class UFlecsSystem_StateTree : public UFlecsSystem_SignalBase
{
	GENERATED_BODY()
};

#undef UE_API
