#include "pch.h"
#include "entitymanager.h"

#include <components/idcomponent.h>

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
	ChunkArchetypeElement* pChunkArchetype = FindChunkArchetype(archetype);
	if (pChunkArchetype == nullptr)
	{
		pChunkArchetype = CreateChunkArchetype(archetype);
	}

	// Check if there are already any chunks
	Chunk* pChunk = pChunkArchetype->pFirstChunk;
	if (pChunk == nullptr)
	{
		pChunk = CreateChunk(*pChunkArchetype);
	}
	else
	{
		// Get the next free chunk
		bool foundFreeChunk = false;
		while (pChunk != nullptr)
		{
			if (pChunk->numberOfEntities < pChunk->maxEntities)
			{
				foundFreeChunk = true;
				index = pChunk->numberOfEntities;
				break;
			}

			pChunk = pChunk->pNext;
		}

		if (!foundFreeChunk)
		{
			pChunk = CreateChunk(*pChunkArchetype);
		}
	}

	pChunk->numberOfEntities++;

	Entity entity(pChunkArchetype->archetype, *pChunk, index);

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
	ChunkArchetypeElement* pElement = (ChunkArchetypeElement*)(malloc(size));

	if (pElement == nullptr)
		return nullptr;

	pElement->pNext = nullptr;
	pElement->pFirstChunk = nullptr;
	pElement->archetype = archetype;

	// Get memory location of the archetype components array
	Component* componentsArray = (Component*)(pElement + 1);
	memcpy(componentsArray, archetype.components, arraySize);

	pElement->archetype.components = componentsArray;

	// First element in list
	if (chunkArchetypeList == nullptr)
	{
		chunkArchetypeList = pElement;
		return pElement;
	}

	// Get last element
	pElement->pNext = chunkArchetypeList;
	chunkArchetypeList = pElement;

	return pElement;
}

Chunk* EntityManager::CreateChunk(EntityArchetype& archetype)
{
	ChunkArchetypeElement* pChunkArchetype = FindChunkArchetype(archetype);

	// Check if this archetype exists
	if (pChunkArchetype == nullptr)
	{
		// Add archetype to list
		pChunkArchetype = CreateChunkArchetype(archetype);
	}

	return CreateChunk(*pChunkArchetype);
}

Chunk* EntityManager::CreateChunk(ChunkArchetypeElement& chunkArchetype)
{
	Chunk* pChunk = (Chunk*)(malloc(chunkSize));

	if (pChunk == nullptr)
	{
		std::cout << "Failed to allocate chunk.";
		return nullptr;
	}

	pChunk->numberOfEntities = 0;
	pChunk->pNext = nullptr;

	const unsigned short freeSpace = chunkSize - sizeof(*pChunk);
	pChunk->maxEntities = freeSpace / chunkArchetype.archetype.entitySize;

	// First chunk in archetype
	if (chunkArchetype.pFirstChunk == nullptr)
	{
		chunkArchetype.pFirstChunk = pChunk;
		return pChunk;
	}

	pChunk->pNext = chunkArchetype.pFirstChunk;
	chunkArchetype.pFirstChunk = pChunk;

	return pChunk;
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
	ChunkArchetypeElement* pChunkArchetype = chunkArchetypeList;
	while (pChunkArchetype != nullptr)
	{
		if (pChunkArchetype->archetype == archetype)
		{
			foundArchetype = true;
			break;
		}
		pChunkArchetype = pChunkArchetype->pNext;
	}

	if (foundArchetype)
	{
		return pChunkArchetype;
	}

	return nullptr;
}

std::vector<ChunkArchetypeElement*> EntityManager::FindChunkArchetypesWithComponent(Component& component)
{
	// Create vector of pointers
	std::vector<ChunkArchetypeElement*> returnList = std::vector<ChunkArchetypeElement*>();

	// Scan through entire linked list
	bool foundArchetype = false;
	ChunkArchetypeElement* pChunkArchetype = chunkArchetypeList;
	while (pChunkArchetype != nullptr)
	{
		// Check if this archetype contains this component
		for (int i = 0; i < pChunkArchetype->archetype.componentCount; i++)
		{
			if (pChunkArchetype->archetype.components[i] == component)
			{
				foundArchetype = true;
				returnList.push_back(pChunkArchetype);
				break;
			}
		}

		pChunkArchetype = pChunkArchetype->pNext;
	}

	return returnList;
}

void EntityManager::DeleteAllEntities()
{
	// Empty list
	if (chunkArchetypeList == nullptr)
		return;

	// Delete each chunk archetype element
	ChunkArchetypeElement* pElementToDelete;
	while (chunkArchetypeList != nullptr)
	{
#ifdef DEBUG
		std::cout << "Deleting chunk archetype of: ";
		for (int i = 0; i < chunkArchetypeList->archetype.componentCount; i++)
		{
			std::cout << (i != 0 ? ", " : "") << chunkArchetypeList->archetype.components[i].name;
		}
		std::cout << "\n";
#endif // DEBUG

		pElementToDelete = chunkArchetypeList;
		chunkArchetypeList = chunkArchetypeList->pNext;

		delete pElementToDelete;
	}
}
