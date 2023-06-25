#pragma once
#include "entityarchetype.h"
#include "chunk.h"

// Points to a linked list of chunks, each of the same archetype
class ChunkArchetypeElement
{
public:
	ChunkArchetypeElement();
	~ChunkArchetypeElement();

	ChunkArchetypeElement* pNext;
	Chunk* pFirstChunk;

	EntityArchetype archetype;
};