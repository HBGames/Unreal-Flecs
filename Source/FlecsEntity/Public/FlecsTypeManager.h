// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsExternalSubsystemTraits.h"

#define UE_API FLECSENTITY_API

namespace UE::Flecs
{
	/** Traits of USubsystem-based types */
	struct FSubsystemTypeTraits
	{
		FSubsystemTypeTraits() = default;

		/** Factory function for creating traits specific to a given subsystem type */
		template <typename T>
		static FSubsystemTypeTraits Make()
		{
			FSubsystemTypeTraits Traits;
			Traits.bGameThreadOnly = TFlecsExternalSubsystemTraits<T>::GameThreadOnly;
			Traits.bThreadSafeWrite = TFlecsExternalSubsystemTraits<T>::ThreadSafeWrite;
			return MoveTemp(Traits);
		}

		/** Whether the subsystem must  be run on the Game Thread */
		bool bGameThreadOnly = true;
		/** Whether the subsystem supports thread-safe write operations */
		bool bThreadSafeWrite = false;
	};
}

#undef UE_API
