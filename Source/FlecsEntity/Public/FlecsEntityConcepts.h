// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityElementTypes.h"
#include "Components/Traits/FlecsComponentTraits.h"
#include "Subsystems/Subsystem.h"

namespace UE::Flecs
{
	template <typename T>
	using Clean = typename TRemoveReference<T>::Type;

	template <typename T>
	concept CComponent = TIsDerivedFrom<Clean<T>, FFlecsComponent>::Value &&
	(
		std::is_trivially_copyable_v<Clean<T>> ||
		static_cast<bool>(TFlecsComponentTraits<Clean<T>>::AuthorAcceptsItsNotTriviallyCopyable)
	);

	template <typename T>
	concept CTag = TIsDerivedFrom<Clean<T>, FFlecsTag>::Value;

	template <typename T>
	concept CNonTag = CComponent<T>;

	template <typename T>
	concept CElement = CNonTag<T> || CTag<T>;

	template <typename T>
	concept CSubsystem = TIsDerivedFrom<Clean<T>, USubsystem>::Value;

	namespace Private
	{
		template <CElement T>
		struct TElementTypeHelper
		{
			using Type = std::conditional_t<CComponent<T>, FFlecsComponent, std::conditional_t<CTag<T>, FFlecsTag, void>>;
		};
	}

	template <typename T>
	using TElementType = typename Private::TElementTypeHelper<T>::Type;
}
