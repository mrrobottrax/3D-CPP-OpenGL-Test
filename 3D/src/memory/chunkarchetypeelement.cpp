#include "pch.h"
#include "chunkarchetypeelement.h"

#include "entitymanager.h"

ChunkArchetypeElement::~ChunkArchetypeElement()
{
	if (pFirstChunk == nullptr)
		return;

	// Delete all chunks
	while (pFirstChunk)
	{
		entityManager.DeleteChunk(pFirstChunk);
	}
}
