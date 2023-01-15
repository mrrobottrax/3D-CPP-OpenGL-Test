#pragma once

#include <engine/memory/chunk.h>
#include <engine/memory/entityArchetype.h>

struct Entity
{
	EntityArchetype* archetype;
	Chunk* chunk;
	unsigned short index;

	Entity(EntityArchetype& archetype, Chunk& chunk, unsigned short index);
	Entity();
};