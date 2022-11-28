#include <velocitySystem.h>

#include <systemManager.h>
#include <entityManager.h>
#include <positionComponent.h>
#include <velocityComponent.h>
#include <forward_list>
#include <component.h>

VelocitySystem::VelocitySystem()
{
}

VelocitySystem::~VelocitySystem()
{
}

void VelocitySystem::update()
{
	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.findChunkArchetypesWithComponent(Component().init<VelocityComponent>());

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
				VelocityComponent& velocity = em.getComponent<VelocityComponent>(entity);

				position.value.x += velocity.linear.x;
				position.value.y += velocity.linear.y;
				position.value.z += velocity.linear.z;
			}
		}
	}

	delete archetypes;
}
