#pragma once
#include "chunk.h"
#include "entity.h"

struct EntityPointer
{
	EntityPointer(Chunk* pChunk, const gEntityIndex_t& indexInChunk)
		: pChunk(pChunk), indexInChunk(indexInChunk)
	{};

	EntityPointer() : pChunk(), indexInChunk()
	{};

	Chunk* pChunk;
	gEntityIndex_t indexInChunk;
};