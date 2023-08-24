#include "pch.h"
#include "entitymanager.h"

#include <components/idcomponent.h>

const Entity EntityManager::AddEntity(EntityArchetype& archetype)
{
	gChunkIndex_t indexInChunk = 0;

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
			if (pChunk->numberOfEntities < pChunk->pChunkArchetype->maxEntities)
			{
				foundFreeChunk = true;
				indexInChunk = pChunk->numberOfEntities;
				break;
			}

			pChunk = pChunk->pNext;
		}

		if (!foundFreeChunk)
		{
			pChunk = CreateChunk(*pChunkArchetype);
		}
	}

	// Get next free index
	gEntityIndex_t index;
	const gEntityIndex_t numIndices = static_cast<gEntityIndex_t>(entityIndices.size());
	if (numberOfEntities >= numIndices)
	{
		entityIndices.push_back(numIndices);
		index = numIndices;
	}
	else
	{
		index = entityIndices[numberOfEntities];
	}

	// Add pointer to pointer array
	if (index >= entityTable.size())
	{
		entityTable.push_back(TableEntry(EntityPointer(pChunk, indexInChunk), numberOfEntities, 0));
	}
	else
	{
		entityTable[index] = TableEntry(EntityPointer(pChunk, indexInChunk), numberOfEntities, entityTable[index].version + 1);
	}

	EntityPointer p(pChunk, indexInChunk);

	// Set id
	IdComponent& id = entityManager.GetComponent<IdComponent>(p);
	id.index = index;
	id.version = entityTable[index].version;

	++pChunk->numberOfEntities;
	++numberOfEntities;

	return Entity(index, id.version);
}

ChunkArchetypeElement* EntityManager::CreateChunkArchetype(EntityArchetype& archetype)
{
	const gSize_t sizeOfChunkArchetype = sizeof(ChunkArchetypeElement);

	// Allocate space for archetype
	const gSize_t arraySize = archetype.componentCount * sizeof(Component);

	if (arraySize <= 0)
	{
		std::cout << "Tried to create a chunk archetype with no components!";
		return nullptr;
	}

	const gSize_t size = sizeOfChunkArchetype + arraySize;
	ChunkArchetypeElement* pElement = (ChunkArchetypeElement*)(malloc(size));

	if (pElement == nullptr)
		return nullptr;

	pElement->pFirstChunk = nullptr;
	pElement->archetype = archetype;

	constexpr gSize_t freeSpace = chunkSize - sizeof(Chunk);
	pElement->maxEntities = freeSpace / archetype.entitySize;

	// Get memory location of the archetype components array
	Component* componentsArray = (Component*)(pElement + 1);
	memcpy(componentsArray, archetype.components, arraySize);

	pElement->archetype.components = componentsArray;

	// Add to front
	pElement->pPrev = nullptr;
	pElement->pNext = chunkArchetypeList;
	if (pElement->pNext)
		pElement->pNext->pPrev = pElement;

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

	assert(("Failed to allocate chunk.", pChunk != nullptr));

	if (pChunk == nullptr)
		return nullptr;

	//memset(pChunk, 0, chunkSize);

	pChunk->numberOfEntities = 0;
	pChunk->pChunkArchetype = &chunkArchetype;

	// Add to front of list
	pChunk->pPrev = nullptr;
	pChunk->pNext = chunkArchetype.pFirstChunk;
	if (pChunk->pNext)
		pChunk->pNext->pPrev = pChunk;

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
		for (gSize_t i = 0; i < pChunkArchetype->archetype.componentCount; i++)
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

void EntityManager::DeleteEntity(const Entity& entity)
{
	const EntityPointer p = GetEntityPointer(entity);

	// Check if only entity in chunk
	if (p.pChunk->numberOfEntities <= 1)
	{
		// Check if only chunk in archetype
		if (!p.pChunk->pNext && !p.pChunk->pPrev)
			DeleteChunkArchetype(p.pChunk->pChunkArchetype);
		else
			DeleteChunk(p.pChunk);
	}
	else
	{
		// Check if not last entity in chunk
		if (p.indexInChunk < p.pChunk->numberOfEntities - 1)
		{
			// Copy last entity in the chunk to old position
			const EntityPointer pLast(p.pChunk, p.pChunk->numberOfEntities - 1);
			const EntityArchetype& archetype = p.pChunk->pChunkArchetype->archetype;
			char* pComponentStream = reinterpret_cast<char*>(p.pChunk + 1);

			for (gSize_t i = 0; i < archetype.componentCount; ++i)
			{
				const gSize_t& offset = archetype.components[i].offset;
				const gSize_t& size = archetype.components[i].size;

				char* thisComponentStream = pComponentStream + p.pChunk->pChunkArchetype->maxEntities * offset;

				const void* source = thisComponentStream + (size * pLast.indexInChunk);
				void* dest = thisComponentStream + (size * p.indexInChunk);

				memcpy(dest, source, size);
			}

			// Change indexInChunk for newly moved last object
			IdComponent& id = GetComponent<IdComponent>(p);
			entityTable[id.index].p.indexInChunk = p.indexInChunk;
		}

		--p.pChunk->numberOfEntities;
	}

	const gEntityIndex_t indexInIndices = entityTable[entity.index].indexInIndices;

	// Check if last index in index array and index in natural position
	gSize_t trueLastIndex = static_cast<gSize_t>(entityIndices.size()) - 1;
	gSize_t lastIndex = numberOfEntities - 1;

	if (lastIndex != indexInIndices)
	{
		// Swap last index in indices array
		const gEntityIndex_t temp = entityIndices[lastIndex];

		entityIndices[lastIndex] = entityIndices[indexInIndices];
		entityIndices[indexInIndices] = temp;

		entityTable[entity.index].indexInIndices = lastIndex;
		entityTable[temp].indexInIndices = indexInIndices;
	}

	if (entityIndices[trueLastIndex] == trueLastIndex)
	{
		//entityIndices.pop_back();
	}

	--numberOfEntities;
}

void EntityManager::DeleteChunk(Chunk* pChunk)
{
#ifdef COMPONENT_DEBUG
	std::cout << "	Deleting chunk.		Entities: " << pChunk->numberOfEntities << "\n";
#endif // COMPONENT_DEBUG

	if (pChunk->pPrev)
		pChunk->pPrev->pNext = pChunk->pNext;
	else
		pChunk->pChunkArchetype->pFirstChunk = pChunk->pNext;

	if (pChunk->pNext)
		pChunk->pNext->pPrev = pChunk->pPrev;

	free(pChunk);
}

void EntityManager::DeleteChunkArchetype(ChunkArchetypeElement* pArchetype)
{
#ifdef COMPONENT_DEBUG
	std::cout << "Deleting chunk archetype of: ";
	for (gSize_t i = 0; i < chunkArchetypeList->archetype.componentCount; i++)
	{
		std::cout << (i != 0 ? ", " : "") << chunkArchetypeList->archetype.components[i].name;
	}
	std::cout << "\n";
#endif // COMPONENT_DEBUG

	// Link around deleted element
	if (pArchetype->pPrev)
		pArchetype->pPrev->pNext = pArchetype->pNext;
	else
		chunkArchetypeList = pArchetype->pNext;

	if (pArchetype->pNext)
		pArchetype->pNext->pPrev = pArchetype->pPrev;

	delete pArchetype;
}

void EntityManager::DeleteAllEntities()
{
	if (chunkArchetypeList == nullptr)
		return;

	// Delete each chunk archetype element
	while (chunkArchetypeList != nullptr)
	{
		DeleteChunkArchetype(chunkArchetypeList);
	}
}
