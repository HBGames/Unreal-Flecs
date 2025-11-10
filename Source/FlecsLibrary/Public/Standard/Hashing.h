// Solstice Games © 2024. All Rights Reserved.

#pragma once

#define DEFINE_STD_HASH(x) \
	template <> \
	struct std::hash<x> \
	{ \
	public: \
		inline std::size_t operator()(const x& Value) const noexcept \
		{ \
			return GetTypeHash(Value); \
		} \
		\
	}; 