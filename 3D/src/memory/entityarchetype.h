#pragma once
#include "component.h"

class EntityArchetype
{
public:
	EntityArchetype()
	{
		componentCount = 0;
		entitySize = 0;
		components = nullptr;
	};

	EntityArchetype(unsigned short componentCount, Component* componentArray);

	Component* components;
	unsigned short componentCount;
	unsigned short entitySize;

	int GetComponentOffset(Component&);

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