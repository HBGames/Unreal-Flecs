// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityElementTypes.h"
#include "Engine/StaticMesh.h"

#include "FlecsEngineMeshComponents.generated.h"

#define UE_API FLECSENGINE_API

/**
 * Component holding a pointer to the static mesh associated to the entity
 */
USTRUCT()
struct FFlecsStaticMeshComponent : public FFlecsComponent
{
	GENERATED_BODY()

	FFlecsStaticMeshComponent() = default;
	UE_API explicit FFlecsStaticMeshComponent(TNotNull<const UStaticMesh*> InMesh);

	UPROPERTY(Transient, VisibleAnywhere, Category=Debug)
	TWeakObjectPtr<const UStaticMesh> Mesh;

	UPROPERTY(Transient, VisibleAnywhere, Category = Debug)
	TArray<TWeakObjectPtr<UMaterialInterface>> OverrideMaterials;
};

#undef UE_API
