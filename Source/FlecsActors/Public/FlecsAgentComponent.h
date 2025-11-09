// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once
#include "FlecsEntityView.h"

#include "FlecsAgentComponent.generated.h"

#define UE_API FLECSACTORS_API

UENUM()
enum class EFlecsAgentComponentState : uint8
{
	None, // Default starting state

	// States of actors needing mass entity creation
	EntityPendingCreation, // Actor waiting for entity creation
	EntityCreated, // Actor with a created entity

	// States are for Actor driven by Mass (puppet) needing fragments initialization
	PuppetPendingInitialization, // Puppet actor queued for fragments initialization
	PuppetInitialized, // Puppet actor with all initialized fragments
	PuppetPaused, // Puppet actor with all fragments removed 
	PuppetPendingReplication, // Replicated puppet actor waiting for NetID
	PuppetReplicatedOrphan, // Replicated puppet actor not associated to a MassAgent
};

/** 
 *  There are two primary use cases for this component:
 *  1. If placed on an AActor blueprint it lets the user specify additional fragments that will be created for 
 *     entities spawned based on this given blueprint. 
 *  2. If present on an actor in the world it makes it communicate with the FlecsSimulation which will create an 
 *     entity representing given actor. Use case 1) will also be applicable in this case. The component is unregistered by 
 *     default and requires manual enabling via an 'Enable' call.
 * 
 *  @todo use case 2) is currently sitting in a shelved CL of Epic Games. Will be worked on next.
 */
UCLASS(MinimalAPI, Blueprintable, ClassGroup="Flecs", meta=(BlueprintSpawnableComponent), HideCategories=(Sockets,Collision))
class UFlecsAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FFlecsEntityView GetEntityView() const { return AgentHandle; }

protected:
	FFlecsEntityView AgentHandle;
};

#undef UE_API
