// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsSubsystemBase.h"

#include "FlecsEntitySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSubsystemBase)

namespace UE::Flecs
{
	namespace Subsystems
	{
		void RegisterSubsystemType(FSubsystemCollectionBase& InCollection, TSubclassOf<USubsystem> InSubsystemClass, FSubsystemTypeTraits&& InTraits)
		{
			if (UFlecsEntitySubsystem* EntitySubsystem = InCollection.InitializeDependency<UFlecsEntitySubsystem>())
			{
				RegisterSubsystemType(EntitySubsystem->GetFlecsWorld(), InSubsystemClass, MoveTemp(InTraits));
			}
		}

		void RegisterSubsystemType(const FFlecsWorld& InFlecsWorld, TSubclassOf<USubsystem> InSubsystemClass, FSubsystemTypeTraits&& InTraits)
		{
			
		}
	}

	namespace Private
	{
		/** 
		 * A helper function calling PostInitialize and OnWorldBeginPlay for the given subsystem, provided the world has already begun play.
		 * @see UFlecsSubsystemBase::HandleLateCreation for more detail
		 */
		void HandleLateCreation(UWorldSubsystem& FlecsWorldSubsystem, const UE::Flecs::Subsystems::FInitializationState InitializationState)
		{
			// handle late creation
			if (UWorld* World = FlecsWorldSubsystem.GetWorld())
			{
				if (World->IsInitialized() == true && InitializationState.bPostInitializeCalled == false)
				{
					FlecsWorldSubsystem.PostInitialize();
				}
				if (World->HasBegunPlay() == true && InitializationState.bOnWorldBeginPlayCalled == false)
				{
					FlecsWorldSubsystem.OnWorldBeginPlay(*World);
				}
			}
		}

		bool bRuntimeSubsystemsEnabled = true;

		namespace
		{
			FAutoConsoleVariableRef AnonymousCVars[] =
			{
				{TEXT("flecs.RuntimeSubsystemsEnabled"), bRuntimeSubsystemsEnabled, TEXT("true by default, setting to false will prevent auto-creation of game-time Flecs-related subsystems. Needs to be set before world loading."), ECVF_Default}
			};
		}
	}
}

bool UFlecsSubsystemBase::AreRuntimeFlecsSubsystemsAllowed(UObject* Outer)
{
	return UE::Flecs::Private::bRuntimeSubsystemsEnabled;
}

bool UFlecsSubsystemBase::ShouldCreateSubsystem(UObject* Outer) const
{
	return ThisClass::AreRuntimeFlecsSubsystemsAllowed(Outer) && Super::ShouldCreateSubsystem(Outer);
}

void UFlecsSubsystemBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bInitializeCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bInitializeCalled = true;

	// register the given child class with default traits. Child-class can always override the traits data registered here.
	// Note that we're not performing the registration for UFlecsEntitySubsystem since that's the subsystem
	// we use to get access to the EntityManager instance in the first place. UFlecsEntitySubsystem has to perform the registration manually
	if (GetClass()->IsChildOf(UFlecsEntitySubsystem::StaticClass()) == false)
	{
		// register the given child class with default traits. Child-class can always override the traits data registered here.
		UE::Flecs::Subsystems::RegisterSubsystemType(Collection, GetClass(), UE::Flecs::FSubsystemTypeTraits::Make<UFlecsSubsystemBase>());
	}
}

void UFlecsSubsystemBase::PostInitialize()
{
	Super::PostInitialize();

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bPostInitializeCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bPostInitializeCalled = true;
}

void UFlecsSubsystemBase::Deinitialize()
{
	InitializationState = UE::Flecs::Subsystems::FInitializationState();

	Super::Deinitialize();
}

void UFlecsSubsystemBase::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bOnWorldBeginPlayCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bOnWorldBeginPlayCalled = true;
}

void UFlecsSubsystemBase::HandleLateCreation()
{
	UE::Flecs::Private::HandleLateCreation(*this, InitializationState);
}

bool UFlecsTickableSubsystemBase::ShouldCreateSubsystem(UObject* Outer) const
{
	return UFlecsSubsystemBase::AreRuntimeFlecsSubsystemsAllowed(Outer) && Super::ShouldCreateSubsystem(Outer);
}

void UFlecsTickableSubsystemBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bInitializeCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bInitializeCalled = true;

	// register the given child class with default traits. Child-class can always override the traits data registered here.
	UE::Flecs::Subsystems::RegisterSubsystemType(Collection, GetClass(), UE::Flecs::FSubsystemTypeTraits::Make<UFlecsTickableSubsystemBase>());
}

void UFlecsTickableSubsystemBase::PostInitialize()
{
	Super::PostInitialize();

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bPostInitializeCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bPostInitializeCalled = true;
}

void UFlecsTickableSubsystemBase::Deinitialize()
{
	InitializationState = UE::Flecs::Subsystems::FInitializationState();

	Super::Deinitialize();
}

void UFlecsTickableSubsystemBase::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// This ensure is here to make sure we handle HandleLateCreation() gracefully, we don't expect it to ever trigger unless users start to manually call the functions
	ensureMsgf(InitializationState.bOnWorldBeginPlayCalled == false, TEXT("%hs called multiple times"), __FUNCTION__);
	InitializationState.bOnWorldBeginPlayCalled = true;
}

void UFlecsTickableSubsystemBase::HandleLateCreation()
{
	UE::Flecs::Private::HandleLateCreation(*this, InitializationState);
}
