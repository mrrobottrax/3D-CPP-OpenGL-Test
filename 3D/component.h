#pragma once

struct Component
{
	unsigned short size;
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