#include <pch.h>
#include <systems/freecamsystem.h>

#include <gl/gl.h>
#include <components/cameracomponent.h>
#include <components/positioncomponent.h>
#include <components/unscaledvelocitycomponent.h>
#include <input/inputmanager.h>

void FreecamSystem::Update()
{
	glm::vec3 moveVector(0);
	glm::vec3 panVector(0);
	double pitchDelta = 0;
	double yawDelta = 0;

	// Move keys
	if (inputManager.GetButtonDown(IN_MoveForward))
	{
		moveVector.z -= 1;
	}
	if (inputManager.GetButtonDown(IN_MoveBack))
	{
		moveVector.z += 1;
	}

	if (inputManager.GetButtonDown(IN_MoveLeft))
	{
		moveVector.x -= 1;
	}
	if (inputManager.GetButtonDown(IN_MoveRight))
	{
		moveVector.x += 1;
	}

	float rotSpeed = 2;

	float delta = timeManager.GetUnscaledDeltaTime();

	// Look keys
	if (inputManager.GetButtonDown(IN_LookUp))
	{
		pitchDelta -= delta * rotSpeed;
	}
	if (inputManager.GetButtonDown(IN_LookDown))
	{
		pitchDelta += delta * rotSpeed;
	}

	if (inputManager.GetButtonDown(IN_LookLeft))
	{
		yawDelta -= delta * rotSpeed;
	}
	if (inputManager.GetButtonDown(IN_LookRight))
	{
		yawDelta += delta * rotSpeed;
	}

	// Look mouse
	double xDelta, yDelta;
	inputManager.GetCursorDelta(&xDelta, &yDelta);
	panVector.y = -(float)xDelta;
	panVector.x = -(float)yDelta;
	pitchDelta += yDelta * 0.0015f;
	yawDelta += xDelta * 0.0015;

	EntityManager& em = entityManager;

	std::vector<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<FreecamComponent>());

	if (archetypes == nullptr)
		return;

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
				FreecamComponent&  freeCam  = em.GetComponent<FreecamComponent>(entity);
				CameraComponent&   cam      = em.GetComponent<CameraComponent>(entity);
				UnscaledVelocityComponent& velocity = em.GetComponent<UnscaledVelocityComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);
				PositionComponent& position = em.GetComponent<PositionComponent>(entity);

				if (!freeCam.enabled)
				{
					velocity.velocity.linear *= 0;
					continue;
				}

				// Panning
				if (freeCam.panning || freeCam.panOnly)
				{
					float pixelsPerUnit = cam.frustumScale * freeCam.viewPortSize[1] * 0.5f;
					panVector /= pixelsPerUnit;

					position.value += panVector * glm::fquat(0.70710678118f, 0, 0, 0.70710678118f) * rotation.value;

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

				float speed = glm::length(velocity.velocity.linear);

				float drop = freeCam.friction * delta;

				// Friction
				if (speed > drop)
				{
					float m = (speed - drop) / speed;

					velocity.velocity.linear *= m;
				}
				else
				{
					velocity.velocity.linear *= 0;
				}

				// Acceleration
				velocity.velocity.linear += moveVector * freeCam.acceleration * delta;

				if (speed > freeCam.speed)
				{
					float m = freeCam.speed / speed;

					velocity.velocity.linear *= m;
				}
			}
		}
	}

	delete archetypes;
}