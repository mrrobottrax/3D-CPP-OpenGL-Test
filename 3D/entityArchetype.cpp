#include <entityArchetype.h>

EntityArchetype::EntityArchetype()
{
	EntityArchetype::componentCount = 0;
	EntityArchetype::components = nullptr;
}

EntityArchetype::EntityArchetype(std::type_index* components, unsigned short componentCount)
{
	EntityArchetype::componentCount = componentCount;
	EntityArchetype::components = components;
}