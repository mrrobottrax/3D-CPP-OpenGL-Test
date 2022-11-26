#include <entityArchetype.h>

EntityArchetype::EntityArchetype()
{
	EntityArchetype::componentCount = 0;
	EntityArchetype::components = nullptr;
}

EntityArchetype::EntityArchetype(Component* components, unsigned short componentCount)
{
	EntityArchetype::componentCount = componentCount;
	EntityArchetype::components = components;
}