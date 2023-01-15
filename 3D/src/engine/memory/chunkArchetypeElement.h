#pragma once

#include <engine/memory/chunk.h>
#include <engine/memory/entityArchetype.h>

// Points to a linked list of chunks, each of the same archetype
class ChunkArchetypeElement
{
public:
	ChunkArchetypeElement();
	~ChunkArchetypeElement();

	ChunkArchetypeElement* next;
	Chunk* firstChunk;

	EntityArchetype archetype;
};