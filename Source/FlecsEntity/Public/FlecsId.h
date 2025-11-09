// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "FlecsId.generated.h"

#define UE_API FLECSENTITY_API

/** Ids are the things that can be added to an entity.
 * An id can be an entity or pair, and can have optional id flags. */
typedef flecs::id_t FFlecsIdType;

struct FFlecsEntityView;
struct FFlecsEntity;

/** Struct that wraps around a flecs::id_t.
 * A flecs id is an identifier that can be added to entities. Ids can be:
 * - entities (including components, tags)
 * - pair ids
 * - entities with id flags set (like flecs::AUTO_OVERRIDE, flecs::TOGGLE)
 */
USTRUCT(BlueprintType)
struct UE_API FFlecsId
{
	GENERATED_BODY()

	using Self = FFlecsId;

	FFlecsId() = default;

	UE_NODISCARD_CTOR explicit FFlecsId(const flecs::id& InId)
		: world_(InId.raw_world()), id_(InId.raw_id())
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(flecs::id_t InValue)
		: FFlecsId(flecs::id(InValue))
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(flecs::world_t* InWorld, const flecs::id_t InValue = 0)
		: FFlecsId(flecs::id(InWorld, InValue))
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(flecs::world_t* InWorld, const flecs::id_t InFirst, const flecs::id_t InSecond)
		: FFlecsId(flecs::id(InWorld, InFirst, InSecond))
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(flecs::world_t* InWorld, const char* InExpression)
		: FFlecsId(flecs::id(InWorld, InExpression))
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(const flecs::id_t InFirst, const flecs::id_t InSecond)
		: FFlecsId(flecs::id(InFirst, InSecond))
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsId(const flecs::id& InFirst, const flecs::id& InSecond)
		: FFlecsId(flecs::id(InFirst, InSecond))
	{
	}

	flecs::id Id() const
	{
		return BitCast<flecs::id>(*this);
	}

	operator flecs::id() const { return Id(); }
	operator flecs::id_t() const { return id_; }
	operator flecs::world_t*() const { return world_; }

	/** Test if id is pair (has first, second) */
	bool IsPair() const { return Id().is_pair(); }

	/** Test if id is a wildcard */
	bool IsWildcard() const { return Id().is_wildcard(); }

	/** Test if id is entity */
	bool IsEntity() const { return Id().is_entity(); }

	/** Return id as entity (only allowed when id is valid entity) */
	FFlecsEntity Entity() const;

	/** Return id with role added */
	FFlecsEntity AddFlags(const flecs::id_t InFlags) const;

	/** Return id with role removed */
	FFlecsEntity RemoveFlags(const flecs::id_t InFlags) const;

	/** Return id without role */
	FFlecsEntity RemoveFlags() const;

	/** Return id without role */
	FFlecsEntity RemoveGeneration() const;

	/** Return component type of id */
	FFlecsEntity TypeId() const;

	/** Test if id has specified role */
	bool HasFlags(const flecs::id_t InFlags) const
	{
		return Id().has_flags(InFlags);
	}

	/** Test if id has any role */
	bool HasFlags() const
	{
		return Id().has_flags();
	}

	/** Return id flags set on id */
	FFlecsEntity Flags() const;

	/** Test if id has specified first */
	bool HasRelation(const flecs::id_t InFirst) const
	{
		return Id().has_relation(InFirst);
	}

	/** Get first element from a pair.
	 * If the id is not a pair, this operation will fail. When the id has a
	 * world, the operation will ensure that the returned id has the correct
	 * generation count. */
	FFlecsEntity First() const;

	/** Get second element from a pair.
	 * If the id is not a pair, this operation will fail. When the id has a
	 * world, the operation will ensure that the returned id has the correct
	 * generation count. */
	FFlecsEntity Second() const;

	/* Convert id to string */
	FString Str() const
	{
		return Id().str().c_str();
	}

	/* Convert id to string */
	FString ToString() const
	{
		return Str();
	}

	/** Convert role of id to string */
	FString FlagsStr() const
	{
		return Id().flags_str().c_str();
	}

	flecs::world GetWorld() const
	{
		return Id().world();
	}

	flecs::world_t* GetRawWorld() const
	{
		return world_;
	}

	flecs::id_t GetRawId() const
	{
		return id_;
	}

protected:
	/* World is optional, but guarantees that entity identifiers extracted from
	 * the id are valid */
	flecs::world_t* world_ = nullptr;
	flecs::id_t id_ = 0;
};

static_assert(sizeof(FFlecsId) == sizeof(flecs::id), "FFlecsId size mismatch with flecs::id");
static_assert(alignof(FFlecsId) == alignof(flecs::id), "FFlecsId alignment mismatch with flecs::id");

#undef UE_API
