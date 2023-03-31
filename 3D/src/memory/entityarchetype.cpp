#include <pch.h>

#include <memory/entityArchetype.h>

EntityArchetype::EntityArchetype(unsigned short componentCount, Component* components)
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

unsigned short EntityArchetype::getComponentOffset(Component& component)
{
	// Scan through component list
	for (int i = 0; i < componentCount; ++i)
	{
		if (components[i] == component)
		{
			return components[i].offset;
		}
	}

	std::cout << "Couldn't find component in archetype for offset!";

	return 0;
}
