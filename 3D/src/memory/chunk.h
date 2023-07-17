#pragma once

#include <common/types.h>

class ChunkArchetypeElement;

struct Chunk
{
	Chunk* pPrev;
	Chunk* pNext;
	ChunkArchetypeElement* pChunkArchetype;
	gSize_t numberOfEntities;
};