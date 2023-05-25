#include <pch.h>
#include <systems/velocitySystem.h>

#include <common/math.h>

#include <systems/systemmanager.h>
#include <memory/entitymanager.h>
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/velocitycomponent.h>
#include <components/unscaledvelocitycomponent.h>
#include <memory/component.h>
#include <timemanager.h>

VelocitySystem::VelocitySystem()
{
}

VelocitySystem::~VelocitySystem()
{
}

void VelocitySystem::UpdatePositions(const std::vector<ChunkArchetypeElement*>* archetypes, const float& deltaTime, bool scaled = true)
{
	if (archetypes == nullptr)
		return;

	const EntityManager& em = entityManager;

	// For each archetype
	for (auto chunkArchetypeIt = archetypes->begin(); chunkArchetypeIt != archetypes->end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* pChunk = (*chunkArchetypeIt)->pFirstChunk; pChunk != nullptr; pChunk = pChunk->pNext)
		{
			// For each entity
			for (unsigned short i = 0; i < pChunk->numberOfEntities; i++)
			{
				Entity entity((*chunkArchetypeIt)->archetype, *pChunk, i);
				PositionComponent& position = em.GetComponent<PositionComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);

				VelocityComponent* pVelocity;
				if (scaled)
				{
					pVelocity = em.GetComponentP<VelocityComponent>(entity);
				}
				else
				{
					pVelocity = &em.GetComponentP<UnscaledVelocityComponent>(entity)->velocity;
				}

				VelocityComponent& velocity = *pVelocity;

				position.value += velocity.linear * deltaTime;

				const glm::vec3& deltaVec = velocity.angular * deltaTime;
				const glm::fquat& deltaRot = gmath::EulerToQuaternion(deltaVec);
				rotation.value = deltaRot * rotation.value;
			}
		}
	}
}

void VelocitySystem::Update()
{
	EntityManager& em = entityManager;

	const std::vector<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<VelocityComponent>());
	UpdatePositions(archetypes, timeManager.GetFixedDeltaTime());
	delete archetypes;
}
