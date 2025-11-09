// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "flecs.h"
#include "FlecsEntity.h"
#include "FlecsEntityMacros.h"

#include "FlecsWorld.generated.h"

#define UE_API FLECSENTITY_API

/** A world is the container for all ECS data and supporting features.
 * Applications can have multiple worlds, though in most cases will only need
 * one. Worlds are isolated from each other, and can have separate sets of
 * systems, components, modules etc.
 *
 * If an application has multiple worlds with overlapping components, it is
 * common (though not strictly required) to use the same component ids across
 * worlds, which can be achieved by declaring a global component id variable.
 * To do this in the C API, see the entities/fwd_component_decl example. The
 * C++ API automatically synchronizes component ids between worlds.
 *
 * Component id conflicts between worlds can occur when a world has already used
 * an id for something else. There are a few ways to avoid this:
 *
 * - Ensure to register the same components in each world, in the same order.
 * - Create a dummy world in which all components are preregistered which
 *   initializes the global id variables.
 *
 * In some use cases, typically when writing tests, multiple worlds are created
 * and deleted with different components, registered in different order. To
 * ensure isolation between tests, the C++ API has a `flecs::reset` function
 * that forces the API to ignore the old component ids. */
typedef flecs::world_t FFlecsWorldType;

/**
 * The world.
 * 
 * The world is the container of all ECS data and systems. If the world is
 * deleted, all data in the world will be deleted as well.
 */
USTRUCT()
struct FFlecsWorld
{
	GENERATED_BODY()

	/** Create a Flecs world. */
	FFlecsWorld()
		: World(flecs::world()),
		  Owner(nullptr)
	{
		// TypeMapComponent = GetTypeMapComponent(); // NOLINT(cppcoreguidelines-prefer-member-initializer)
		// check(TypeMapComponent);
#if WITH_FLECSENTITY_DEBUG
		InitDebugName();
#endif
	}

	/** Create world with command line arguments.
	 * Currently command line arguments are not interpreted, but they may be
	 * used in the future to configure Flecs parameters.
	 */
	UE_NODISCARD_CTOR explicit FFlecsWorld(const int argc, char* argv[], UObject* InOwner)
		: World(argc, argv),
		  Owner(InOwner)
	{
#if WITH_FLECSENTITY_DEBUG
		InitDebugName();
#endif
	}

	/** Create world from C world. */
	UE_NODISCARD_CTOR explicit FFlecsWorld(FFlecsWorldType* InWorld, UObject* InOwner)
		: World(InWorld),
		  Owner(InOwner)
	{
#if WITH_FLECSENTITY_DEBUG
		InitDebugName();
#endif
	}

	/** Implicit conversion from flecs::world. */
	UE_NODISCARD_CTOR FFlecsWorld(const flecs::world& InWorld, UObject* InOwner)
		: World(InWorld),
		  Owner(InOwner)
	{
#if WITH_FLECSENTITY_DEBUG
		InitDebugName();
#endif
	}

	// Let flecs::world handle reference counting and lifetime.
	FFlecsWorld(const FFlecsWorld&) = default;
	FFlecsWorld(FFlecsWorld&&) noexcept = default;
	FFlecsWorld& operator=(const FFlecsWorld&) = default;
	FFlecsWorld& operator=(FFlecsWorld&&) noexcept = default;

	// No need to call World.release() explicitly; flecs::world::~world already does.
	~FFlecsWorld() = default;

	/** 
	 * Fetches the world associated with the Owner. 
	 * @note that it's ok for a given FlecsWorld to not have an owner or the owner not being part of a UWorld, depending on the use case
	 */
	UWorld* GetWorld() const { return Owner.IsValid() ? Owner->GetWorld() : nullptr; }
	UObject* GetOwner() const { return Owner.Get(); }

	/* Releases the underlying world object. If this is the last handle, the world will be finalized. */
	void Release() { World.release(); }

	/** Implicit conversion to flecs::world. */
	operator const flecs::world&() const { return World; }

	/** Implicit conversion to flecs::world_t*. */
	operator flecs::world_t*() const { return World; }

	/** Make current world object owner of the world. This may only be called on
	 * one flecs::world object, and may only be called once. Failing to do so
	 * will result in undefined behavior.
	 * 
	 * This operation allows a custom (C) world to be wrapped by a C++ object,
	 * and transfer ownership so that the world is automatically cleaned up.
	 */
	void MakeOwner() { World.make_owner(); }

	/** Deletes and recreates the world. */
	void Reset() { World.reset(); }

	/** Obtain pointer to C world object. */
	flecs::world_t* C_Ptr() const { return World.c_ptr(); }

	/** Signal application should quit. After calling this operation, the next call to Progress() returns false. */
	void Quit() const { World.quit(); }

	/** Register action to be executed when world is destroyed. */
	void AtFini(ecs_fini_action_t Action, void* Context = nullptr) const { World.atfini(Action, Context); }

