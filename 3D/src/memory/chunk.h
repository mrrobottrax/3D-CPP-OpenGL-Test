#pragma once

#include <common/types.h>

class ChunkArchetypeElement;

struct Chunk
{
	Chunk* pNext;
	ChunkArchetypeElement* pChunkArchetype;
	gSize_t numberOfEntities;
	gSize_t maxEntities;
};