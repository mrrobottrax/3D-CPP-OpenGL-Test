#include <pch.h>
#include <systems/freecamsystem.h>

#include <inputmanager.h>
#include <gl/gl.h>
#include <managers.h>
#include <components/cameracomponent.h>

FreecamSystem::FreecamSystem()
{
}

FreecamSystem::~FreecamSystem()
{
}

void FreecamSystem::Update()
{
	glm::vec3 moveVector(0);
	glm::vec3 panVector(0);
	double pitchDelta = 0;
	double yawDelta = 0;

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

	// Look mouse
	double xDelta, yDelta;
	InputManager::GetCursorDelta(&xDelta, &yDelta);
	panVector.y = -(float)xDelta;
	panVector.x = -(float)yDelta;
	pitchDelta += yDelta * 0.0015f;
	yawDelta += xDelta * 0.0015;

	EntityManager& em = *entityManager;

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
				CameraComponent&   cam      = em.GetComponent<CameraComponent>(entity);
				VelocityComponent& velocity = em.GetComponent<VelocityComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);

				if (!freeCam.enabled)
				{
					velocity.linear *= 0;
					continue;
				}

				// Panning
				if (freeCam.panning || freeCam.panOnly)
				{
					velocity.linear = panVector * glm::fquat(0.70710678118f, 0, 0, 0.70710678118f) * rotation.value;

					if (freeCam.panOnly)
						velocity.linear /= cam.frustumScale * 0.3f;
					else
						velocity.linear /= cam.frustumScale * 0.1f;

					continue;
				}

				// Look
				if (!freeCam.panOnly && !freeCam.panning)
				{
					glm::fquat deltaRot(glm::vec3(pitchDelta, 0, 0));
					rotation.value = deltaRot * rotation.value;

					deltaRot = glm::fquat(glm::vec3(0, yawDelta, 0));
					rotation.value = rotation.value * deltaRot;

					moveVector = moveVector * rotation.value;
				}

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