// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsModuleSettings.h"

#include "FlecsEntitySettings.generated.h"

#define UE_API FLECSENTITY_API

class UFlecsPhase;
class UFlecsSystem;

USTRUCT()
struct FFlecsSystemPhaseConfig
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Flecs", Config, NoClear, meta=(AllowAbstract=false, NoResetToDefault, ShowDisplayNames))
	TSubclassOf<UFlecsPhase> PhaseClass;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UFlecsSystem>> SystemCDOs;

#if WITH_EDITORONLY_DATA
	// This phase is available only in editor since it's used to present the user the order in which systems
	// will be executed when given phase gets triggered
	UPROPERTY(Transient)
	TObjectPtr<UFlecsPhase> PhaseCDO;

	UPROPERTY(VisibleAnywhere, Category="Flecs", Transient)
	FText Description;
#endif
};

/**
 * Implements the settings for the FlecsEntity module
 */
UCLASS(MinimalAPI, Config="Flecs", DefaultConfig, DisplayName="Flecs Entity")
class UFlecsEntitySettings : public UFlecsModuleSettings
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSettingsChange, const FPropertyChangedEvent& /*PropertyChangedEvent*/);
#endif
	DECLARE_MULTICAST_DELEGATE(FOnInitialized);

	UE_API UFlecsEntitySettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UE_API void Initialize();

	UE_API TConstArrayView<FFlecsSystemPhaseConfig> GetSystemPhasesConfig();
	const FFlecsSystemPhaseConfig& GetSystemPhaseConfig(const TNotNull<TSubclassOf<UFlecsPhase>> PhaseClass) const;

	FOnInitialized& GetOnInitialized() { return OnInitializedEvent; }

#if WITH_EDITOR
	FOnSettingsChange& GetOnSettingsChange() { return OnSettingsChange; }

	bool IsInitialized() const { return bInitialized; }

#endif

protected:
#if WITH_EDITOR
	UE_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	UE_API virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	UE_API virtual void PostInitProperties() override;
	UE_API virtual void BeginDestroy() override;

	UE_API void OnPostEngineInit();
	UE_API void BuildPhases();
	UE_API void BuildSystemList();

private:
	void OnModulePackagesUnloaded(TConstArrayView<UPackage*> Packages);

	FFlecsSystemPhaseConfig& FindOrAddPhaseConfig(const TNotNull<TSubclassOf<UFlecsPhase>>& PhaseClass);

public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Config="Flecs", Config)
	TArray<FFlecsSystemPhaseConfig> SystemPhasesConfig;

	/** This list contains all the systems available in the given binary (including plugins). The contents are sorted by display name.*/
	UPROPERTY(VisibleAnywhere, Category="Flecs", Transient, Instanced, EditFixedSize)
	TArray<TObjectPtr<UFlecsSystem>> SystemCDOs;

protected:
#if WITH_EDITORONLY_DATA
	FOnSettingsChange OnSettingsChange;
#endif // WITH_EDITORONLY_DATA

	bool bInitialized = false;
	bool bEngineInitialized = false;

	FOnInitialized OnInitializedEvent;
};

#undef UE_API
