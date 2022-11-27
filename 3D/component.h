#pragma once

#include <string>
#include <typeindex>

struct Component
{
	Component(std::type_index, unsigned short);

	unsigned short size;
	unsigned short offset;
	size_t hash;

	// For debug only
	// TODO: Remove
	char name[32];

	bool operator==(const Component& other)
	{
		if (hash != other.hash)
			return false;

		return true;
	}
};