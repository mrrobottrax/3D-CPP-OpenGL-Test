#pragma once
#include "component.h"
#include <common/types.h>

class EntityArchetype
{
public:
	EntityArchetype() : componentCount(), entitySize(), components()
	{};

	EntityArchetype(gSize_t componentCount, Component* componentArray);

	Component* components;
	gSize_t componentCount;
	gSize_t entitySize;

	int GetComponentOffset(Component&);

	bool operator==(const EntityArchetype& other)
	{
		if (componentCount != other.componentCount)
			return false;

		if (entitySize != other.entitySize)
			return false;

		// Go through each component and make sure they're the same
		// TODO: Use hashes?
		for (gSize_t i = 0; i < componentCount; i++)
		{
			if (components[i].hash != other.components[i].hash)
			{
				return false;
			}
		}

		return true;
	}
};