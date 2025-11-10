// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "Mesh/FlecsEngineMeshComponents.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEngineMeshComponents)

FFlecsStaticMeshComponent::FFlecsStaticMeshComponent(TNotNull<const UStaticMesh*> InMesh)
	: Mesh(InMesh)
{
}
