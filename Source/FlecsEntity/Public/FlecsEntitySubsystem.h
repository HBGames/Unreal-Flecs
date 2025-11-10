// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsSubsystemBase.h"
#include "World/FlecsWorld.h"

#include "FlecsEntitySubsystem.generated.h"

#define UE_API FLECSENTITY_API

class UFlecsSystem;
/**
 * The sole responsibility of this world subsystem class is to host the default instance of FFlecsWorld
 * for a given UWorld. All the gameplay-related use cases of Flecs (found in FlecsGameplay and related plugins) 
 * use this by default.
 */
UCLASS(MinimalAPI)
class UFlecsEntitySubsystem : public UFlecsTickableSubsystemBase
{
	GENERATED_BODY()

	//~ Begin USubsystem interface
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void PostInitialize() override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;
	//~ End of USubsystem interface

public:
	UE_API UFlecsEntitySubsystem();
	UE_API ~UFlecsEntitySubsystem();

	UE_API const FFlecsWorld& GetFlecsWorld() const { return FlecsWorld; }
	UE_API FFlecsWorld& GetMutableFlecsWorld() { return FlecsWorld; }

	bool HasSystemOfExactClass(const TNotNull<TSubclassOf<UFlecsSystem>> InSystemClass) const;

	/** Creates a runtime instance of every system in the given array if there's no system of that class in the Systems array already.
	 * Call this function when adding systems to an already configured world. If you're creating one from scratch,
	 * calling any of the InitializeFrom* methods will be more efficient (and will produce same results)
	 * or call AppendOrOverrideRuntimeSystemCopies.
	 */
	void AppendUniqueRuntimeSystemCopies(TConstArrayView<const UFlecsSystem*> InSystems, const TNotNull<UObject*> InOwner, const FFlecsWorld& InFlecsWorld);

	/** Adds InSystem(s) to Systems without any additional checks. */
	void AppendSystem(const TNotNull<UFlecsSystem*> InSystem);
	void AppendSystems(TArrayView<UFlecsSystem*> InSystems);
	void AppendSystems(TArray<UFlecsSystem*>&& InSystems);

	/** @return True if the given system has indeed been added, i.e. will return false if System was already registered. */
	bool AppendUniqueSystem(const TNotNull<UFlecsSystem*> InSystem);

	/** Creates an instance of SystemClass and adds it to Systems without any additional checks. */
	void AppendSystem(const TNotNull<TSubclassOf<UFlecsSystem>> SystemClass, const TNotNull<UObject*> InOwner);

	/** @return whether the given system has been removed from hosted systems collection. */
	bool RemoveSystem(const TNotNull<const UFlecsSystem*> InSystem);

	int32 NumSystems() const { return Systems.Num(); }
	TConstArrayView<UFlecsSystem*> GetSystems() const { return ObjectPtrDecay(Systems); }
	TArrayView<TObjectPtr<UFlecsSystem>> GetMutableSystems() { return Systems; }

	/** Returns Systems array using move semantics. */
	TArray<TObjectPtr<UFlecsSystem>>&& MoveSystemsArray() { return MoveTemp(Systems); }

	/**
	 * Sorts systems aggregates in Systems array so that the ones with higher ExecutionPriority are executed first
	 * The function will also remove nullptrs, if any, before sorting.
	 */
	UE_API void SortByExecutionPriority();

protected:
	void InitializeFlecsWorld();

	void RegisterSystems();

protected:
	UPROPERTY()
	TArray<TObjectPtr<UFlecsSystem>> Systems;

	FFlecsWorld FlecsWorld;
};

#undef UE_API
