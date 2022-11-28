#pragma once

#include <string>
#include <typeindex>

struct Component
{
	Component();

	template <class T>
	Component& init();

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

template <class T>
inline Component& Component::init()
{
	Component::offset = 0;
	Component::hash = typeid(T).hash_code();
	Component::size = sizeof(T);

	const char* name = typeid(T).name();

	//TODO: remove
	for (int i = 0; i < 32; i++)
	{
		Component::name[i] = name[i];
		if (Component::name[i] == '\0')
		{
			break;
		}
	}

	return *this;
}