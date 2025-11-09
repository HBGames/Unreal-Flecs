// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "Phases/FlecsPhase.h"
#include "FlecsEntityMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPhase)

void UFlecsPhase::DebugOutputDescription(FOutputDevice& Ar, int32 Indent) const
{
#if WITH_FLECSENTITY_DEBUG
	const UClass* PhaseClass = GetClass();
	const FText PhaseDisplayName = PhaseClass->GetDisplayNameText();

	// Build a human-readable description of our dependency (if any)
	FString DependsOnText = TEXT("None");
	if (DependsOnPhase)
	{
		const UClass* DependsClass = DependsOnPhase.Get();
		const FText DependsDisplayName = DependsClass->GetDisplayNameText();

		DependsOnText = FString::Printf(
			TEXT("%s (%s)"),
			*DependsDisplayName.ToString(),
			*DependsClass->GetName()
		);
	}

	// We still *use* PhaseId, but only as a simple registered flag
	const bool bIsRegistered = (PhaseId != 0);

	Ar.Logf(
		TEXT("%*sPhase: %s (%s), DependsOn: %s, Status: %s"),
		Indent,
		TEXT(""),
		*PhaseDisplayName.ToString(),
		*PhaseClass->GetName(),
		*DependsOnText,
		bIsRegistered ? TEXT("Registered") : TEXT("Not registered")
	);
#endif
}

void UFlecsPhase::RegisterPhase(const flecs::world& World)
{
	checkf(World, TEXT("Invalid Flecs world when registering phase %s"), *GetName());

	// Check if this phase is already registered.
	if (PhaseId == 0) return;

	World.defer([this, &World]()
	{
		flecs::entity PhaseEntity = World.entity().add(flecs::Phase);
		if (DependsOnPhase)
		{
			const UFlecsPhase* DependsOnPhaseCDO = GetDefault<UFlecsPhase>(DependsOnPhase);
			PhaseEntity.add(flecs::DependsOn, DependsOnPhaseCDO->GetFlecsPhaseId());
		}

		PhaseId = PhaseEntity.id();
	});
}
