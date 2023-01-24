#pragma once

struct Chunk
{
	Chunk* next;
	unsigned int numberOfEntities;
	unsigned int maxEntities;
};