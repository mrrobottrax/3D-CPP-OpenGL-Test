#pragma once

#include <typeindex>

class EntityArchetype
{
public:
	EntityArchetype();
	EntityArchetype(std::type_index*, unsigned short);

	std::type_index* components;
	unsigned short componentCount;

	friend bool operator==(const EntityArchetype& lhs, const EntityArchetype& rhs)
	{
		if (lhs.componentCount != rhs.componentCount)
			return false;

		return lhs.components == rhs.components;
	}
};