#pragma once

#include <memory/chunk.h>
#include <memory/entityArchetype.h>

struct Entity
{
	EntityArchetype* pArchetype;
	Chunk* pChunk;
	unsigned short index;

	Entity(EntityArchetype& archetype, Chunk& chunk, unsigned short index);
	Entity();

	inline bool operator < (const Entity& other) const
	{
		if (pChunk < other.pChunk)
			return true;

		if (pChunk == other.pChunk && index < other.index)
			return true;

		return false;
	}

	inline bool operator == (const Entity& other) const
	{
		if (pChunk == other.pChunk && index == other.index)
			return true;

		return false;
	}
};