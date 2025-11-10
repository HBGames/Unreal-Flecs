// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Systems/FlecsSystemTypes.h"
#include "World/FlecsWorld.h"

#define UE_API FLECSENTITY_API

namespace UE::Flecs::Utils
{
	/** Returns the current execution mode for the systems calculated from the world network mode */
	UE_API extern EFlecsSystemExecutionFlags GetSystemExecutionFlagsForWorld(const UWorld& World);

	/** Based on the given World (which can be null) and `ExecutionFlagsOverride`, this function determines the execution flags to use. */
	UE_API extern EFlecsSystemExecutionFlags DetermineSystemExecutionFlags(const UWorld* World, EFlecsSystemExecutionFlags ExecutionFlagsOverride = EFlecsSystemExecutionFlags::None);

	/** based on the given World (which can be null), this function determines the additional level tick types for the pipelines */
	uint8 DetermineSystemSupportedTickTypes(const UWorld* World);


	/**
	* AbstractSort is a sorting function that only needs to know how many items there are, how to compare items
	* at individual locations - where location is in [0, NumElements) - and how to swap two elements at given locations.
	* The main use case is to sort multiple arrays while keeping them in sync. For example:
	*
	* TArray<float> Lead = { 3.1, 0.2, 2.6, 1.0 };
	* TArray<UObject*> Payload = { A, B, C, D };
	*
	* AbstractSort(Lead.Num()										// NumElements
	* 	, [&Lead](const int32 LHS, const int32 RHS)					// Predicate
	*		{
	*			return Lead[LHS] < Lead[RHS];
	*		}
	* 	, [&Lead, &Payload](const int32 A, const int32 B)			// SwapFunctor
	*	 	{
	*			Swap(Lead[A], Lead[B]);
	* 			Swap(Payload[A], Payload[B]);
	*		}
	* );
	*/
	template <typename TPred, typename TSwap>
	inline void AbstractSort(const int32 NumElements, TPred&& Predicate, TSwap&& SwapFunctor)
	{
		if (NumElements == 0)
		{
			return;
		}

		TArray<int32> Indices;
		Indices.AddUninitialized(NumElements);
		int i = 0;
		do
		{
			Indices[i] = i;
		}
		while (++i < NumElements);

		Indices.Sort(Predicate);

		for (i = 0; i < NumElements; ++i)
		{
			int32 SwapFromIndex = Indices[i];
			while (SwapFromIndex < i)
			{
				SwapFromIndex = Indices[SwapFromIndex];
			}

			if (SwapFromIndex != i)
			{
				SwapFunctor(i, SwapFromIndex);
			}
		}
	}

	UE_API extern FFlecsWorld* GetFlecsWorld(const UObject* WorldContextObject);
	UE_API extern FFlecsWorld* GetFlecsWorld(const UWorld* World);
	UE_API extern FFlecsWorld& GetFlecsWorldChecked(const TNotNull<const UWorld*>& World);
}

#undef UE_API
