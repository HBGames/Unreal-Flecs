// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "flecs.h"
#include "FlecsEntity.h"

#include "FlecsPhase.generated.h"

#define UE_API FLECSENTITY_API

/**
 * Base type for mapping Flecs pipeline phases to Unreal objects.
 *
 * Each derived class represents a specific Flecs phase (OnStart, PreFrame, OnLoad, etc.)
 * and exposes the underlying Flecs phase id so systems can be registered against it.
 */
UCLASS(Abstract)
class UFlecsPhase : public UObject
{
	GENERATED_BODY()

public:
	/** Get the Flecs phase id associated with this phase. */
	UE_API virtual FFlecsEntityType GetFlecsPhaseId() const { return PhaseId; }

	/** Get the optional phase that this phase depends on. */
	UE_API virtual TSubclassOf<UFlecsPhase> GetDependsOnPhase() const { return DependsOnPhase; }

	UE_API virtual void DebugOutputDescription(FOutputDevice& Ar, int32 Indent = 0) const;

public:
	UE_API void RegisterPhase(const flecs::world& World);

protected:
	/** The Flecs phase id. */
	FFlecsEntityType PhaseId = 0;

	/** Optional phase in which this phase depends on. */
	UPROPERTY()
	TSubclassOf<UFlecsPhase> DependsOnPhase;
};

/**
 * On Start
 *
 * A special phase that is only run the first time world.progress() is called.
 * Use this for initialization logic that should only execute once for the lifetime
 * of the world (e.g., bootstrapping systems or global data).
 */
UCLASS(DisplayName="On Start")
class UFlecsPhase_OnStart : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_OnStart() { PhaseId = flecs::OnStart; }
};

/**
 * Pre Frame
 *
 * Runs at the beginning of each frame, before any of the per-frame load/update phases.
 * This is a good place to reset temporary data, merge staged data, or perform work
 * that must happen once per frame before the main simulation and I/O begin.
 */
UCLASS(DisplayName="Pre Frame")
class UFlecsPhase_PreFrame : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PreFrame() { PhaseId = flecs::PreFrame; }
};

/**
 * On Load
 *
 * This phase contains all the systems that load data into your ECS.
 * This would be a good place to load keyboard and mouse inputs, network messages,
 * or any other external data that needs to be pulled into the world.
 */
UCLASS(DisplayName="On Load")
class UFlecsPhase_OnLoad : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_OnLoad()
	{
		PhaseId = flecs::OnLoad;
		DependsOnPhase = UFlecsPhase_PreFrame::StaticClass();
	}
};

/**
 * Post Load
 *
 * Often the imported data needs to be processed.
 * Maybe you want to associate your key presses with high level actions rather than comparing
 * explicitly in your game code if the user pressed the 'K' key.
 * The PostLoad phase is a good place for this kind of input interpretation or normalization.
 */
UCLASS(DisplayName="Post Load")
class UFlecsPhase_PostLoad : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PostLoad()
	{
		PhaseId = flecs::PostLoad;
		DependsOnPhase = UFlecsPhase_OnLoad::StaticClass();
	}
};

/**
 * Pre Update
 *
 * Now that the input is loaded and processed, it's time to get ready to start processing our game logic.
 * Anything that needs to happen after input processing but before processing the game logic can happen here.
 * This can be a good place to prepare the frame, maybe clean up some things from the previous frame, etcetera.
 */
UCLASS(DisplayName="Pre Update")
class UFlecsPhase_PreUpdate : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PreUpdate()
	{
		PhaseId = flecs::PreUpdate;
		DependsOnPhase = UFlecsPhase_PostLoad::StaticClass();
	}
};

/**
 * On Update
 *
 * This is usually where the magic happens!
 * This is where you put all of your gameplay systems.
 * By default, systems are added to this phase.
 */
UCLASS(DisplayName="On Update")
class UFlecsPhase_OnUpdate : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_OnUpdate()
	{
		PhaseId = flecs::OnUpdate;
		DependsOnPhase = UFlecsPhase_PreUpdate::StaticClass();
	}
};

/**
 * On Validate
 *
 * This phase was introduced to deal with validating the state of the game after processing the gameplay systems.
 * Sometimes you moved entities too close to each other, or the speed of an entity is increased too much.
 * This phase is for righting that wrong. A typical feature to implement in this phase would be collision detection.
 */
UCLASS(DisplayName="On Validate")
class UFlecsPhase_OnValidate : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_OnValidate()
	{
		PhaseId = flecs::OnValidate;
		DependsOnPhase = UFlecsPhase_OnUpdate::StaticClass();
	}
};

/**
 * Post Update
 *
 * When your game logic has been updated, and your validation pass has run,
 * you may want to apply some corrections. For example, if your collision detection system detected
 * collisions in the OnValidate phase, you may want to move the entities so that they no longer overlap.
 */
UCLASS(DisplayName="Post Update")
class UFlecsPhase_PostUpdate : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PostUpdate()
	{
		PhaseId = flecs::PostUpdate;
		DependsOnPhase = UFlecsPhase_OnValidate::StaticClass();
	}
};

/**
 * Pre Store
 *
 * Now that all the frame data is computed, validated and corrected for,
 * it is time to prepare the frame for rendering. Any systems that need to run before rendering,
 * but after processing the game logic should go here.
 * A good example would be a system that calculates transform matrices from a scene graph.
 */
UCLASS(DisplayName="Pre Store")
class UFlecsPhase_PreStore : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PreStore()
	{
		PhaseId = flecs::PreStore;
		DependsOnPhase = UFlecsPhase_PostUpdate::StaticClass();
	}
};

/**
 * On Store
 *
 * This is where it all comes together. Your frame is ready to be rendered, and that is exactly
 * what you would do in this phase. Submit render commands, copy data to render buffers,
 * or otherwise hand off the final state of the world to your renderer.
 */
UCLASS(DisplayName="On Store")
class UFlecsPhase_OnStore : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_OnStore()
	{
		PhaseId = flecs::OnStore;
		DependsOnPhase = UFlecsPhase_PreStore::StaticClass();
	}
};

/**
 * Post Frame
 *
 * Runs after the frame has been stored / rendered.
 * Use this to perform end-of-frame cleanup, bookkeeping, profiling, or scheduling work
 * that should only happen once all other phases for the frame have completed.
 */
UCLASS(DisplayName="Post Frame")
class UFlecsPhase_PostFrame : public UFlecsPhase
{
	GENERATED_BODY()

public:
	UFlecsPhase_PostFrame()
	{
		PhaseId = flecs::PostFrame;
		DependsOnPhase = UFlecsPhase_OnStore::StaticClass();
	}
};

#undef UE_API
