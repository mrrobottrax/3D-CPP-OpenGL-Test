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
	while (true)
	{
		std::cout << "	Deleting chunk";

		free(chunk);

		if (nextChunk == nullptr)
		{
			break;
		}

		chunk = nextChunk;
		nextChunk = chunk->next;
	}
}