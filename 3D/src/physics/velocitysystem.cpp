#include "pch.h"
#include "velocitysystem.h"

#include <components/unscaledvelocitycomponent.h>
#include <components/velocitycomponent.h>
#include <components/rotationcomponent.h>
#include <components/positioncomponent.h>

#include <common/math.h>

void VelocitySystem::UpdatePositions(const std::vector<ChunkArchetypeElement*>& archetypes, const float& deltaTime, bool scaled = true)
{
	if (archetypes.size() == 0)
		return;

	const EntityManager& em = entityManager;

	// For each archetype
	for (auto chunkArchetypeIt = archetypes.begin(); chunkArchetypeIt != archetypes.end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* pChunk = (*chunkArchetypeIt)->pFirstChunk; pChunk != nullptr; pChunk = pChunk->pNext)
		{
			// For each entity
			for (gSize_t i = 0; i < pChunk->numberOfEntities; i++)
			{
				const EntityPointer p(pChunk, i);
				PositionComponent& position = em.GetComponent<PositionComponent>(p);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(p);

				VelocityComponent* pVelocity;
				if (scaled)
				{
					pVelocity = em.GetComponentP<VelocityComponent>(p);
				}
				else
				{
					pVelocity = &em.GetComponentP<UnscaledVelocityComponent>(p)->velocity;
				}

				VelocityComponent& velocity = *pVelocity;

				position.value += velocity.linear * deltaTime;

				const glm::vec3& deltaVec = velocity.angular * deltaTime;
				const glm::fquat& deltaRot = gMath::EulerToQuaternion(deltaVec);
				rotation.value = deltaRot * rotation.value;
			}
		}
	}
}

void VelocitySystem::Update()
{
	EntityManager& em = entityManager;

	const std::vector<ChunkArchetypeElement*> archetypes = em.FindChunkArchetypesWithComponent(Component().Init<VelocityComponent>());
	UpdatePositions(archetypes, timeManager.GetFixedDeltaTime());
}
