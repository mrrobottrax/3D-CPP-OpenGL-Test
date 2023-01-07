#pragma once

#include <chunk.h>
#include <entityArchetype.h>

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