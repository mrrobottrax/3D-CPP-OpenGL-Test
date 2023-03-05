#include <pch.h>
#include <systems/velocitySystem.h>

#include <systems/systemmanager.h>
#include <memory/entitymanager.h>
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/velocitycomponent.h>
#include <memory/component.h>
#include <timemanager.h>

VelocitySystem::VelocitySystem()
{
}

VelocitySystem::~VelocitySystem()
{
}

void VelocitySystem::Update()
{
	EntityManager& em = entityManager;

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<VelocityComponent>());

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
				PositionComponent& position = em.GetComponent<PositionComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);
				VelocityComponent& velocity = em.GetComponent<VelocityComponent>(entity);

				position.value += velocity.linear * timeManager.GetDeltaTime();

				glm::vec3 deltaVec = velocity.angular * timeManager.GetDeltaTime();
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
