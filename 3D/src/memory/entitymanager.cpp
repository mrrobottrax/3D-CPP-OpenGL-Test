#include <pch.h>

#include <memory/entitymanager.h>
#include <components/idcomponent.h>
#include <components/positioncomponent.h>
#include <components/cameracomponent.h>
#include <components/velocitycomponent.h>
#include <components/meshcomponent.h>

EntityManager::EntityManager()
{
	nextEntityIndex = 0;
	chunkArchetypeList = nullptr;
}

EntityManager::~EntityManager()
{
	
}

Entity EntityManager::AddEntity(EntityArchetype& archetype)
{
	unsigned short index = 0;

	// Check if the chunk archetype exists
	ChunkArchetypeElement* chunkArchetype = FindChunkArchetype(archetype);
	if (chunkArchetype == nullptr)
	{
		chunkArchetype = CreateChunkArchetype(archetype);
	}

	// Check if there are already any chunks
	Chunk* chunk = chunkArchetype->firstChunk;
	if (chunk == nullptr)
	{
		chunk = CreateChunk(*chunkArchetype);
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
			chunk = CreateChunk(*chunkArchetype);
		}
	}

	chunk->numberOfEntities++;

	Entity entity(chunkArchetype->archetype, *chunk, index);

	GetComponent<IdComponent>(entity).index = nextEntityIndex;
	nextEntityIndex++;

	return entity;
}

ChunkArchetypeElement* EntityManager::CreateChunkArchetype(EntityArchetype& archetype)
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

Chunk* EntityManager::CreateChunk(EntityArchetype& archetype)
{
	ChunkArchetypeElement* chunkArchetype = FindChunkArchetype(archetype);

	// Check if this archetype exists
	if (chunkArchetype == nullptr)
	{
		// Add archetype to list
		chunkArchetype = CreateChunkArchetype(archetype);
	}

	return CreateChunk(*chunkArchetype);
}

Chunk* EntityManager::CreateChunk(ChunkArchetypeElement& chunkArchetype)
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

ChunkArchetypeElement* EntityManager::FindChunkArchetype(EntityArchetype& archetype)
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

std::forward_list<ChunkArchetypeElement*>* EntityManager::FindChunkArchetypesWithComponent(Component& component)
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

void EntityManager::DeleteAllEntities()
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
