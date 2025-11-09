// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "Settings/FlecsEntitySettings.h"

#include "Algo/RemoveIf.h"
#include "Misc/StringOutputDevice.h"
#include "Phases/FlecsPhase.h"
#include "Systems/FlecsSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntitySettings)

//----------------------------------------------------------------------//
//  UFlecsEntitySettings
//----------------------------------------------------------------------//
UFlecsEntitySettings::UFlecsEntitySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCoreDelegates::OnPostEngineInit.AddUObject(this, &UFlecsEntitySettings::OnPostEngineInit);

	// we need to get notified about modules being unloaded (like Game Feature Plugins) so that we can remove
	// stored CDOs originating from the modules being removed.
	FCoreUObjectDelegates::CompiledInUObjectsRemovedDelegate.AddUObject(this, &UFlecsEntitySettings::OnModulePackagesUnloaded);
}

void UFlecsEntitySettings::Initialize()
{
	if (bInitialized || !bEngineInitialized) return;

	BuildSystemList();
	BuildPhases();

	bInitialized = true;

	OnInitializedEvent.Broadcast();
}

TConstArrayView<FFlecsSystemPhaseConfig> UFlecsEntitySettings::GetSystemPhasesConfig()
{
	BuildSystemList();
	return MakeArrayView(SystemPhasesConfig);
}

const FFlecsSystemPhaseConfig& UFlecsEntitySettings::GetSystemPhaseConfig(const TNotNull<TSubclassOf<UFlecsPhase>> PhaseClass) const
{
	const FFlecsSystemPhaseConfig* FoundPhaseConfig = SystemPhasesConfig.FindByPredicate([PhaseClass](const FFlecsSystemPhaseConfig& A) { return A.PhaseClass == PhaseClass; });
	checkf(FoundPhaseConfig, TEXT("Requested PhaseClass %s not found in FlecsEntitySettings"), *PhaseClass->GetName());
	return *FoundPhaseConfig;
}

#if WITH_EDITOR
void UFlecsEntitySettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName SystemCDOsName = GET_MEMBER_NAME_CHECKED(ThisClass, SystemCDOs);

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
	{
		// ignore adding elements to arrays since it would be 'None' at first
		return;
	}

	if (PropertyChangedEvent.Property)
	{
		const FName PropName = PropertyChangedEvent.Property->GetFName();
		if (PropName == SystemCDOsName)
		{
			BuildSystemList();
		}

		BuildPhases();
		OnSettingsChange.Broadcast(PropertyChangedEvent);
	}
}

void UFlecsEntitySettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static const FName AutoRegisterName = "bAutoRegisterWithSystemPhases";

	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FProperty* Property = PropertyChangedEvent.Property;
	FProperty* MemberProperty = nullptr;
	FEditPropertyChain::TDoubleLinkedListNode* LastPropertyNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	while (LastPropertyNode && LastPropertyNode->GetNextNode())
	{
		LastPropertyNode = LastPropertyNode->GetNextNode();
	}

	if (LastPropertyNode)
	{
		MemberProperty = LastPropertyNode->GetValue();
	}

	if (MemberProperty && MemberProperty->GetFName() == AutoRegisterName)
	{
		BuildSystemList();
	}
}
#endif

void UFlecsEntitySettings::PostInitProperties()
{
	Super::PostInitProperties();
}

void UFlecsEntitySettings::BeginDestroy()
{
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	FCoreUObjectDelegates::CompiledInUObjectsRemovedDelegate.RemoveAll(this);
	Super::BeginDestroy();
}

void UFlecsEntitySettings::OnPostEngineInit()
{
	bEngineInitialized = true;
	Initialize();
}

