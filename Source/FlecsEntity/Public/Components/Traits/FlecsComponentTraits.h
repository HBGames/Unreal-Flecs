// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

/** 
 * Component traits.
 * @see TFlecsExternalSubsystemTraits
 */
template <typename T>
struct TFlecsComponentTraits final
{
	enum
	{
		// Component types are best kept trivially copyable for performance reasons.
		// To enforce that we test this trait when checking if a given type is a valid component type.
		// This test can be skipped by specifically opting out, which also documents that 
		// making the given type non-trivially-copyable was a deliberate decision. 
		AuthorAcceptsItsNotTriviallyCopyable = false
	};
};
