#pragma once

#include <typeindex>
#include <iostream>
#include <engine/memory/component.h>

class EntityArchetype
{
public:
	EntityArchetype();
	EntityArchetype(unsigned short, Component*);

	Component* components;
	unsigned short componentCount;
	unsigned short entitySize;

	unsigned short getComponentOffset(Component&);

	bool operator==(const EntityArchetype& other)
	{
		if (componentCount != other.componentCount)
			return false;

		// Go through each component and make sure they're the same
		// TODO: Use hashes?
		for (int i = 0; i < componentCount; i++)
		{
			if (components[i].hash != other.components[i].hash)
			{
				return false;
			}
		}

		return true;
	}
};