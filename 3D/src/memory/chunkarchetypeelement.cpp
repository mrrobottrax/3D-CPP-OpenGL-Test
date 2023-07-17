#include "pch.h"
#include "chunkarchetypeelement.h"

ChunkArchetypeElement::~ChunkArchetypeElement()
{
	if (pFirstChunk == nullptr)
		return;

	// Delete all chunks
	while (pFirstChunk)
	{
		DeleteChunk(pFirstChunk);
	}
}

void ChunkArchetypeElement::DeleteChunk(Chunk* pChunk)
{
#ifdef DEBUG
	std::cout << "	Deleting chunk.		Entities: " << pChunk->numberOfEntities << "\n";
#endif //DEBUG

	if (pChunk->pPrev)
		pChunk->pPrev->pNext = pChunk->pNext;
	else
		pFirstChunk = pChunk->pNext;

	if (pChunk->pNext)
		pChunk->pNext->pPrev = pChunk->pPrev;

	free(pChunk);
}
