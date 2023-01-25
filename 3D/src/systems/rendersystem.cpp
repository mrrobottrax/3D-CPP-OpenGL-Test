#include <pch.h>

#include <systems/renderSystem.h>

#include <common/matrixStack.h>
#include <memory/entityManager.h>
#include <components/meshComponent.h>
#include <components/positionComponent.h>
#include <components/rotationComponent.h>

#include <gl/glutil.h>

RenderSystem::RenderSystem(Entity& mainEntity)
{
	RenderSystem::mainCamera = &EntityManager::GetInstance().GetComponent<CameraComponent>(mainEntity);

	InitMainCameraMatrix();
}

RenderSystem::RenderSystem()
{
	mainCamera = nullptr;
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::SetMainCameraEntity(Entity& entity)
{
	RenderSystem::mainCamera = &EntityManager::GetInstance().GetComponent<CameraComponent>(entity);
	RenderSystem::mainCameraEntity = entity;
	InitMainCameraMatrix();
}

void RenderSystem::InitMainCameraMatrix()
{
	mainCamera->frustumScale = CalcFrustumScale(mainCamera->fov);
	glm::mat4 matrix = CalcPerspectiveMatrix();

	for (int i = 0; i < 16; i++)
	{
		mainCamera->matrix = matrix;
	}
}

float RenderSystem::CalcFrustumScale(float fov)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fovRad = fov * degToRad;
	return 1.0f / tan(fovRad / 2.0f);
}

void RenderSystem::UpdateMatrixAspect(int width, int height)
{
	if (mainCamera == nullptr)
		return;

	mainCamera->matrix[0][0] = mainCamera->frustumScale / (width / (float)height);
	mainCamera->matrix[1][1] = mainCamera->frustumScale;
}

glm::mat4 RenderSystem::CalcPerspectiveMatrix()
{
	glm::mat4 matrix = glm::mat4(0.0f);

	int width, height;
	glfwGetWindowSize(mainWindow, &width, &height);

	matrix[0][0] = mainCamera->frustumScale / (width / (float)height);
	matrix[1][1] = mainCamera->frustumScale;
	matrix[2][2] = (mainCamera->farClip + mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);
	matrix[2][3] = -1.0f;
	matrix[3][2] = (2 * mainCamera->farClip * mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);

	return matrix;
}

void RenderSystem::Update()
{
	if (mainCamera == nullptr)
	{
		return;
	}

	EntityManager& em = EntityManager::GetInstance();

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<MeshComponent>());
	if (archetypes == nullptr)
		return;

	glUseProgram(shaderProgram);
	glBindVertexArray(vao);

	MatrixStack mStack;
	glm::mat3 normalMat;

	mStack.push();
	normalMat = mStack.top();
	mStack.applyMatrix(glm::mat4_cast(-em.GetComponent<RotationComponent>(mainCameraEntity).value));
	mStack.translate(-em.GetComponent<PositionComponent>(mainCameraEntity).value);

	glm::vec3 sunDir(1, 2, .2f);
	sunDir = glm::normalize(sunDir);

	float sunIntensity = 1.3f;
	float ambientIntensity = 0.1f;

	glUniformMatrix4fv(perspectiveMatrix, 1, GL_FALSE, &mainCamera->matrix[0][0]);
	glUniform1f(sunIntensityUnif, sunIntensity);
	glUniform1f(ambientIntensityUnif, ambientIntensity);

	glm::mat3 newNormalMat;

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
				MeshComponent&     mesh     = em.GetComponent<MeshComponent>(entity);
				PositionComponent& position = em.GetComponent<PositionComponent>(entity);
				RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);

				mStack.pushCpy();
				mStack.translate(position.value);
				mStack.applyMatrix(glm::mat4_cast(rotation.value));

				newNormalMat = glm::mat3_cast(rotation.value) * normalMat;

				// Draw object
				glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh->positionBufferObject);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh->normalBufferObject);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mesh->elementBufferObject);

				glUniformMatrix4fv(positionMatrix, 1, GL_FALSE, &mStack.top()[0][0]);
				glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, &newNormalMat[0][0]);
				glUniform3f(sunDirUnif, sunDir.x, sunDir.y, sunDir.z);
				glUniform4f(colorUnif, 1, 1, 1, 1);

				glDrawElements(GL_TRIANGLES, mesh.mesh->indicesCount, GL_UNSIGNED_SHORT, 0);

				mStack.pop();
			}
		}
	}

	mStack.pop();

	glBindVertexArray(0);
	glUseProgram(0);

	delete archetypes;
}
