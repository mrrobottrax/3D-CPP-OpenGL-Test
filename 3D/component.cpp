#pragma once
#include <component.h>

Component::Component(std::type_index index, unsigned short size)
{
	Component::hash = index.hash_code();
	Component::size = size;
	Component::offset = 0;

	//TODO: remove
	for (int i = 0; i < 32; i++)
	{
		Component::name[i] = index.name()[i];
		if (Component::name[i] == '\0')
		{
			break;
		}
	}
}