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
};