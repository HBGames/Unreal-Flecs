// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

inline FFlecsEntity FFlecsId::Entity() const
{
	return FFlecsEntity(world_, id_);
}

inline FFlecsEntity FFlecsId::AddFlags(const flecs::id_t InFlags) const
{
	return Id().add_flags(InFlags);
}

inline FFlecsEntity FFlecsId::RemoveFlags(const flecs::id_t InFlags) const
{
	return Id().remove_flags(InFlags);
}

inline FFlecsEntity FFlecsId::RemoveFlags() const
{
	return Id().remove_flags();
}

inline FFlecsEntity FFlecsId::RemoveGeneration() const
{
	return Id().remove_generation();
}

inline FFlecsEntity FFlecsId::TypeId() const
{
	return Id().type_id();
}

inline FFlecsEntity FFlecsId::Flags() const
{
	return Id().flags();
}

inline FFlecsEntity FFlecsId::First() const
{
	return Id().first();
}

inline FFlecsEntity FFlecsId::Second() const
{
	return Id().second();
}
