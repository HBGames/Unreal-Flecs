// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "FlecsType.generated.h"

#define UE_API FLECSENTITY_API

/** Type struct.
 * A type is a vector of component ids which can be requested from entities or tables.
 */
USTRUCT()
struct UE_API FFlecsType
{
	GENERATED_BODY()

	using Self = FFlecsType;

	FFlecsType() = default;

	UE_NODISCARD_CTOR FFlecsType(const flecs::type& InType)
		: world_(InType.raw_world()), type_(InType.raw_type()), empty_(InType.raw_empty())
	{
	}

	UE_NODISCARD_CTOR explicit FFlecsType(flecs::world_t* InWorld, const flecs::type_t* InType)
		: FFlecsType(flecs::type(InWorld, InType))
	{
	}

	flecs::type Type() const
	{
		return flecs::type(world_, type_);
	}

	explicit operator flecs::type() const
	{
		return Type();
	}

	/** Convert type to comma-separated string */
	FString Str() const
	{
		return Type().str().c_str();
	}

	/** Return number of ids in type */
	int32 Count() const
	{
		return Type().count();
	}

	/** Return pointer to array. */
	flecs::id_t* Array() const
	{
		return Type().array();
	}

	/** Get id at specified index in type */
	flecs::id Get(int32 InIndex) const
	{
		return Type().get(InIndex);
	}

	const flecs::id_t* begin() const
	{
		return Type().begin();
	}

	const flecs::id_t* end() const
	{
		return Type().end();
	}

	/** Implicit conversion to world_t */
	operator flecs::world_t*() const
	{
		return world_;
	}

	/** Implicit conversion to type_t */
	operator const flecs::type_t*() const
	{
		return type_;
	}

private:
	flecs::world_t* world_ = nullptr;
	const flecs::type_t* type_ = nullptr;
	flecs::id_t empty_ = 0;
};

static_assert(sizeof(FFlecsType) == sizeof(flecs::type), "FFlecsType size mismatch with flecs::type");
static_assert(alignof(FFlecsType) == alignof(flecs::type), "FFlecsType alignment mismatch with flecs::type");

#undef UE_API
