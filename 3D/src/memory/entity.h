#pragma once
#include "chunk.h"
#include "entityarchetype.h"

typedef unsigned short gEntityIndex_t;

struct Entity
{
	gEntityIndex_t index;
	gEntityIndex_t version;

	Entity(const gEntityIndex_t& index, const gEntityIndex_t& version)
		: index(index), version(version)
	{};

	Entity() : index(), version()
	{};

	inline bool constexpr operator < (const Entity& other) const
	{
		if (index < other.index)
			return true;

		if (index == other.index && version < other.version)
			return true;

		return false;
	}

	inline bool constexpr operator == (const Entity& other) const
	{
		if (index == other.index && version == other.version)
			return true;

		return false;
	}
};