void UFlecsEntitySettings::BuildPhases()
{
	TArray<UClass*> SubClassess;
	GetDerivedClasses(UFlecsPhase::StaticClass(), SubClassess);

	for (int i = SubClassess.Num() - 1; i >= 0; --i)
	{
		UClass* PhaseClass = SubClassess[i];
		if (!PhaseClass || PhaseClass->HasAnyClassFlags(CLASS_Abstract)) continue;

		UFlecsPhase* PhaseCDO = GetMutableDefault<UFlecsPhase>(PhaseClass);

		FFlecsSystemPhaseConfig& PhaseConfig = FindOrAddPhaseConfig(PhaseClass);
		PhaseConfig.PhaseClass = PhaseClass;
#if WITH_EDITOR
		PhaseConfig.PhaseCDO = PhaseCDO;
#endif

#if WITH_EDITOR
		FStringOutputDevice Ar;
		PhaseConfig.PhaseCDO->DebugOutputDescription(Ar);
		PhaseConfig.Description = FText::FromString(Ar);
#endif
	}
}

void UFlecsEntitySettings::BuildSystemList()
{
	SystemCDOs.Reset();

	TArray<UClass*> SubClassess;
	GetDerivedClasses(UFlecsSystem::StaticClass(), SubClassess);

	for (int i = SubClassess.Num() - 1; i >= 0; --i)
	{
		if (SubClassess[i]->HasAnyClassFlags(CLASS_Abstract)) continue;

		UFlecsSystem* SystemCDO = GetMutableDefault<UFlecsSystem>(SubClassess[i]);
		if (SystemCDO && SubClassess[i]
#if WITH_EDITOR
			&& SystemCDO->ShouldShowUpInSettings()
#endif
		)
		{
			// Observers might register later than the GC disregard window, causing a GC mismatch between this early-initialized class and them.
			const bool bIsDisregardForGC = GUObjectArray.IsDisregardForGC(this);
			if (bIsDisregardForGC && !(GUObjectArray.IsDisregardForGC(SystemCDO) || SystemCDO->HasAnyFlags(RF_MarkAsRootSet)))
			{
				SystemCDO->AddToRoot();
			}

			SystemCDOs.Add(SystemCDO);
			if (SystemCDO->ShouldAutoAddToGlobalList())
			{
				FindOrAddPhaseConfig(SystemCDO->GetExecuteInPhase()).SystemCDOs.Add(SystemCDO);
			}
		}
	}

	SystemCDOs.Sort([](UFlecsSystem& LHS, UFlecsSystem& RHS)
	{
		return LHS.GetName().Compare(RHS.GetName()) < 0;
	});
}

void UFlecsEntitySettings::OnModulePackagesUnloaded(TConstArrayView<UPackage*> Packages)
{
	const int32 InitialNum = SystemCDOs.Num();

	SystemCDOs.SetNum(Algo::RemoveIf(SystemCDOs, [&Packages](const TObjectPtr<UFlecsSystem>& CDO)
	{
		return !CDO || Packages.Contains(CDO->GetPackage());
	}));

	if (SystemCDOs.Num() != InitialNum)
	{
		// Rebuild the phase configs
		for (FFlecsSystemPhaseConfig& PhaseConfig : SystemPhasesConfig)
		{
			PhaseConfig.SystemCDOs.Reset();
		}

		for (UFlecsSystem* SystemCDO : SystemCDOs)
		{
			check(SystemCDO);
			if (SystemCDO->ShouldAutoAddToGlobalList())
			{
				FindOrAddPhaseConfig(SystemCDO->GetExecuteInPhase()).SystemCDOs.Add(SystemCDO);
			}
		}
	}
}

FFlecsSystemPhaseConfig& UFlecsEntitySettings::FindOrAddPhaseConfig(const TNotNull<TSubclassOf<UFlecsPhase>>& PhaseClass)
{
	FFlecsSystemPhaseConfig* FoundSystemPhase = SystemPhasesConfig.FindByPredicate([PhaseClass](const FFlecsSystemPhaseConfig& A) { return A.PhaseClass == PhaseClass; });
	FFlecsSystemPhaseConfig& ReturnValue = FoundSystemPhase ? *FoundSystemPhase : SystemPhasesConfig.AddDefaulted_GetRef();
	ReturnValue.PhaseClass = PhaseClass;
	return ReturnValue;
}
