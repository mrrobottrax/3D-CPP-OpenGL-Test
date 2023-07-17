#include "pch.h"
#include "entityArchetype.h"

EntityArchetype::EntityArchetype(gSize_t componentCount, Component* components)
{
	this->componentCount = componentCount;
	this->components = components;

	// Calculate size
	gSize_t offset = 0;
	for (gSize_t i = 0; i < componentCount; ++i)
	{
		components[i].offset = offset;
		offset += components[i].size;
	}

	this->entitySize = offset;
}

int EntityArchetype::GetComponentOffset(Component& component)
{
	// Scan through component list
	for (gSize_t i = 0; i < componentCount; ++i)
	{
		if (components[i] == component)
		{
			return components[i].offset;
		}
	}

	return -1;
}
