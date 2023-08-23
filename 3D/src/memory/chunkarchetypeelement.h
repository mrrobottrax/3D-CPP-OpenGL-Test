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

	// Previous and next archetypes in linked list
	// TODO: Use dictionary or something with better access from component
	ChunkArchetypeElement* pPrev;
	ChunkArchetypeElement* pNext;

	Chunk* pFirstChunk;
	gSize_t maxEntities;

	EntityArchetype archetype;
};