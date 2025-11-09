// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityElementTypes.generated.h"

USTRUCT()
struct FFlecsElement
{
	GENERATED_BODY()
};

// This is the base class for all components
USTRUCT()
struct FFlecsComponent : public FFlecsElement
{
	GENERATED_BODY()
};

// these are the messages we'll print out when static checks whether a given type is a component fails
#define _FLECS_INVALID_COMPONENT_CORE_MESSAGE "Make sure to inherit from FFlecsComponent or one of its child-types and ensure that the struct is trivially copyable, or opt out by specializing TFlecsFragmentTraits for this type and setting AuthorAcceptsItsNotTriviallyCopyable = true"
#define FLECS_INVALID_COMPONENT_MSG  "Given struct doesn't represent a valid component type." _FLECS_INVALID_COMPONENT_CORE_MESSAGE
#define FLECS_INVALID_COMPONENT_MSG_F  "Type %s is not a valid component type." _FLECS_INVALID_COMPONENT_CORE_MESSAGE

// This is the base class for types that will only be tested for presence/absence, i.e. Tags.
// Subclasses should never contain any member properties.
USTRUCT()
struct FFlecsTag : public FFlecsElement
{
	GENERATED_BODY()
};

namespace UE::Flecs
{
	template <typename T>
	bool IsA(const UStruct* /*Struct*/)
	{
		return false;
	}

	template <>
	inline bool IsA<FFlecsElement>(const UStruct* Struct)
	{
		// @todo check that it's not any of the directly inherited types (that are "abstract" in their nature until inherited from themselves). 
		return Struct && Struct->IsChildOf(TBaseStructure<FFlecsElement>::Get());
	}

	template <>
	inline bool IsA<FFlecsComponent>(const UStruct* Struct)
	{
		return Struct && Struct->IsChildOf(TBaseStructure<FFlecsComponent>::Get());
	}

	template <>
	inline bool IsA<FFlecsTag>(const UStruct* Struct)
	{
		return Struct && Struct->IsChildOf(TBaseStructure<FFlecsTag>::Get());
	}
}
