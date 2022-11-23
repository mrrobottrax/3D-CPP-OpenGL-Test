#pragma once

#include <component.h>

struct Entity
{
	unsigned short componentCount;

	Component* components;
};