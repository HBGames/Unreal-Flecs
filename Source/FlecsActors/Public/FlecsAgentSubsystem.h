// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsSubsystemBase.h"

#include "FlecsAgentSubsystem.generated.h"

#define UE_API FLECSACTORS_API

class UFlecsAgentComponent;

namespace UE::FlecsActor
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsAgentComponentDelegate, const UFlecsAgentComponent& /*AgentComponent*/);
}

/**
 * 
 */
UCLASS(MinimalAPI)
class UFlecsAgentSubsystem : public UFlecsSubsystemBase
{
	GENERATED_BODY()

public:
	/**
	 * @return The delegate of when FlecsAgentComponent gets associated to a flecs entity
	 */
	UE::FlecsActor::FFlecsAgentComponentDelegate& GetOnFlecsAgentComponentEntityAssociated()
	{
		return OnFlecsAgentComponentEntityAssociated;
	}

	/**
	 * @return The delegate of when FlecsAgentComponent is detaching from its flecs entity
	 */
	UE::FlecsActor::FFlecsAgentComponentDelegate& GetOnFlecsAgentComponentEntityDetaching()
	{
		return OnFlecsAgentComponentEntityDetaching;
	}

protected:
	UE::FlecsActor::FFlecsAgentComponentDelegate OnFlecsAgentComponentEntityAssociated;
	UE::FlecsActor::FFlecsAgentComponentDelegate OnFlecsAgentComponentEntityDetaching;
};

#undef UE_API
