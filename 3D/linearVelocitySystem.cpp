#include <linearVelocitySystem.h>

#include <systemManager.h>
#include <entityManager.h>
#include <positionComponent.h>
#include <linearVelocityComponent.h>
#include <forward_list>
#include <component.h>

LinearVelocitySystem::LinearVelocitySystem()
{
}

LinearVelocitySystem::~LinearVelocitySystem()
{
}

void LinearVelocitySystem::update()
{
	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.findChunkArchetypesWithComponent(Component().init<LinearVelocityComponent>());

	if (archetypes == nullptr)
		return;

	// For each archetype
	for (std::forward_list<ChunkArchetypeElement*>::iterator chunkArchetypeIt = archetypes->begin(); chunkArchetypeIt != archetypes->end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* chunk = (*chunkArchetypeIt)->firstChunk; chunk != nullptr; chunk = chunk->next)
		{
			// For each entity
			for (unsigned short i = 0; i < chunk->numberOfEntities; i++)
			{
				Entity entity((*chunkArchetypeIt)->archetype, *chunk, i);
				PositionComponent& position = em.getComponent<PositionComponent>(entity);
				LinearVelocityComponent& velocity = em.getComponent<LinearVelocityComponent>(entity);

				position.x += velocity.x;
				position.y += velocity.y;
				position.z += velocity.z;
			}
		}
	}

	delete archetypes;
}
