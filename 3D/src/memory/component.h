#pragma once

#include <common/types.h>

#ifdef DEBUG
//#define COMPONENT_DEBUG
#endif

struct Component
{
	template <class T>
	Component& Init();

	gSize_t size;
	gSize_t offset;
	size_t hash;

#ifdef COMPONENT_DEBUG
	// For debug only
	// TODO: Remove
	char name[32];
#endif // COMPONENT_DEBUG

	bool operator==(const Component& other)
	{
		if (hash != other.hash)
			return false;

		return true;
	}
};

template <class T>
inline Component& Component::Init()
{
	Component::offset = 0;
	Component::hash = typeid(T).hash_code();
	Component::size = sizeof(T);

#ifdef COMPONENT_DEBUG
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
#endif // COMPONENT_DEBUG

	return *this;
}