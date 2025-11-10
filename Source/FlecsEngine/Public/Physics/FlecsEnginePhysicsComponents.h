// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityElementTypes.h"
#include "Engine/EngineTypes.h"

#include "FlecsEnginePhysicsComponents.generated.h"

class UStaticMesh;

#define UE_API FLECSENGINE_API

/**
 * Component holding data related to physics collision
 */
USTRUCT()
struct FFlecsColliderComponent : public FFlecsComponent
{
	GENERATED_BODY()

	FFlecsColliderComponent() = default;

	UPROPERTY(Transient, VisibleAnywhere, Category="Debug")
	TEnumAsByte<ECollisionEnabled::Type> CollisionType = ECollisionEnabled::NoCollision;

	UPROPERTY(Transient, VisibleAnywhere, Category="Debug")
	FCollisionResponseContainer CollisionResponse;
};

#undef UE_API
