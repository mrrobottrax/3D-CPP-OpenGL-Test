#include <pch.h>

#include <systems/freecamsystem.h>

#include <inputmanager.h>
#include <gl/gl.h>

FreecamSystem::FreecamSystem()
{
}

FreecamSystem::~FreecamSystem()
{
}

void FreecamSystem::Update()
{
	glm::vec3 moveVector(0);
	float pitchDelta = 0;
	float yawDelta = 0;

	// Move keys
	if (InputManager::keybindings[GLFW_KEY_W])
	{
		moveVector.z -= 1;
	}
	if (InputManager::keybindings[GLFW_KEY_S])
	{
		moveVector.z += 1;
	}

	if (InputManager::keybindings[GLFW_KEY_A])
	{
		moveVector.x -= 1;
	}
	if (InputManager::keybindings[GLFW_KEY_D])
	{
		moveVector.x += 1;
	}

	float rotSpeed = 2;

	// Look keys
	if (InputManager::keybindings[GLFW_KEY_UP])
	{
		pitchDelta -= TimeManager::deltaTime * rotSpeed;
	}
	if (InputManager::keybindings[GLFW_KEY_DOWN])
	{
		pitchDelta += TimeManager::deltaTime * rotSpeed;
	}

	if (InputManager::keybindings[GLFW_KEY_LEFT])
	{
		yawDelta -= TimeManager::deltaTime * rotSpeed;
	}
	if (InputManager::keybindings[GLFW_KEY_RIGHT])
	{
		yawDelta += TimeManager::deltaTime * rotSpeed;
	}

	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<FreecamComponent>());

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
				FreecamComponent&  freeCam  = em.GetComponent<FreecamComponent>(entity);
				VelocityComponent& velocity = em.GetComponent<VelocityComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);

				// Look
				glm::fquat deltaRot(glm::vec3(pitchDelta, 0, 0));
				rotation.value = deltaRot * rotation.value;

				deltaRot = glm::fquat(glm::vec3(0, yawDelta, 0));
				rotation.value = rotation.value * deltaRot;

				moveVector = moveVector * rotation.value;

				float speed = glm::length(velocity.linear);

				float drop = freeCam.friction * TimeManager::deltaTime;

				// Friction
				if (speed > drop)
				{
					float m = (speed - drop) / speed;

					velocity.linear *= m;
				}
				else
				{
					velocity.linear *= 0;
				}

				// Acceleration
				velocity.linear += moveVector * freeCam.acceleration * TimeManager::deltaTime;

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