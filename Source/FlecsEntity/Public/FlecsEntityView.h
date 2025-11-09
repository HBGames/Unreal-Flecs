// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsId.h"

#include "FlecsEntityView.generated.h"

#define UE_API FLECSENTITY_API

struct FFlecsEntity;

USTRUCT(BlueprintType)
struct UE_API FFlecsEntityView : public FFlecsId
{
	GENERATED_BODY()

	using Self = FFlecsEntityView;

	FFlecsEntityView() = default;

	UE_NODISCARD_CTOR FFlecsEntityView(const flecs::entity_view& InEntityView)
	{
		world_ = InEntityView.raw_world();
		id_ = InEntityView.raw_id();
	}

	/** Wrap an existing entity id.
	 *
	 * @param InWorld The world in which the entity is created.
	 * @param InId The entity id.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntityView(flecs::world_t* InWorld, const flecs::id_t InId)
		: FFlecsEntityView(flecs::entity_view(InWorld, InId))
	{
	}

	/** Implicit conversion from flecs::entity_t to flecs::entity_view. */
	UE_NODISCARD_CTOR FFlecsEntityView(flecs::entity_t InId)
		: FFlecsEntityView(flecs::entity_view(InId))
	{
	}

	operator flecs::entity_view() const { return View(); }

	flecs::entity_view View() const { return BitCast<flecs::entity_view>(*this); }

	/** Check if entity is valid.
	 * An entity is valid if:
	 * - its id is not 0
	 * - the id contains a valid bit pattern for an entity
	 * - the entity is alive (see is_alive())
	 *
	 * @return True if the entity is valid, false otherwise.
	 * @see ecs_is_valid()
	 */
	bool IsValid() const
	{
		return View().is_valid();
	}

	explicit operator bool() const { return IsValid(); }

	/** Check if entity is alive.
	 *
	 * @return True if the entity is alive, false otherwise.
	 * @see ecs_is_alive()
	 */
	bool IsAlive() const
	{
		return View().is_alive();
	}

	/** Return the entity name.
	 *
	 * @return The entity name.
	 */
	FString Name() const
	{
		return View().name().c_str();
	}

	/** Return the entity symbol.
	 *
	 * @return The entity symbol.
	 */
	FString Symbol() const
	{
		return View().symbol().c_str();
	}

	/** Return the entity path.
	 *
	 * @return The hierarchical entity path.
	 */
	FString Path(const char* InSeparator = "::", const char* InInitSeparator = "::") const
	{
		return View().path(InSeparator, InInitSeparator).c_str();
	}

	/** Return the entity path relative to a parent.
	 *
	 * @return The relative hierarchical entity path.
	 */
	FString PathFrom(const flecs::entity_t InParent, const char* InSeparator = "::", const char* InInitSeparator = "::") const
	{
		return View().path_from(InParent, InSeparator, InInitSeparator).c_str();
	}


	/** Return the entity path relative to a parent.
	 *
	 * @return The relative hierarchical entity path.
	 */
	template <typename Parent>
	FString PathFrom(const char* InSeparator = "::", const char* InInitSeparator = "::") const
	{
		return View().template path_from<Parent>(InSeparator, InInitSeparator).c_str();
	}

	bool Enabled() const
	{
		return View().enabled();
	}

	/** Get the entity's type.
	 *
	 * @return The entity's type.
	 */
};

static_assert(sizeof(FFlecsEntityView) == sizeof(flecs::entity_view), "FFlecsEntityView size mismatch with flecs::entity_view");

#undef UE_API
