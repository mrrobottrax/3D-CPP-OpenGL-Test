#include <chunkArchetypeElement.h>

ChunkArchetypeElement::ChunkArchetypeElement()
{
	next = nullptr;
	firstChunk = nullptr;
	lastChunk = nullptr;
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

		std::cout << "	Deleting chunk";
		free(chunk);

		chunk = nextChunk;
	}
}