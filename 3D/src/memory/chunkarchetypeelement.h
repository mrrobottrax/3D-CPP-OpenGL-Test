#pragma once
#include "entityarchetype.h"
#include "chunk.h"

// Points to a linked list of chunks, each of the same archetype
class ChunkArchetypeElement
{
public:
	ChunkArchetypeElement() : pPrev(), pNext(), pFirstChunk(), maxEntities()
	{};

	~ChunkArchetypeElement();

	ChunkArchetypeElement* pPrev;
	ChunkArchetypeElement* pNext;
	Chunk* pFirstChunk;
	gSize_t maxEntities;

	EntityArchetype archetype;

	void DeleteChunk(Chunk* pChunk);
};