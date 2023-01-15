#include <engine/memory/entityManager.h>
#include <engine/idComponent.h>
#include <engine/positionComponent.h>
#include <engine/cameraComponent.h>
#include <iostream>
#include <engine/velocityComponent.h>
#include <forward_list>
#include <engine/meshComponent.h>

EntityManager* EntityManager::instance{ nullptr };
std::mutex EntityManager::mutex;

EntityManager& EntityManager::GetInstance()
{
	std::lock_guard<std::mutex> lock(mutex);
	if (instance == nullptr)
	{
		instance = new EntityManager();
	}

	return *instance;
};

EntityManager::EntityManager()
{
	nextEntityIndex = 0;
	chunkArchetypeList = nullptr;
}

EntityManager::~EntityManager()
{
	// Empty list
	if (chunkArchetypeList == nullptr)
		return;

	// Delete each chunk archetype element
	ChunkArchetypeElement* elementToDelete;
	while (chunkArchetypeList != nullptr)
	{
		std::cout << "Deleting chunk archetype of: ";
		for (int i = 0; i < chunkArchetypeList->archetype.componentCount; i++)
		{
			std::cout << (i != 0 ? ", " : "") << chunkArchetypeList->archetype.components[i].name;
		}
		std::cout << "\n";

		elementToDelete = chunkArchetypeList;
		chunkArchetypeList = chunkArchetypeList->next;

		delete elementToDelete;
	}
}

Entity EntityManager::addEntity(EntityArchetype& archetype)
{
	unsigned short index = 0;

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
		chunk = createChunk(*chunkArchetype);
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
				index = chunk->numberOfEntities;
				break;
			}

			chunk = chunk->next;
		}

		if (!foundFreeChunk)
		{
			chunk = createChunk(*chunkArchetype);
		}
	}

	chunk->numberOfEntities++;

	Entity entity(chunkArchetype->archetype, *chunk, index);

	getComponent<IdComponent>(entity).index = nextEntityIndex;
	nextEntityIndex++;

	return entity;
}

ChunkArchetypeElement* EntityManager::createChunkArchetype(EntityArchetype& archetype)
{
	unsigned short sizeOfChunkArchetype = sizeof(ChunkArchetypeElement);

	// Allocate space for archetype
	unsigned short arraySize = archetype.componentCount * sizeof(Component);

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
	element->archetype = archetype;

	// Get memory location of the archetype components array
	Component* componentsArray = (Component*)(element + 1);
	memcpy(componentsArray, archetype.components, arraySize);

	element->archetype.components = componentsArray;

	// First element in list
	if (chunkArchetypeList == nullptr)
	{
		chunkArchetypeList = element;
		return element;
	}

	// Get last element
	element->next = chunkArchetypeList;
	chunkArchetypeList = element;

	return element;
}

Chunk* EntityManager::createChunk(EntityArchetype& archetype)
{
	ChunkArchetypeElement* chunkArchetype = findChunkArchetype(archetype);

	// Check if this archetype exists
	if (chunkArchetype == nullptr)
	{
		// Add archetype to list
		chunkArchetype = createChunkArchetype(archetype);
	}

	return createChunk(*chunkArchetype);
}

Chunk* EntityManager::createChunk(ChunkArchetypeElement& chunkArchetype)
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
	chunk->maxEntities = freeSpace / chunkArchetype.archetype.entitySize;

	// First chunk in archetype
	if (chunkArchetype.firstChunk == nullptr)
	{
		chunkArchetype.firstChunk = chunk;
		return chunk;
	}

	chunk->next = chunkArchetype.firstChunk;
	chunkArchetype.firstChunk = chunk;

	return chunk;
}

ChunkArchetypeElement* EntityManager::findChunkArchetype(EntityArchetype& archetype)
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
		if (chunkArchetype->archetype == archetype)
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

std::forward_list<ChunkArchetypeElement*>* EntityManager::findChunkArchetypesWithComponent(Component& component)
{
	// Create linked list of pointers
	std::forward_list<ChunkArchetypeElement*>* returnList = new std::forward_list<ChunkArchetypeElement*>();

	// Scan through entire linked list
	bool foundArchetype = false;
	ChunkArchetypeElement* chunkArchetype = chunkArchetypeList;
	while (chunkArchetype != nullptr)
	{
		// Check if this archetype contains this component
		for (int i = 0; i < chunkArchetype->archetype.componentCount; i++)
		{
			if (chunkArchetype->archetype.components[i] == component)
			{
				foundArchetype = true;
				returnList->emplace_front(chunkArchetype);
				break;
			}
		}

		chunkArchetype = chunkArchetype->next;
	}

	if (foundArchetype)
	{
		return returnList;
	}

	delete returnList;
	return nullptr;
}

void EntityManager::update()
{
}