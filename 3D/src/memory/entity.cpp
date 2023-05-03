#include <pch.h>

#include <memory/entity.h>

Entity::Entity(EntityArchetype& archetype, Chunk& chunk, unsigned short index)
{
	Entity::pArchetype = &archetype;
	Entity::pChunk = &chunk;
	Entity::index = index;
}

Entity::Entity()
{
	Entity::pArchetype = nullptr;
	Entity::pChunk = nullptr;
	Entity::index = 0;
}