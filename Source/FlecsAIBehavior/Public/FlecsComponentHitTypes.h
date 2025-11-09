// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "Engine/EngineTypes.h"
#include "FlecsEntityView.h"

#include "FlecsComponentHitTypes.generated.h"

namespace UE::Flecs::Signals
{
	const FName HitReceived = FName(TEXT("HitReceived"));
}


USTRUCT()
struct FFlecsHitResult
{
	GENERATED_BODY()

	FFlecsHitResult() = default;

	FFlecsHitResult(const FFlecsEntityView InOtherEntity, const double InTime)
		: OtherEntity(InOtherEntity),
		  HitTime(InTime),
		  LastFilteredHitTime(InTime)
	{
	}

	bool IsValid() const { return OtherEntity.IsValid(); }

	FFlecsEntityView OtherEntity;

	/**  Time when first hit was received. */
	double HitTime = 0.;

	/** Time used for filtering frequent hits. */
	double LastFilteredHitTime = 0.;
};
