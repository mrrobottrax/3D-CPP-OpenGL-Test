#pragma once

#include <typeindex>
#include <iostream>

class EntityArchetype
{
public:
	EntityArchetype();
	EntityArchetype(std::type_index*, unsigned short);

	std::type_index* components;
	unsigned short componentCount;

	bool operator==(const EntityArchetype& other)
	{
		if (componentCount != other.componentCount)
			return false;

		// Go through each component and make sure they're the same
		// TODO: Use hashes
		for (int i = 0; i < componentCount; i++)
		{
			if (components[i] != other.components[i])
			{
				return false;
			}
		}

		return true;
	}
};