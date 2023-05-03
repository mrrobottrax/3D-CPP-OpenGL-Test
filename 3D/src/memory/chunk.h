#pragma once

struct Chunk
{
	Chunk* pNext;
	unsigned int numberOfEntities;
	unsigned int maxEntities;
};