#pragma once

struct Component
{
	unsigned short size;
	size_t hash;

	// For debug only
	// TODO: Remove
	char* name;
};