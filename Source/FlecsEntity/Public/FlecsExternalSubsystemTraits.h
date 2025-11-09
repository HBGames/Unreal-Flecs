// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

/**
 * Traits describing how a given piece of code can be used by Flecs. We require author or user of a given subsystem to 
 * define its traits. To do it add the following in an accessible location. 
 *
 * template<>
 * struct TFlecsExternalSubsystemTraits<UMyCustomManager>
 * {
 *		enum { GameThreadOnly = false; }
 * }
 *
 * this will let Flecs know it can access UMyCustomManager on any thread.
 *
 * This information is being used to calculate system and query dependencies as well as appropriate distribution of
 * calculations across threads.
 */
template <typename T>
struct TFlecsExternalSubsystemTraits final
{
	enum
	{
		// Unless configured otherwise each subsystem will be treated as "game-thread only".
		GameThreadOnly = true,

		// If set to true all RW and RO operations will be viewed as RO when calculating processor dependencies
		ThreadSafeWrite = !GameThreadOnly,
	};
};
