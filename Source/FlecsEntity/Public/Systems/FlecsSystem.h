// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "flecs.h"
#include "FlecsSystemTypes.h"
#include "FlecsEntityMacros.h"

#include "FlecsSystem.generated.h"

#define UE_API FLECSENTITY_API

struct FFlecsWorld;
class UFlecsPhase;

/**
 * 
 */
USTRUCT()
struct FFlecsSystemExecutionOrder
{
	GENERATED_BODY()
};

/**
 * 
 */
UCLASS(Abstract, EditInlineNew, CollapseCategories, Config="Flecs", DefaultConfig, ConfigDoNotCheckDefaults, MinimalAPI)
class UFlecsSystem : public UObject
{
	GENERATED_BODY()

public:
	UE_API UFlecsSystem();
	UE_API explicit UFlecsSystem(const FObjectInitializer& ObjectInitializer);

	bool IsInitialized() const;

	/** Calls flecs system builder and handles initialization bookkeeping. */
	UE_API void CallInitialize(const TNotNull<UObject*> InOwner, const FFlecsWorld& InFlecsWorld);

	ESystemExecutionFlags GetExecutionFlags() const;

	/** Whether this system should execute according the CurrentExecutionFlags parameters */
	bool ShouldExecute(const ESystemExecutionFlags CurrentExecutionFlags) const;

	/** Controls whether there can be multiple instances of a given class in a single FFlecsRuntimePipeline and during dependency solving. */
	bool ShouldAllowMultipleInstances() const;

	void DebugOutputDescription(FOutputDevice& Ar) const;
	UE_API virtual void DebugOutputDescription(FOutputDevice& Ar, int32 Indent) const;
	UE_API virtual FString GetSystemName() const;

	UE_API virtual TSubclassOf<UFlecsPhase> GetExecuteInPhase() const;
	UE_API virtual void SetExecuteInPhase(const TNotNull<TSubclassOf<UFlecsPhase>> InPhaseClass);

	bool IsImmediate() const;
	bool IsMultithreaded() const;

	UE_API virtual FFlecsSystemExecutionOrder& GetExecutionOrder();

#ifdef FLECS_ENABLE_SYSTEM_PRIORITY
	int32 GetPriority() const;
	void SetPriority(const int32 NewPriority);
#endif

	void MarkAsDynamic();
	bool IsDynamic() const;

	bool ShouldAutoAddToGlobalList() const;
#if WITH_EDITOR
	bool ShouldShowUpInSettings() const;
#endif

	/** Sets bAutoRegisterWithSystemPhases. Setting it to true will result in this system class being always 
	 * instantiated to be automatically evaluated every frame. @see FFlecsSystemPhaseManager
	 * Note that calling this function is only valid on CDOs. Calling it on a regular instance will fail an ensure and 
	 * have no other effect, i.e. CDO's value won't change */
	UE_API void SetShouldAutoRegisterWithGlobalList(const bool bAutoRegister);

#if CPUPROFILERTRACE_ENABLED
	FString StatId;
#endif

protected:
	/** Called to initialize the system's internal state. Override to perform custom steps. */
	UE_API virtual void InitializeInternal(UObject& InOwner, const FFlecsWorld& InFlecsWorld);

	UE_API virtual void BuildSystem(const flecs::system_builder<>& SystemBuilder);

	UE_API virtual void PostInitProperties() override;

	/** Called during the system phase to which this system is registered. */
	UE_API virtual void Run(flecs::iter& Iterator) PURE_VIRTUAL(UFlecsSystem::Run,);

protected:
	/** Configures when this given system can be executed. */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	FFlecsSystemExecutionOrder ExecutionOrder;

	/** System phase this system will be automatically run as part of. Needs to be set before the system gets
	 *  registered with FlecsSystemPhaseManager, otherwise it will have no effect. This property is usually read via
	 *  a given class's CDO, so it's recommended to set it in the constructor. */
	UPROPERTY(EditAnywhere, Category="System", Config, NoClear, meta=(AllowAbstract=false, NoResetToDefault, ShowDisplayNames))
	TSubclassOf<UFlecsPhase> ExecuteInPhase;

	/** Whether this system should be executed on StandAlone or Server or Client */
	UPROPERTY(EditAnywhere, Category="Pipeline", meta=(Bitmask, BitmaskEnum="/Script/FlecsEntity.ESystemExecutionFlags"), Config)
	uint8 ExecutionFlags = static_cast<int32>(ESystemExecutionFlags::Server | ESystemExecutionFlags::Standalone);

	/** Configures whether this system should be automatically included in the global list of systems executed every tick (see ExecuteInPhase and ExecutionOrder). */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	uint8 bAutoRegisterWithSystemPhases : 1 = true;

	/** Meant as a class property, make sure to set it in subclass' constructor. Controls whether there can be multiple
	 *  instances of a given class in a single FFlecsRuntimePipeline and during dependency solving. */
	uint8 bAllowMultipleInstances : 1 = false;

	/** Specify whether system should be run in staged context.
	 * If false system will always run staged. */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	uint8 bImmediate : 1 = false;

	/** Specify whether system can run on multiple threads.
     * If false, system will always run on a single thread. */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	uint8 bMultithreaded : 1 = false;

	/** Interval in seconds at which the system should run */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	double Interval = 0.0;

	/** Rate at which the system should run */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	int32 Rate = 0;

	/** Priority of the system */
	UPROPERTY(EditDefaultsOnly, Category="System", Config)
	int32 Priority = 0;

#if WITH_EDITORONLY_DATA
	/** Used to permanently remove a given system class from PipeSetting's listing. Used primarily for test-time 
	 *  system classes, but can also be used by project-specific code to prune the system list. */
	UPROPERTY(Config)
	uint8 bCanShowUpInSettings : 1 = true;
#endif

private:
	/**
	 * Gets set to true when an instance of the system gets added to the phase as a "dynamic system".
	 * Once set it's never expected to be cleared out to `false` thus the private visibility of the member variable.
	 * A "dynamic" system is a one that has bAutoRegisterWithSystemPhases == false, meaning it's not automatically
	 * added to the pipeline. Additionally, making systems dynamic allows one to have multiple instances
	 * of systems of the same class.
	 * @see MarkAsDynamic()
	 * @see IsDynamic()
	 */
	uint8 bIsDynamic : 1 = false;

	/** Used to track whether Initialized has been called. */
	uint8 bInitialized : 1 = false;

private:
	flecs::system OwnedSystem;

#if WITH_FLECSENTITY_DEBUG
	FString DebugDescription;
#endif
};

UCLASS(DisplayName="Test System")
class UFlecsSystem_TestSystem : public UFlecsSystem
{
	GENERATED_BODY()
};

#undef UE_API
