#include <pch.h>

#include <memory/chunkArchetypeElement.h>

ChunkArchetypeElement::ChunkArchetypeElement()
{
	pNext = nullptr;
	pFirstChunk = nullptr;
}

ChunkArchetypeElement::~ChunkArchetypeElement()
{
	if (pFirstChunk == nullptr)
		return;

	Chunk* pChunk = pFirstChunk;

	// Delete all chunks
	Chunk* pNextChunk = pChunk->pNext;
	while (pChunk != nullptr)
	{
		pNextChunk = pChunk->pNext;

#ifdef DEBUG
		std::cout << "	Deleting chunk.		Entities: " << pChunk->numberOfEntities << "\n";
#endif //DEBUG

		free(pChunk);

		pChunk = pNextChunk;
	}
}