	/** Test if Quit() has been called. */
	bool ShouldQuit() const { return World.should_quit(); }


	/** Begin frame.
	 * When an application does not use progress() to control the main loop, it
	 * can still use Flecs features such as FPS limiting and time measurements.
	 * This operation needs to be invoked whenever a new frame is about to get
	 * processed.
	 *
	 * Calls to FrameBegin() must always be followed by FrameEnd().
	 *
	 * The function accepts a DeltaTime parameter, which will get passed to
	 * systems. This value is also used to compute the amount of time the
	 * function needs to sleep to ensure it does not exceed the target_fps, when
	 * it is set. When 0 is provided for DeltaTime, the time will be measured.
	 *
	 * This function should only be run from the main thread.
	 *
	 * @param InDeltaTime Time elapsed since the last frame.
	 * @return The provided DeltaTime, or measured time if 0 was provided.
	 *
	 * @see ecs_frame_begin()
	 * @see FFlecsWorld::FrameEnd()
	 */
	float FrameBegin(const float InDeltaTime = 0.0f) const { return World.frame_begin(InDeltaTime); }


	/** End frame.
	 * This operation must be called at the end of the frame, and always after
	 * FrameBegin().
	 *
	 * This function should only be run from the main thread.
	 *
	 * @see ecs_frame_end()
	 * @see FFlecsWorld::FrameBegin()
	 */
	void FrameEnd() const { World.frame_end(); }


	/** Begin readonly mode.
	 *
	 * @param bMultiThreaded Whether to enable readonly/multi-threaded mode.
	 * 
	 * @return Whether world is currently readonly.
	 *
	 * @see ecs_readonly_begin()
	 * @see FFlecsWorld::IsReadonly()
	 * @see FFlecsWorld::ReadonlyEnd()
	 */
	bool ReadonlyBegin(const bool bMultiThreaded = false) const { return World.readonly_begin(bMultiThreaded); }

	/** End readonly mode.
	 * 
	 * @see ecs_readonly_end()
	 * @see FFlecsWorld::IsReadonly()
	 * @see FFlecsWorld::ReadonlyBegin()
	 */
	void ReadonlyEnd() const { World.readonly_end(); }


	/** Defer operations until end of frame.
	 * When this operation is invoked while iterating, operations in between the
	 * DeferBegin() and DeferEnd() operations are executed at the end of the frame.
	 *
	 * This operation is thread safe.
	 *
	 * @return true if world changed from non-deferred mode to deferred mode.
	 *
	 * @see ecs_defer_begin()
	 * @see FFlecsWorld::Defer()
	 * @see FFlecsWorld::DeferEnd()
	 * @see FFlecsWorld::IsDeferred()
	 * @see FFlecsWorld::DeferResume()
	 * @see FFlecsWorld::DeferSuspend()
	*/
	bool DeferBegin() const { return World.defer_begin(); }

	/** End block of operations to defer.
	 * @see DeferBegin().
	 *
	 * This operation is thread safe.
	 *
	 * @return true if world changed from deferred mode to non-deferred mode.
	 *
	 * @see ecs_defer_end()
	 * @see FFlecsWorld::Defer()
	 * @see FFlecsWorld::DeferBegin()
	 * @see FFlecsWorld::IsDeferred()
	 * @see FFlecsWorld::DeferResume()
	 * @see FFlecsWorld::DeferSuspend()
	 */
	bool DeferEnd() const { return World.defer_end(); }

	/** Test whether deferring is enabled.
	 *
	 * @return True if deferred, false if not.
	 *
	 * @see ecs_is_deferred()
	 * @see FFlecsWorld::Defer()
	 * @see FFlecsWorld::DeferBegin()
	 * @see FFlecsWorld::DeferEnd()
	 * @see FFlecsWorld::DeferResume()
	 * @see FFlecsWorld::DeferSuspend()
	 */
	bool IsDeferred() const { return World.is_deferred(); }


	/** Configure world to have N stages.
	 * This initializes N stages, which allows applications to defer operations to
	 * multiple isolated defer queues. This is typically used for applications with
	 * multiple threads, where each thread gets its own queue, and commands are
	 * merged when threads are synchronized.
	 *
	 * Note that set_threads() already creates the appropriate number of stages.
	 * The set_stage_count() operation is useful for applications that want to manage
	 * their own stages and/or threads.
	 *
	 * @param InStages The number of stages.
	 *
	 * @see ecs_set_stage_count()
	 * @see FFlecsWorld::GetStageCount()
	 */
	void SetStageCount(const int InStages) const { World.set_stage_count(InStages); }

	/** Get number of configured stages.
	 * Return number of stages set by set_stage_count().
	 *
	 * @return The number of stages used for threading.
	 *
	 * @see ecs_get_stage_count()
	 * @see FFlecsWorld::SetStageCount()
	 */
	int32 GetStageCount() const { return World.get_stage_count(); }

