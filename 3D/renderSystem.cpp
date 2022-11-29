#include <renderSystem.h>

#include <iostream>
#include <matrixStack.h>
#include <entityManager.h>
#include <meshComponent.h>
#include <positionComponent.h>
#include <rotationComponent.h>
#include <main.h>

RenderSystem::RenderSystem(Entity& mainEntity)
{
	RenderSystem::mainCamera = &EntityManager::GetInstance().getComponent<CameraComponent>(mainEntity);

	initMainCameraMatrix();
}

RenderSystem::RenderSystem()
{
	mainCamera = nullptr;
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::setMainCameraEntity(Entity& entity)
{
	RenderSystem::mainCamera = &EntityManager::GetInstance().getComponent<CameraComponent>(entity);
	initMainCameraMatrix();
}

void RenderSystem::initMainCameraMatrix()
{
	mainCamera->frustumScale = calcFrustumScale(mainCamera->fov);
	glm::mat4 matrix = calcPerspectiveMatrix();

	for (int i = 0; i < 16; i++)
	{
		mainCamera->matrix = matrix;
	}
}

float RenderSystem::calcFrustumScale(float fov)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fovRad = fov * degToRad;
	return 1.0f / tan(fovRad / 2.0f);
}

void RenderSystem::updateMatrixAspect(int width, int height)
{
	if (mainCamera == nullptr)
		return;

	mainCamera->matrix[0][0] = mainCamera->frustumScale / (width / (float)height);
	mainCamera->matrix[1][1] = mainCamera->frustumScale;
}

glm::mat4 RenderSystem::calcPerspectiveMatrix()
{
	glm::mat4 matrix = glm::mat4(0.0f);

	matrix[0][0] = mainCamera->frustumScale;
	matrix[1][1] = mainCamera->frustumScale;
	matrix[2][2] = (mainCamera->farClip + mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);
	matrix[2][3] = -1.0f;
	matrix[3][2] = (2 * mainCamera->farClip * mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);

	return matrix;
}

void RenderSystem::update()
{
	if (mainCamera == nullptr)
	{
		return;
	}

	MatrixStack mStack;

	mStack.push(mainCamera->matrix);

	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.findChunkArchetypesWithComponent(Component().init<MeshComponent>());

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
				MeshComponent& mesh = em.getComponent<MeshComponent>(entity);
				PositionComponent& position = em.getComponent<PositionComponent>(entity);
				RotationComponent& rotation = em.getComponent<RotationComponent>(entity);

				mStack.pushCpy();
				mStack.translate(position.value);
				mStack.applyMatrix(glm::mat4_cast(rotation.value));

				// Draw object
				glUniform4f(colorUniform, 1, 1, 1, 1);
				glUniform3f(offsetUniform, 0, 0, 0);
				glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &mStack.top()[0][0]);

				glDrawElements(GL_TRIANGLES, mesh.mesh->indicesCount, GL_UNSIGNED_SHORT, 0);

				mStack.pop();
			}
		}
	}

	delete archetypes;

	mStack.pop();
}
