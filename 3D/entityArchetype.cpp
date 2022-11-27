#include <entityArchetype.h>
#include <iostream>

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
	unsigned short offset = 0;
	for (int i = 0; i < componentCount; ++i)
	{
		components[i].offset = size;
		size += components[i].size;
	}

	EntityArchetype::entitySize = size;
}

unsigned short EntityArchetype::getComponentOffset(Component* component)
{
	// Scan through component list
	for (int i = 0; i < componentCount; ++i)
	{
		if (components[i] == *component)
		{
			return component->offset;
		}
	}

	std::cout << "Couldn't find component in archetype for offset!";

	return 0;
}