	/** Get current stage id.
	 * The stage id can be used by an application to learn about which stage it
	 * is using, which typically corresponds with the worker thread id.
	 *
	 * @return The stage id.
	 */
	int32 GetStageId() const { return World.get_stage_id(); }

	/** Test if is a stage.
	 * If this function returns false, it is guaranteed that this is a valid
	 * world object.
	 *
	 * @return True if the world is a stage, false if not.
	 */
	bool IsStage() const { return World.is_stage(); }

	/** Merge world or stage.
	 * When automatic merging is disabled, an application can call this
	 * operation on either an individual stage, or on the world which will merge
	 * all stages. This operation may only be called when staging is not enabled
	 * (either after progress() or after readonly_end()).
	 *
	 * This operation may be called on an already merged stage or world.
	 *
	 * @see ecs_merge()
	 */
	void Merge() const { World.merge(); }

	/** Get stage-specific world pointer.
	 * Flecs threads can safely invoke the API as long as they have a private
	 * context to write to, also referred to as the stage. This function returns a
	 * pointer to a stage, disguised as a world pointer.
	 *
	 * Note that this function does not(!) create a new world. It simply wraps the
	 * existing world in a thread-specific context, which the API knows how to
	 * unwrap. The reason the stage is returned as an ecs_world_t is so that it
	 * can be passed transparently to the existing API functions, vs. having to
	 * create a dedicated API for threading.
	 *
	 * @param InStageId The index of the stage to retrieve.
	 * @return A thread-specific pointer to the world.
	 */
	FFlecsWorld GetStage(const int32 InStageId) const { return FFlecsWorld(World.get_stage(InStageId), Owner.Get()); }

	/** Create asynchronous stage.
	 * An asynchronous stage can be used to asynchronously queue operations for
	 * later merging with the world. An asynchronous stage is similar to a regular
	 * stage, except that it does not allow reading from the world.
	 *
	 * Asynchronous stages are never merged automatically, and must therefore be
	 * manually merged with the ecs_merge function. It is not necessary to call
	 * defer_begin or defer_end before and after enqueuing commands, as an
	 * asynchronous stage unconditionally defers operations.
	 *
	 * The application must ensure that no commands are added to the stage while the
	 * stage is being merged.
	 *
	 * @return The stage.
	 */
	FFlecsWorld AsyncStage() const { return FFlecsWorld(World.async_stage(), Owner.Get()); }


	/** Get actual world.
	 * If the current object points to a stage, this operation will return the
	 * actual world.
	 *
	 * @return The actual world.
	 */
	FFlecsWorld GetFlecsWorld() const { return FFlecsWorld(World.get_world(), Owner.Get()); }


	/** Test whether the current world object is readonly.
	 * This function allows the code to test whether the currently used world
	 * object is readonly or whether it allows for writing.
	 *
	 * @return True if the world or stage is readonly.
	 *
	 * @see ecs_stage_is_readonly()
	 * @see FFlecsWorld::ReadonlyBegin()
	 * @see FFlecsWorld::ReadonlyEnd()
	 */
	bool IsReadonly() const { return World.is_readonly(); }

