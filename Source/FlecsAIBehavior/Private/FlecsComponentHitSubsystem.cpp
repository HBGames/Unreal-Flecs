// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsComponentHitSubsystem.h"

#include "FlecsAgentComponent.h"
#include "FlecsAgentSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "FlecsSignals/Public/FlecsSignalSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsComponentHitSubsystem)

namespace UE::FlecsComponentHit
{
	bool bOnlyProcessHitsFromPlayers = true;

	FAutoConsoleVariableRef ConsoleVariables[] =
	{
		FAutoConsoleVariableRef(
			TEXT("ai.flecs.OnlyProcessHitsFromPlayers"),
			bOnlyProcessHitsFromPlayers,
			TEXT("Activates extra filtering to ignore hits from actors that are not controlled by the player."),
			ECVF_Cheat)
	};
}

const FFlecsHitResult* UFlecsComponentHitSubsystem::GetLastHit(const FFlecsEntityView Entity) const
{
	return HitResults.Find(Entity);
}

void UFlecsComponentHitSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UFlecsSimulationSubsystem>();

	SignalSubsystem = Collection.InitializeDependency<UFlecsSignalSubsystem>();
	checkfSlow(SignalSubsystem != nullptr, TEXT("FlecsSignalSubsystem is required"));

	AgentSubsystem = Collection.InitializeDependency<UFlecsAgentSubsystem>();
	checkfSlow(AgentSubsystem != nullptr, TEXT("FlecsAgentSubsystem is required"));

	AgentSubsystem->GetOnFlecsAgentComponentEntityAssociated().AddLambda([this](const UFlecsAgentComponent& AgentComponent)
	{
		if (UCapsuleComponent* CapsuleComponent = AgentComponent.GetOwner()->FindComponentByClass<UCapsuleComponent>())
		{
			RegisterForComponentHit(AgentComponent.GetEntityView(), *CapsuleComponent);
		}
	});

	AgentSubsystem->GetOnFlecsAgentComponentEntityDetaching().AddLambda([this](const UFlecsAgentComponent& AgentComponent)
	{
		if (UCapsuleComponent* CapsuleComponent = AgentComponent.GetOwner()->FindComponentByClass<UCapsuleComponent>())
		{
			UnregisterForComponentHit(AgentComponent.GetEntityView(), *CapsuleComponent);
		}
	});

	OverrideSubsystemTraits<UFlecsComponentHitSubsystem>(Collection);
}

void UFlecsComponentHitSubsystem::Deinitialize()
{
	checkfSlow(AgentSubsystem != nullptr, TEXT("FlecsAgentSubsystem must have be set during initialization"));
	AgentSubsystem->GetOnFlecsAgentComponentEntityAssociated().RemoveAll(this);
	AgentSubsystem->GetOnFlecsAgentComponentEntityDetaching().RemoveAll(this);

	Super::Deinitialize();
}

void UFlecsComponentHitSubsystem::Tick(float DeltaTime)
{
	const TNotNull<const UWorld*> World = GetWorld();

	const double CurrentTime = World->GetTimeSeconds();
	constexpr double HitResultDecayDuration = 1.;

	for (auto Iter = HitResults.CreateIterator(); Iter; ++Iter)
	{
		const FFlecsHitResult& HitResult = Iter.Value();
		const double ElapsedTime = CurrentTime - HitResult.LastFilteredHitTime;
		if (ElapsedTime > HitResultDecayDuration)
		{
			Iter.RemoveCurrent();
		}
	}
}

TStatId UFlecsComponentHitSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsComponentHitSubsystem, STATGROUP_Tickables);
}

void UFlecsComponentHitSubsystem::RegisterForComponentHit(const FFlecsEntityView Entity, UCapsuleComponent& CapsuleComponent)
{
	EntityToComponentMap.Add(Entity, &CapsuleComponent);
	ComponentToEntityMap.Add(&CapsuleComponent, Entity);
	CapsuleComponent.OnComponentHit.AddDynamic(this, &UFlecsComponentHitSubsystem::OnHitCallback);
}

void UFlecsComponentHitSubsystem::UnregisterForComponentHit(const FFlecsEntityView Entity, UCapsuleComponent& CapsuleComponent)
{
	EntityToComponentMap.Remove(Entity);
	ComponentToEntityMap.Remove(&CapsuleComponent);
	CapsuleComponent.OnComponentHit.RemoveAll(this);
}

void UFlecsComponentHitSubsystem::OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const TNotNull<const UWorld*> World = GetWorld();


	const FFlecsEntityView Entity = ComponentToEntityMap.FindChecked(HitComp);
	FFlecsEntityView* OtherEntity = ComponentToEntityMap.Find(OtherComp);

	bool bProcessHit = (OtherEntity != nullptr && OtherEntity->IsSet());
	if (bProcessHit && UE::FlecsComponentHit::bOnlyProcessHitsFromPlayers)
	{
		const APawn* HitActorAsPawn = (HitComp != nullptr) ? Cast<APawn>(HitComp->GetOwner()) : nullptr;
		const APawn* OtherAsPawn = Cast<APawn>(OtherActor);
		bProcessHit = (HitActorAsPawn != nullptr && HitActorAsPawn->IsPlayerControlled()) || (OtherAsPawn != nullptr && OtherAsPawn->IsPlayerControlled());
	}

	const double CurrentTime = World->GetTimeSeconds();

	// If new hit result comes during this duration, it will be merged to existing one.
	constexpr double HitResultMergeDuration = 1.;
	if (bProcessHit)
	{
		FFlecsHitResult* ExistingHitResult = HitResults.Find(Entity);
		if (ExistingHitResult)
		{
			const double TimeSinceLastHit = CurrentTime - ExistingHitResult->LastFilteredHitTime;
			if (TimeSinceLastHit < HitResultMergeDuration)
			{
				ExistingHitResult->LastFilteredHitTime = CurrentTime;
				bProcessHit = false;
			}
		}
	}

	if (bProcessHit)
	{
		HitResults.Add(Entity, {*OtherEntity, CurrentTime});

		checkfSlow(SignalSubsystem != nullptr, TEXT("FlecsSignalSubsystem must have be set during initialization"));
		SignalSubsystem->SignalEntity(UE::Flecs::Signals::HitReceived, Entity);
	}
}
