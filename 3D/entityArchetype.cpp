#include <entityArchetype.h>

EntityArchetype::EntityArchetype()
{
	EntityArchetype::componentCount = 0;
	EntityArchetype::entitySize = 0;
	EntityArchetype::components = nullptr;
}

EntityArchetype::EntityArchetype(Component* components, unsigned short componentCount)
{
	EntityArchetype::componentCount = componentCount;
	EntityArchetype::components = components;

	// Calculate size
	unsigned short size = 0;
	for (int i = 0; i < componentCount; ++i)
	{
		size += components[i].size;
	}

	EntityArchetype::entitySize = size;
}