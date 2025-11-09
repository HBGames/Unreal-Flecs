// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsTypeManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsEntityConcepts.h"

#include "FlecsSubsystemBase.generated.h"

#define UE_API FLECSENTITY_API

struct FFlecsWorld;

namespace UE::Flecs
{
	namespace Subsystems
	{
		struct FInitializationState
		{
			uint8 bInitializeCalled : 1 = false;
			uint8 bPostInitializeCalled : 1 = false;
			uint8 bOnWorldBeginPlayCalled : 1 = false;
		};

		UE_API void RegisterSubsystemType(FSubsystemCollectionBase& InCollection, TSubclassOf<USubsystem> InSubsystemClass, FSubsystemTypeTraits&& InTraits);
		UE_API void RegisterSubsystemType(const FFlecsWorld& InFlecsWorld, TSubclassOf<USubsystem> InSubsystemClass, FSubsystemTypeTraits&& InTraits);
	}
}

/** 
 * The sole responsibility of this world subsystem class is to serve functionality common to all 
 * Flecs-related UWorldSubsystem-based subsystems, like whether the subsystems should get created at all. 
 */
UCLASS(Abstract, Config="Flecs", DefaultConfig, MinimalAPI)
class UFlecsSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UE_API bool AreRuntimeFlecsSubsystemsAllowed(UObject* Outer);
	UE::Flecs::Subsystems::FInitializationState GetInitializationState() const { return InitializationState; }

protected:
	//~USubsystem interface
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void PostInitialize() override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~End of USubsystem interface

	/**
	 * Needs to be called in Initialize for subsystems we want to behave properly when dynamically added after UWorld::BeginPlay
	 * (for example via GameplayFeatureActions). This is required for subsystems relying on their PostInitialize and/or OnWorldBeginPlay called.
	 */
	UE_API void HandleLateCreation();

	/**
	 * Registers given subsystem class as part of Flecs type information. Needs to be called as part of Initialize override.
	 * Note that calling the function is only required if the registered traits differ from the parent class'.
	 */
	template <UE::Flecs::CSubsystem T>
	void OverrideSubsystemTraits(FSubsystemCollectionBase& Collection)
	{
		UE::Flecs::Subsystems::RegisterSubsystemType(Collection, T::StaticClass(), UE::Flecs::FSubsystemTypeTraits::Make<T>());
	}

	/**
	 * Tracks which initialization function had already been called. Requires the child classes to call Super implementation
	 * for their Initialize, PostInitialize, Deinitialize and OnWorldBeginPlayCalled overrides
	 */
	UE::Flecs::Subsystems::FInitializationState InitializationState;
};


/**
 * The sole responsibility of this tickable world subsystem class is to serve functionality common to all
 * Flecs-related UTickableWorldSubsystem-based subsystems, like whether the subsystems should get created at all.
 */
UCLASS(Abstract, Config="Flecs", DefaultConfig, MinimalAPI)
class UFlecsTickableSubsystemBase : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UE::Flecs::Subsystems::FInitializationState GetInitializationState() const { return InitializationState; }

protected:
	//~USubsystem interface
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void PostInitialize() override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~End of USubsystem interface

	/**
	 * Registers given subsystem class as part of Flecs type information. Needs to be called as part of Initialize override.
	 * Note that calling the function is only required if the registered traits differ from the parent class'.
	 */
	template <UE::Flecs::CSubsystem T>
	void OverrideSubsystemTraits(FSubsystemCollectionBase& Collection)
	{
		UE::Flecs::Subsystems::RegisterSubsystemType(Collection, T::StaticClass(), UE::Flecs::FSubsystemTypeTraits::Make<T>());
	}

	/**
	 * Needs to be called in Initialize for subsystems we want to behave properly when dynamically added after UWorld::BeginPlay
	 * (for example via GameplayFeatureActions). This is required for subsystems relying on their PostInitialize and/or OnWorldBeginPlay called.
	 */
	UE_API void HandleLateCreation();

private:
	/** 
	 * Tracks which initialization function had already been called. Requires the child classes to call Super implementation
	 * for their Initialize, PostInitialize, Deinitialize and OnWorldBeginPlayCalled overrides
	 */
	UE::Flecs::Subsystems::FInitializationState InitializationState;
};

#undef UE_API