	/** Set world context.
	 * Set a context value that can be accessed by anyone that has a reference
	 * to the world.
	 *
	 * @param ctx A pointer to a user defined structure.
	 * @param ctx_free A function that is invoked with ctx when the world is freed.
	 *
	 *
	 * @see ecs_set_ctx()
	 * @see FFlecsWorld::GetContext()
	 */
	void SetContext(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const { World.set_ctx(ctx, ctx_free); }

	/** Get world context.
	 * This operation retrieves a previously set world context.
	 *
	 * @return The context set with set_binding_ctx(). If no context was set, the
	 *         function returns NULL.
	 *
	 * @see ecs_get_ctx()
	 * @see FFlecsWorld::SetContext()
	 */
	void* GetContext() const { return World.get_ctx(); }

	/** Set world binding context.
	 *
	 * Same as set_ctx() but for binding context. A binding context is intended
	 * specifically for language bindings to store binding specific data.
	 *
	 * @param ctx A pointer to a user defined structure.
	 * @param ctx_free A function that is invoked with ctx when the world is freed.
	 *
	 * @see ecs_set_binding_ctx()
	 * @see FFlecsWorld::GetBindingContext()
	 */
	void SetBindingContext(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const { World.set_binding_ctx(ctx, ctx_free); }

	/** Get world binding context.
	 * This operation retrieves a previously set world binding context.
	 *
	 * @return The context set with set_binding_ctx(). If no context was set, the
	 *         function returns NULL.
	 *
	 * @see ecs_get_binding_ctx()
	 * @see FFlecsWorld::SetBindingContext()
	 */
	void* GetBindingContext() const { return World.get_binding_ctx(); }

	/** Preallocate memory for number of entities.
	 * This function preallocates memory for the entity index.
	 *
	 * @param InEntityCount Number of entities to preallocate memory for.
	 *
	 * @see ecs_dim()
	 */
	void Dim(const int32 InEntityCount) const { World.dim(InEntityCount); }

	/** Set entity range.
	 * This function limits the range of issued entity ids between min and max.
	 *
	 * @param InMin Minimum entity id issued.
	 * @param InMax Maximum entity id issued.
	 *
	 * @see ecs_set_entity_range()
	 */
	void SetEntityRange(const FFlecsEntityType InMin, const FFlecsEntityType InMax) const { World.set_entity_range(InMin, InMax); }

	/** Enforce that operations cannot modify entities outside of range.
	 * This function ensures that only entities within the specified range can
	 * be modified. Use this function if specific parts of the code only are
	 * allowed to modify a certain set of entities, as could be the case for
	 * networked applications.
	 *
	 * @param bInEnabled True if range check should be enabled, false if not.
	 *
	 * @see ecs_enable_range_check()
	 */
	void EnableRangeCheck(const bool bInEnabled) const { World.enable_range_check(bInEnabled); }

	/** Set current scope.
	 *
	 * @param InScope The scope to set.
	 * @return The current scope;
	 *
	 * @see ecs_set_scope()
	 * @see FFlecsWorld::GetScope()
	 */
	FFlecsEntity SetScope(const FFlecsEntity& InScope) const { return FFlecsEntity(World.set_scope(InScope.Entity())); }

	/** Get current scope.
	 *
	 * @return The current scope.
	 *
	 * @see ecs_get_scope()
	 * @see FFlecsWorld::SetScope()
	 */
	FFlecsEntity GetScope() const { return FFlecsEntity(World.get_scope()); }

	/** Same as set_scope but with type.
	 *
	 * @see ecs_set_scope()
	 * @see FFlecsWorld::GetScope()
	 */
	template <typename T>
	FFlecsEntity SetScope() const { return FFlecsEntity(World.set_scope<T>()); }

	/** Set search path.
	 *
	 * @see ecs_set_lookup_path()
	 * @see FFlecsWorld::Lookup()
	 */
	FFlecsEntityType* SetLookupPath(const FFlecsEntityType* InSearchPath) const { return World.set_lookup_path(InSearchPath); }

	/** Lookup entity by name.
	 *
	 * @param InName Entity name.
	 * @param bInRecursive When false, only the current scope is searched.
	 * @result The entity if found, or 0 if not found.
	 */
	FFlecsEntity Lookup(const FString& InName, const FString& InSeperator = "::", const FString& InRootSeperator = "::", bool bInRecursive = true) const
	{
		return FFlecsEntity(World.lookup(
			reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InName).Get()),
			reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InSeperator).Get()),
			reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InRootSeperator).Get()),
			bInRecursive));
	}

	/** Set singleton component. */
	template <typename T, flecs::if_t<!flecs::is_callable<T>::value> = 0>
	void Set(const T& Value) const { World.set<T>(Value); }

	/** Set singleton component. */
	template <typename T, flecs::if_t<!flecs::is_callable<T>::value> = 0>
	void Set() const { World.set<T>(); }

	/** Set singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>,
	          typename A = flecs::actual_type_t<P>, flecs::if_not_t<flecs::is_pair<First>::value> = 0>
	void Set(const Second& InValue) const { World.set<First, Second, P, A>(InValue); }

	/** Set singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>,
	          typename A = flecs::actual_type_t<P>, flecs::if_not_t<flecs::is_pair<First>::value> = 0>
	void Set(A&& InValue) const { World.set<First, Second, P, A>(Forward<A>(InValue)); }

	/** Set singleton pair. */
	template <typename First, typename Second>
	void Set() const { World.set<First, Second>(); }

	/** Set singleton pair. */
	template <typename First, typename Second>
	void Set(Second InSecond, First&& InValue) const { World.set<First, Second>(InSecond, Forward<First>(InValue)); }

	/** Set singleton component inside a callback. */
	template <typename Func, flecs::if_t<flecs::is_callable<Func>::value> = 0>
	void Set(const Func& InFunc) const { World.set<Func>(InFunc); }

	template <typename T, typename... Args>
	void Emplace(Args&&... InArgs) const { World.emplace<T, Args>(Forward<Args>(InArgs)...); }

	/** Ensure singleton component. */
	template <typename T>
	T& Obtain() const { return World.obtain<T>(); }

	/** Mark singleton component as modified. */
	template <typename T>
	void Modified() const { World.modified<T>(); }

	/** Get ref singleton component. */
	template <typename T>
	flecs::ref<T> GetRef() const { return World.get_ref<T>(); }

	/** TryGet singleton component. */
	const void* TryGet(FFlecsIdType InId) const { return World.try_get(InId); }

	/** TryGet singleton pair. */
	const void* TryGet(FFlecsEntityType InFirst, FFlecsEntityType InSecond) const { return World.try_get(InFirst, InSecond); }

	/** TryGet singleton component. */
	template <typename T>
	const T* TryGet() const { return World.try_get<T>(); }

	/** TryGet singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>, typename A = flecs::actual_type_t<P>>
	const A* TryGet() const { return World.try_get<First, Second, P, A>(); }

	/** TryGet singleton pair. */
	template <typename First, typename Second>
	const First* TryGet(Second InSecond) const { return World.try_get<First, Second>(InSecond); }

	/** Get singleton component. */
	const void* Get(FFlecsIdType InId) const { return World.get(InId); }

	/** Get singleton component. */
	const void* Get(FFlecsEntityType InFirst, FFlecsEntityType InSecond) const { return World.get(InFirst, InSecond); }

	/** Get singleton component. */
	template <typename T>
	const T& Get() const { return World.get<T>(); }

	/** Get singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>, typename A = flecs::actual_type_t<P>>
	const A& Get() const { return World.get<First, Second, P, A>(); }

	/** Get singleton pair. */
	template <typename First, typename Second>
	const First& Get(Second InSecond) const { return World.get<First, Second>(InSecond); }

	/** Get singleton component inside a callback. */
	template <typename Func, flecs::if_t<flecs::is_callable<Func>::value> = 0>
	void Get(const Func& InFunc) const { World.get<Func>(InFunc); }

	/** TryGet mutable singleton component. */
	void* TryGetMut(FFlecsIdType InId) const { return World.try_get_mut(InId); }

	/** TryGet mutable singleton pair. */
	void* TryGetMut(FFlecsEntityType InFirst, FFlecsEntityType InSecond) const { return World.try_get_mut(InFirst, InSecond); }

	/** TryGet mutable singleton pair. */
	template <typename T>
	T* TryGetMut() const { return World.try_get_mut<T>(); }

	/** TryGet mutable singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>, typename A = flecs::actual_type_t<P>>
	A* TryGetMut() const { return World.try_get_mut<First, Second, P, A>(); }

	/** TryGet mutable singleton pair. */
	template <typename First, typename Second>
	First* TryGetMut(Second InSecond) const { return World.try_get_mut<First, Second>(InSecond); }

	/** Get mutable singleton component. */
	void* GetMut(FFlecsIdType InId) const { return World.get_mut(InId); }

	/** Get mutable singleton pair. */
	void* GetMut(FFlecsEntityType InFirst, FFlecsEntityType InSecond) const { return World.get_mut(InFirst, InSecond); }

	template <typename T>
	T& GetMut() const { return World.get_mut<T>(); }

	/** Get mutable singleton pair. */
	template <typename First, typename Second, typename P = flecs::pair<First, Second>, typename A = flecs::actual_type_t<P>>
	A& GetMut() const { return World.get_mut<First, Second, P, A>(); }

	/** Get mutable singleton pair. */
	template <typename First, typename Second>
	First& GetMut(Second InSecond) const { return World.get_mut<First, Second>(InSecond); }

	/** Test if world has singleton component.
	 * 
	 * @tparam T The component to check.
	 * @return Whether the world has the singleton component.
	*/
	template <typename T>
	bool Has() const { return World.has<T>(); }

	/** Test if world has the provided pair.
	 *
	 * @tparam First The first element of the pair
	 * @tparam Second The second element of the pair
	 * @return Whether the world has the singleton pair.
	 */
	template <typename First, typename Second>
	bool Has() const { return World.has<First, Second>(); }

	/** Test if world has the provided pair.
	 *
	 * @tparam First The first element of the pair
	 * @param InSecond The second element of the pair.
	 * @return Whether the world has the singleton pair.
	*/
	template <typename First>
	bool Has(const FFlecsIdType InSecond) const { return World.has<First>(InSecond); }

	/** Test if world has the provided pair.
	 *
	 * @param InFirst The first element of the pair
	 * @param InSecond The second element of the pair
	 * @return Whether the world has the singleton pair.
	 */
	bool Has(const FFlecsIdType InFirst, const FFlecsIdType InSecond) const { return World.has(InFirst, InSecond); }

	/** Check for enum singleton constant 
	 * 
	 * @tparam E The enum type.
	 * @param InValue The enum constant to check.
	 * @return Whether the world has the specified enum constant.
	 */
	template <typename E, flecs::if_t<flecs::is_enum<E>::value> = 0>
	bool Has(const E InValue) const { return World.has<E>(InValue); }

	/** Add singleton component. */
	template <typename T>
	void Add() const { World.add<T>(); }

	/** Adds a pair to the singleton component.
	 *
	 * @tparam First The first element of the pair
	 * @tparam Second The second element of the pair
	 */
	template <typename First, typename Second>
	void Add() const { World.add<First, Second>(); }

	/** Adds a pair to the singleton component.
	 *
	 * @tparam First The first element of the pair
	 * @param InSecond The second element of the pair.
	 */
	template <typename First>
	void Add(const FFlecsEntityType InSecond) const { World.add<First>(InSecond); }

	/** Adds a pair to the singleton entity.
	 *
	 * @param InFirst The first element of the pair
	 * @param InSecond The second element of the pair
	 */
	void Add(const FFlecsEntityType InFirst, const FFlecsEntityType InSecond) const { World.add(InFirst, InSecond); }

	/** Add enum singleton constant 
	 * 
	 * @tparam E The enum type.
	 * @param InValue The enum constant.
	 */
	template <typename E, flecs::if_t<flecs::is_enum<E>::value> = 0>
	void Add(E InValue) const { World.add<E>(InValue); }

	/** Remove singleton component. */
	template <typename T>
	void Remove() const { World.remove<T>(); }

	/** Removes the pair singleton component.
	 *
	 * @tparam First The first element of the pair
	 * @tparam Second The second element of the pair
	 */
	template <typename First, typename Second>
	void Remove() const { World.remove<First, Second>(); }

	/** Removes the pair singleton component.
	 *
	 * @tparam First The first element of the pair
	 * @param InSecond The second element of the pair.
	 */
	template <typename First>
	void Remove(const FFlecsEntityType InSecond) const { World.remove<First>(InSecond); }

	/** Removes the pair singleton component.
	 *
	 * @param InFirst The first element of the pair
	 * @param InSecond The second element of the pair
	 */
	void Remove(const FFlecsEntityType InFirst, const FFlecsEntityType InSecond) const { World.remove(InFirst, InSecond); }

	/** Iterate entities in root of world
	 * Accepts a callback with the following signature:
	 *
	 * @code
	 * void(*)(flecs::entity e);
	 * @endcode
	 */
	template <typename Func>
	void Children(Func&& InFunc) const { World.children(Forward<Func>(InFunc)); }


	/** Get singleton entity for type. */
	template <typename T>
	FFlecsEntity Singleton() const { return FFlecsEntity(World.singleton<T>()); }

	/** Get target for a given pair from a singleton entity.
	 * This operation returns the target for a given pair. The optional
	 * index can be used to iterate through targets, in case the entity has
	 * multiple instances for the same relationship.
	 *
	 * @tparam First The first element of the pair.
	 * @param InIndex The index (0 for the first instance of the relationship).
	 */
	template <typename First>
	FFlecsEntity Target(const int32 InIndex = 0) const { return FFlecsEntity(World.target<First>(InIndex)); }

	/** Get target for a given pair from a singleton entity.
	 * This operation returns the target for a given pair. The optional
	 * index can be used to iterate through targets, in case the entity has
	 * multiple instances for the same relationship.
	 *
	 * @param InFirst The first element of the pair for which to retrieve the target.
	 * @param InIndex The index (0 for the first instance of the relationship).
	 */
	template <typename T>
	FFlecsEntity Target(const FFlecsEntityType InFirst, const int32 InIndex = 0) const { return FFlecsEntity(World.target<T>(InFirst, InIndex)); }

	/** Get target for a given pair from a singleton entity.
	 * This operation returns the target for a given pair. The optional
	 * index can be used to iterate through targets, in case the entity has
	 * multiple instances for the same relationship.
	 *
	 * @param InFirst The first element of the pair for which to retrieve the target.
	 * @param InIndex The index (0 for the first instance of the relationship).
	 */
	FFlecsEntity Target(const FFlecsEntityType InFirst, const int32 InIndex = 0) const { return FFlecsEntity(World.target(InFirst, InIndex)); }

	/** Create alias for component.
	 *
	 * @tparam T to create an alias for.
	 * @param InAlias Alias for the component.
	 * @return Entity representing the component.
	 */
	template <typename T>
	FFlecsEntity Use(const FString& InAlias) const { return FFlecsEntity(World.use<T>(reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InAlias).Get()))); }

	/** Create alias for entity.
	 *
	 * @param InName Name of the entity.
	 * @param InAlias Alias for the entity.
	 */
	FFlecsEntity Use(const FString& InName, const FString& InAlias) const { return FFlecsEntity(World.use(reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InName).Get()), reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InAlias).Get()))); }

	/** Create alias for entity.
	 *
	 * @param InEntity Entity for which to create the alias.
	 * @param InAlias Alias for the entity.
	 */
	void Use(const FFlecsEntity& InEntity, const FString& InAlias) const { World.use(InEntity.Entity(), reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InAlias).Get())); }

	/** Count entities matching a component.
	 *
	 * @param InComponentId The component id.
	 */
	int32 Count(const FFlecsIdType InComponentId) const { return World.count(InComponentId); }

	/** Count entities matching a pair.
	 *
	 * @param InFirst The first element of the pair.
	 * @param InSecond The second element of the pair.
	 */
	int32 Count(const FFlecsEntityType InFirst, const FFlecsEntityType InSecond) const { return World.count(InFirst, InSecond); }

	/** Count entities matching a component.
	 *
	 * @tparam T The component type.
	 */
	template <typename T>
	int32 Count() const { return World.count<T>(); }

	/** Count entities matching a pair.
	 *
	 * @tparam First The first element of the pair.
	 * @param InSecond The second element of the pair.
	 */
	template <typename First>
	int Count(const FFlecsEntityType InSecond) const { return World.count<First>(InSecond); }

	/** Count entities matching a pair.
	 *
	 * @tparam First The first element of the pair.
	 * @tparam Second The second element of the pair.
	 */
	template <typename First, typename Second>
	int32 Count() const { return World.count<First, Second>(); }

	/** All entities created in function are created with id. */
	template <typename Func>
	void With(const FFlecsIdType InWithId, const Func& InFunc) const { World.with<Func>(InWithId, InFunc); }

	/** All entities created in function are created with type. */
	template <typename T, typename Func>
	void With(const Func& InFunc) const { World.with<T, Func>(InFunc); }

	/** All entities created in function are created with pair. */
	template <typename First, typename Second, typename Func>
	void With(const Func& InFunc) const { World.with<First, Second, Func>(InFunc); }

	/** All entities created in function are created with pair. */
	template <typename First, typename Func>
	void With(const FFlecsIdType Second, const Func& InFunc) const { World.with<First, Func>(Second, InFunc); }

	/** All entities created in function are created with pair. */
	template <typename Func>
	void With(const FFlecsIdType InFirst, const FFlecsIdType InSecond, const Func& InFunc) const { World.with<Func>(InFirst, InSecond, InFunc); }

	/** All entities created in function are created in scope. All operations
	* called in function (such as lookup) are relative to scope. */
	template <typename Func>
	void Scope(const FFlecsIdType Parent, const Func& InFunc) const { World.scope<Func>(Parent, InFunc); }

	/** Same as scope(parent, func), but with T as parent. */
	template <typename T, typename Func>
	void Scope(const Func& InFunc) const { World.scope<T, Func>(InFunc); }

	/** Use provided scope for operations ran on returned world.
	 * Operations need to be ran in a single statement. */
	flecs::scoped_world Scope(const FFlecsIdType Parent) const { return World.scope(Parent); }

	template <typename T>
	flecs::scoped_world Scope() const { return World.scope<T>(); }

	flecs::scoped_world Scope(const FString& InName) const { return World.scope(reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InName).Get())); }

	/** Delete all entities with specified id. */
	void DeleteWith(const FFlecsIdType InId) const { World.delete_with(InId); }

	/** Delete all entities with specified pair. */
	void DeleteWith(const FFlecsEntityType InFirst, const FFlecsEntityType InSecond) const { World.delete_with(InFirst, InSecond); }

	/** Delete all entities with specified component. */
	template <typename T>
	void DeleteWith() const { World.delete_with<T>(); }

	/** Delete all entities with specified pair. */
	template <typename First, typename Second>
	void DeleteWith() const { World.delete_with<First, Second>(); }

	/** Delete all entities with specified pair. */
	template <typename First>
	void DeleteWith(const FFlecsEntityType InSecond) const { World.delete_with<First>(InSecond); }

	/** Remove all instances of specified id. */
	void RemoveAll(const FFlecsIdType InId) const { World.remove_all(InId); }

	/** Remove all instances of specified pair. */
	void RemoveAll(const FFlecsEntityType First, const FFlecsEntityType Second) const { World.remove_all(First, Second); }

	/** Remove all instances of specified component. */
	template <typename T>
	void RemoveAll() const { World.remove_all<T>(); }

	/** Remove all instances of specified pair. */
	template <typename First, typename Second>
	void RemoveAll() const { World.remove_all<First, Second>(); }

	/** Remove all instances of specified pair. */
	template <typename First>
	void RemoveAll(const FFlecsEntityType InSecond) const { World.remove_all<First>(InSecond); }

	/** Defer all operations called in function.
	 *
	 * @see FFlecsWorld::DeferBegin()
	 * @see FFlecsWorld::DeferEnd()
	 * @see FFlecsWorld::DeferIsDeferred()
	 * @see FFlecsWorld::DeferResume()
	 * @see FFlecsWorld::DeferSuspend()
	 */
	template <typename Func>
	void Defer(const Func& InFunc) const { World.defer<Func>(InFunc); }

	/** Suspend deferring operations.
	 *
	 * @see ecs_defer_suspend()
	 * @see FFlecsWorld::Defer()
	 * @see FFlecsWorld::DeferBegin()
	 * @see FFlecsWorld::DeferEnd()
	 * @see FFlecsWorld::DeferIsDeferred()
	 * @see FFlecsWorld::DeferResume()
	 */
	void DeferSuspend() const { World.defer_suspend(); }

	/** Resume deferring operations.
	 *
	 * @see ecs_defer_resume()
	 * @see FFlecsWorld::Defer()
	 * @see FFlecsWorld::DeferBegin()
	 * @see FFlecsWorld::DeferEnd()
	 * @see FFlecsWorld::DeferIsDeferred()
	 * @see FFlecsWorld::DeferSuspend()
	 */
	void DeferResume() const { World.defer_resume(); }

	/** Check if entity id exists in the world.
	 *
	 * @see ecs_exists()
	 * @see FFlecsWorld::IsAlive()
	 * @see FFlecsWorld::IsValid()
	 */
	bool Exists(const FFlecsEntityType InEntity) const { return World.exists(InEntity); }

	/** Check if entity id exists in the world.
	 *
	 * @see ecs_is_alive()
	 * @see FFlecsWorld::Exists()
	 * @see FFlecsWorld::IsValid()
	 */
	bool IsAlive(const FFlecsEntityType InEntity) const { return World.is_alive(InEntity); }

	/** Check if entity id is valid.
	 * Invalid entities cannot be used with API functions.
	 *
	 * @see ecs_is_valid()
	 * @see FFlecsWorld::Exists()
	 * @see FFlecsWorld::IsAlive()
	 */
	bool IsValid(const FFlecsEntityType InEntity) const { return World.is_valid(InEntity); }

	/** Get alive entity for id.
	 * Returns the entity with the current generation.
	 *
	 * @see ecs_get_alive()
	 */
	FFlecsEntity GetAlive(const FFlecsEntityType InEntity) const { return FFlecsEntity(World.get_alive(InEntity)); }

	/**
	 * @see ecs_make_alive()
	 */
	FFlecsEntity MakeAlive(const FFlecsEntityType InEntity) const { return FFlecsEntity(World.make_alive(InEntity)); }

	/** Set version of entity provided.
     * 
     * @see ecs_set_version()
     */
	void SetVersion(const FFlecsEntityType InEntity) const { World.set_version(InEntity); }

	/** Run callback after completing frame. */
	void RunPostFrame(ecs_fini_action_t InAction, void* InContext) const { World.run_post_frame(InAction, InContext); }

	/** Get the world info.
	 *
	 * @see ecs_get_world_info()
	 */
	const flecs::world_info_t* GetInfo() const { return World.get_info(); }

	/** Get delta_time */
	ecs_ftime_t GetDeltaTime() const { return World.delta_time(); }

	/** Free unused memory. 
	 * 
	 * @see ecs_shrink()
	 */
	void Shrink() const { World.shrink(); }

	/** Begin exclusive access
	 * 
	 * @param InThreadName Optional thread name for improved debug messages.
	 * @see ecs_exclusive_access_begin()
	 */
	void ExclusiveAccessBegin(const FString& InThreadName = "") { World.exclusive_access_begin(reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*InThreadName).Get())); }

	/** End exclusive access
	 * 
	 * @param bInLockWorld Lock world for all threads, allow readonly operations.
	 * @see ecs_exclusive_access_end()
	 */
	void ExclusiveAccessEnd(const bool bInLockWorld = false) { World.exclusive_access_end(bInLockWorld); }

	/** Return component id if it has been registered.
	 * This operation is similar to world::id() but will never automatically 
	 * register the component.
	 * 
	 * @tparam T The type for which to obtain the id.
	 */
	template <typename T>
	FFlecsIdType IdIfRegistered() const { return World.id_if_registered<T>(); }

	/** Return type info */
	const flecs::type_info_t* TypeInfo(const FFlecsIdType InComponent) { return World.type_info(InComponent); }

	/** Return type info */
	const flecs::type_info_t* TypeInfo(const FFlecsEntityType InFirst, const FFlecsEntityType InSecond) { return World.type_info(InFirst, InSecond); }

	/** Return type info */
	template <typename T>
	const flecs::type_info_t* TypeInfo() { return World.type_info<T>(); }

	/** Return type info */
	template <typename Second>
	const flecs::type_info_t* TypeInfo(const FFlecsEntityType First) { return World.type_info<Second>(First); }

	/** Return type info */
	template <typename R, typename T>
	const flecs::type_info_t* TypeInfo() { return World.type_info<R, T>(); }

	flecs::system System(FFlecsEntity InEntity) const { return World.system(InEntity.Entity()); }

	template <typename... Comps, typename... Args>
	flecs::system_builder<Comps...> System(Args&&... InArgs) const { return World.system<Comps...>(Forward<Args>(InArgs)...); }

private:
	flecs::world World;

#if WITH_FLECSENTITY_DEBUG

	void InitDebugName() { DebugName = Owner ? (Owner->GetName() + TEXT("_FlecsWorld")) : TEXT("Unset"); }

	FString DebugName;
#endif

	/** Optional UObject that conceptually owns / is associated with this world. */
	TWeakObjectPtr<UObject> Owner;

	uint8* TypeMapComponent = nullptr;
};

#undef UE_API
