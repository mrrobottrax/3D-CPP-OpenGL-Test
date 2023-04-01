#include <pch.h>

#include <memory/chunkArchetypeElement.h>

ChunkArchetypeElement::ChunkArchetypeElement()
{
	next = nullptr;
	firstChunk = nullptr;
}

ChunkArchetypeElement::~ChunkArchetypeElement()
{
	if (firstChunk == nullptr)
		return;

	Chunk* chunk = firstChunk;

	// Delete all chunks
	Chunk* nextChunk = chunk->next;
	while (chunk != nullptr)
	{
		nextChunk = chunk->next;

#ifdef DEBUG
		std::cout << "	Deleting chunk.		Entities: " << chunk->numberOfEntities << "\n";
#endif //DEBUG

		free(chunk);

		chunk = nextChunk;
	}
}