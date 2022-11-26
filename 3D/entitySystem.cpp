#include <entitySystem.h>
#include <idComponent.h>
#include <positionComponent.h>
#include <cameraComponent.h>
#include <iostream>

EntitySystem::EntitySystem()
{
	chunkArchetypeList = nullptr;
	chunkArchetypeListLast = nullptr;

	// Create player
	Component components[] = {
		getComponent(typeid(IdComponent), sizeof(IdComponent)),
		getComponent(typeid(PositionComponent), sizeof(PositionComponent)),
		getComponent(typeid(CameraComponent), sizeof(CameraComponent))
	};

	EntityArchetype player(components, 3);
	addEntity(&player);
}

EntitySystem::~EntitySystem()
{
	// Empty list
	if (chunkArchetypeList == nullptr)
		return;

	// Delete each chunk archetype element
	ChunkArchetypeElement* nextElement = chunkArchetypeList->next;
	while (true)
	{
		std::cout << "Deleting chunk archetype of: ";
		for (int i = 0; i < chunkArchetypeList->archetype.componentCount; i++)
		{
			std::cout << (i != 0 ? ", " : "") << chunkArchetypeList->archetype.components[i].name;
		}
		std::cout << "\n";

		delete chunkArchetypeList;

		if (nextElement == nullptr)
		{
			break;
		}

		chunkArchetypeList = nextElement;
		nextElement = chunkArchetypeList->next;
	}
}

Component EntitySystem::getComponent(std::type_index index, unsigned short size)
{
	Component c;
	c.hash = index.hash_code();
	c.size = size;
	c.name = new char[32];
	
	//TODO: remove
	for (int i = 0; i < 32; i++)
	{
		c.name[i] = index.name()[i];
		if (c.name[i] == '\0')
		{
			break;
		}
	}

	return c;
}

void EntitySystem::addEntity(EntityArchetype* archetype)
{
	// Check if the chunk archetype exists
	ChunkArchetypeElement* chunkArchetype = findChunkArchetype(archetype);
	if (chunkArchetype == nullptr)
	{
		chunkArchetype = createChunkArchetype(archetype);
	}

	// Check if there are already any chunks
	Chunk* chunk = chunkArchetype->firstChunk;
	if (chunk == nullptr)
	{
		chunk = createChunk(chunkArchetype);
	}
	else
	{
		// Get the next free chunk
		bool foundFreeChunk = false;
		while (chunk != nullptr)
		{
			if (chunk->numberOfEntities < chunk->maxEntities)
			{
				foundFreeChunk = true;
				break;
			}

			chunk = chunk->next;
		}

		if (!foundFreeChunk)
		{
			chunk = createChunk(chunkArchetype);
		}
	}

	chunk->numberOfEntities++;
}

ChunkArchetypeElement* EntitySystem::createChunkArchetype(EntityArchetype* archetype)
{
	unsigned short sizeOfChunkArchetype = sizeof(ChunkArchetypeElement);

	// Allocate space for archetype
	unsigned short arraySize = archetype->componentCount * sizeof(Component);

	if (arraySize <= 0)
	{
		std::cout << "Tried to create a chunk archetype with no components!";
		return nullptr;
	}

	unsigned short size = sizeOfChunkArchetype + arraySize;
	ChunkArchetypeElement* element = (ChunkArchetypeElement*)(malloc(size));

	if (element == nullptr)
		return nullptr;

	element->next = nullptr;
	element->firstChunk = nullptr;
	element->lastChunk = nullptr;
	element->archetype = *archetype;

	// Get memory location of the archetype components array
	Component* componentsArray = (Component*)(element + 1);
	memcpy(componentsArray, archetype->components, arraySize);

	element->archetype.components = componentsArray;

	// First element in list
	if (chunkArchetypeList == nullptr)
	{
		chunkArchetypeList = element;
		chunkArchetypeListLast = element;
		return element;
	}

	// Get last element
	chunkArchetypeListLast->next = element;
	chunkArchetypeListLast = element;

	return element;
}

Chunk* EntitySystem::createChunk(EntityArchetype* archetype)
{
	ChunkArchetypeElement* chunkArchetype = findChunkArchetype(archetype);

	// Check if this archetype exists
	if (chunkArchetype == nullptr)
	{
		// Add archetype to list
		chunkArchetype = createChunkArchetype(archetype);
	}

	return createChunk(chunkArchetype);
}

Chunk* EntitySystem::createChunk(ChunkArchetypeElement* chunkArchetype)
{
	Chunk* chunk = (Chunk*)(malloc(chunkSize));

	if (chunk == nullptr)
	{
		std::cout << "Failed to allocate chunk.";
		return nullptr;
	}

	chunk->numberOfEntities = 0;
	chunk->next = nullptr;

	const unsigned short freeSpace = chunkSize - sizeof(*chunk);
	chunk->maxEntities = freeSpace / chunkArchetype->archetype.entitySize;

	// First chunk in archetype
	if (chunkArchetype->firstChunk == nullptr)
	{
		chunkArchetype->firstChunk = chunk;
		chunkArchetype->lastChunk = chunk;
		return chunk;
	}

	chunkArchetype->lastChunk->next = chunk;
	chunkArchetype->lastChunk = chunk;

	return chunk;
}

ChunkArchetypeElement* EntitySystem::findChunkArchetype(EntityArchetype* archetype)
{
	// Return null if list is empty
	if (chunkArchetypeList == nullptr)
	{
		return nullptr;
	}

	// Scan through entire linked list
	bool foundArchetype = false;
	ChunkArchetypeElement* chunkArchetype = chunkArchetypeList;
	while (chunkArchetype != nullptr)
	{
		if (chunkArchetype->archetype == *archetype)
		{
			foundArchetype = true;
			break;
		}
		chunkArchetype = chunkArchetype->next;
	}

	if (foundArchetype)
	{
		return chunkArchetype;
	}

	return nullptr;
}

void EntitySystem::update()
{
}