#include <pch.h>

#include <engine/velocitySystem.h>

#include <engine/systemManager.h>
#include <engine/memory/entityManager.h>
#include <engine/positionComponent.h>
#include <engine/rotationComponent.h>
#include <engine/velocityComponent.h>
#include <engine/memory/component.h>
#include <engine/timeManager.h>

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
				RotationComponent& rotation = em.getComponent<RotationComponent>(entity);
				VelocityComponent& velocity = em.getComponent<VelocityComponent>(entity);

				position.value += velocity.linear * timeManager::deltaTime;

				glm::vec3 deltaVec = velocity.angular * timeManager::deltaTime;
				float length = glm::length(deltaVec);

				// Prevent divide by 0
				if (length < 1E-16f)
					continue;

				float half = length / 2.0f;
				float sin = std::sin(half);
				float cos = std::cos(half);

				glm::fquat deltaRot(length * cos, deltaVec.x * sin, deltaVec.y * sin, deltaVec.z * sin);
				rotation.value = deltaRot * rotation.value;
				rotation.value = glm::normalize(rotation.value);
			}
		}
	}

	delete archetypes;
}
