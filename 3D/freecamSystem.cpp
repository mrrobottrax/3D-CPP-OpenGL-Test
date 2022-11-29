#include "freecamSystem.h"

#include <inputManager.h>
#include <gl.h>
#include <glm/glm.hpp>

#include <iostream>

FreecamSystem::FreecamSystem()
{
}

FreecamSystem::~FreecamSystem()
{
}

void FreecamSystem::update()
{
	glm::vec3 moveVector(0);

	if (inputManager::keybindings[GLFW_KEY_W])
	{
		moveVector.z -= 1;
	}
	if (inputManager::keybindings[GLFW_KEY_S])
	{
		moveVector.z += 1;
	}

	if (inputManager::keybindings[GLFW_KEY_A])
	{
		moveVector.x -= 1;
	}
	if (inputManager::keybindings[GLFW_KEY_D])
	{
		moveVector.x += 1;
	}

	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.findChunkArchetypesWithComponent(Component().init<FreecamComponent>());

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
				FreecamComponent& freeCam = em.getComponent<FreecamComponent>(entity);
				VelocityComponent& velocity = em.getComponent<VelocityComponent>(entity);
				
				float speed = glm::length(velocity.linear);

				float drop = freeCam.friction * timeManager::deltaTime;

				// Friction
				if (speed > drop)
				{
					float m = (speed - drop) / speed;

					velocity.linear *= m;
				}

				// Acceleration
				velocity.linear += moveVector * freeCam.acceleration * timeManager::deltaTime;

				if (speed > freeCam.speed)
				{
					float m = freeCam.speed / speed;

					velocity.linear *= m;
				}
			}
		}
	}

	delete archetypes;
}
