#pragma once

#include <chunk.h>
#include <entityArchetype.h>

struct Entity
{
	EntityArchetype* archetype;
	Chunk* chunk;
	unsigned short index;

	Entity(EntityArchetype& archetype, Chunk& chunk, unsigned short index);
	Entity();
};