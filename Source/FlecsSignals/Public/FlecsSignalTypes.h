// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityView.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFlecsSignals, Log, All);

#define UE_API FLECSSIGNALS_API

struct FFlecsSignalNameLookup
{
	/** Max number of names each entity can contain */
	static constexpr int32 MaxSignalNames = 64;

	/** 
	 * Retrieve if it is already registered or adds new signal to the lookup and return the bitflag for that Signal
	 * @SignalName is the name of the signal to retrieve or add to the lookup.
	 * @return bitflag describing the name, or 0 if max names is reached. 
	 */
	UE_API uint64 GetOrAddSignalName(const FName SignalName);

	/**
	 * Adds specified Signal name bitflag to an entity 
	 * @param Entity is the entity where the signal has been raised
	 * @param SignalFlag is the actual bitflag describing the signal
	 */
	UE_API void AddSignalToEntity(const FFlecsEntityView Entity, const uint64 SignalFlag);

	/** 
	 * Retrieve for a specific entity the raised signal this frame
	 * @return Array of signal names raised for this entity 
	 */
	UE_API void GetSignalsForEntity(const FFlecsEntityView Entity, TArray<FName>& OutSignals) const;

	/** Empties the name lookup and entity signals */
	UE_API void Reset();

protected:
	/** Array of Signal names */
	TArray<FName> SignalNames;

	/** Map from entity id to name bitmask */
	TMap<FFlecsEntityView, uint64> EntitySignals;
};

#undef UE_API
