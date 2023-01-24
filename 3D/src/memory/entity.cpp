#include <pch.h>

#include <memory/entity.h>

Entity::Entity(EntityArchetype& archetype, Chunk& chunk, unsigned short index)
{
	Entity::archetype = &archetype;
	Entity::chunk = &chunk;
	Entity::index = index;
}

Entity::Entity()
{
	Entity::archetype = nullptr;
	Entity::chunk = nullptr;
	Entity::index = 0;
}