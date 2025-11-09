// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsComponentHitTypes.h"
#include "FlecsSubsystemBase.h"

#include "FlecsComponentHitSubsystem.generated.h"

#define UE_API FLECSAIBEHAVIOR_API

class UFlecsAgentSubsystem;
class UCapsuleComponent;
class UFlecsSignalSubsystem;
class UFlecsSimulationSubsystem;

/**
 * 
 */
UCLASS(MinimalAPI)
class UFlecsComponentHitSubsystem : public UFlecsTickableSubsystemBase
{
	GENERATED_BODY()

public:
	UE_API const FFlecsHitResult* GetLastHit(const FFlecsEntityView Entity) const;

protected:
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;

	UE_API void RegisterForComponentHit(const FFlecsEntityView Entity, UCapsuleComponent& CapsuleComponent);
	UE_API void UnregisterForComponentHit(FFlecsEntityView Entity, UCapsuleComponent& CapsuleComponent);

	UFUNCTION()
	UE_API void OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	TObjectPtr<UFlecsSignalSubsystem> SignalSubsystem;

	UPROPERTY()
	TObjectPtr<UFlecsAgentSubsystem> AgentSubsystem;

	UPROPERTY()
	TMap<FFlecsEntityView, FFlecsHitResult> HitResults;

	UPROPERTY()
	TMap<TObjectPtr<UActorComponent>, FFlecsEntityView> ComponentToEntityMap;

	UPROPERTY()
	TMap<FFlecsEntityView, TObjectPtr<UActorComponent>> EntityToComponentMap;
};

template <>
struct TFlecsExternalSubsystemTraits<UFlecsComponentHitSubsystem>
{
	enum
	{
		GameThreadOnly = false,
		ThreadSafeWrite = false,
	};
};

#undef UE_API
