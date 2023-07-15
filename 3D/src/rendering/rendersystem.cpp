#include "pch.h"
#include "renderSystem.h"

#include "debugdraw.h"

#include <components/scalecomponent.h>
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/meshcomponent.h>
#include <common/matrixstack.h>

#include <gl/glutil.h>

Cvar r_draw = { "r_draw", 1 };

void RenderSystem::SetMainCameraEntity(Entity& entity)
{
	RenderSystem::pMainCamera = &entityManager.GetComponent<CameraComponent>(entity);
	RenderSystem::mainCameraEntity = entity;
}

void RenderSystem::CalcFrustumScale(CameraComponent& camera, const float fov)
{
	if (!camera.ortho)
	{
		const float degToRad = 3.14159f * 2.0f / 360.0f;
		float fovRad = fov * degToRad;
		float scale = 1.0f / tan(fovRad / 2.0f);

		camera.frustumScale = scale;
	}
}

void RenderSystem::UpdateMatrixAspect(CameraComponent& camera, const int width, const int height)
{
	camera.matrix[0][0] = camera.frustumScale / (width / (float)height);
	camera.matrix[1][1] = camera.frustumScale;
}

void RenderSystem::CalcPerspectiveMatrix(CameraComponent& camera, const int width, const int height)
{
	glm::mat4 matrix = glm::mat4(0.0f);

	if (!camera.ortho)
	{
		matrix[0][0] = camera.frustumScale / (width / (float)height);
		matrix[1][1] = camera.frustumScale;
		matrix[2][2] = (camera.farClip + camera.nearClip) / (camera.nearClip - camera.farClip);
		matrix[2][3] = -1.0f;
		matrix[3][2] = (2 * camera.farClip * camera.nearClip) / (camera.nearClip - camera.farClip);
	}
	else
	{
		matrix[0][0] = camera.frustumScale / (width / (float)height);
		matrix[1][1] = camera.frustumScale;
		matrix[2][2] = -2 / (camera.farClip - camera.nearClip);
		matrix[3][3] = 1;
	}

	camera.matrix = matrix;
}

void RenderSystem::Update()
{
	if (!autoDraw)
	{
		return;
	}

	if (pMainCamera == nullptr)
	{
		return;
	}

	DrawShaded();
	debugDraw.Render();
}

void RenderSystem::DrawShaded()
{
	if (!r_draw.value)
	{
		return;
	}

	glUseProgram(standardShaderProgram);
	glBindVertexArray(vao);

	DrawBase();

	glBindVertexArray(0);
	glUseProgram(0);
}

void RenderSystem::DrawWireframe()
{
	glUseProgram(wireframeShaderProgram);
	glBindVertexArray(vao);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	DrawBase();

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);

	glBindVertexArray(0);
	glUseProgram(0);
}

void RenderSystem::DrawBase()
{
	EntityManager& em = entityManager;

	std::vector<ChunkArchetypeElement*> archetypes = em.FindChunkArchetypesWithComponent(Component().Init<MeshComponent>());
	if (archetypes.size() == 0)
		return;

	MatrixStack mStack;

	mStack.Push();
	mStack.ApplyMatrix(glm::mat4_cast(em.GetComponent<RotationComponent>(mainCameraEntity).value));
	mStack.Translate(-em.GetComponent<PositionComponent>(mainCameraEntity).value);

	glm::vec3 sunDir(1, 2, .2f);
	sunDir = glm::normalize(sunDir);

	float sunIntensity = 1.3f;
	float ambientIntensity = 0.1f;

	glUniformMatrix4fv(sharedPerspectiveMatrixUnif, 1, GL_FALSE, &pMainCamera->matrix[0][0]);
	glUniform1f(sunIntensityUnif, sunIntensity);
	glUniform1f(ambientIntensityUnif, ambientIntensity);

	glm::mat3 normalMat;

	// For each archetype
	for (auto chunkArchetypeIt = archetypes.begin(); chunkArchetypeIt != archetypes.end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* pChunk = (*chunkArchetypeIt)->pFirstChunk; pChunk != nullptr; pChunk = pChunk->pNext)
		{
			// For each entity
			for (unsigned short i = 0; i < pChunk->numberOfEntities; i++)
			{
				const Entity entity((*chunkArchetypeIt)->archetype, *pChunk, i);
				const MeshComponent& mesh = em.GetComponent<MeshComponent>(entity);
				const PositionComponent& position = em.GetComponent<PositionComponent>(entity);
				const RotationComponent& rotation = em.GetComponent<RotationComponent>(entity);
				const ScaleComponent* pScale = em.GetComponentP<ScaleComponent>(entity);

				mStack.PushCpy();
				mStack.Translate(position.value);
				mStack.ApplyMatrix(glm::mat4_cast(rotation.value));
				if (pScale)
					mStack.Scale(pScale->value);

				normalMat = glm::mat3_cast(rotation.value);

				// Draw object

				glBindBuffer(GL_ARRAY_BUFFER, mesh.pMesh->positionBufferObject);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // in position
				glBindBuffer(GL_ARRAY_BUFFER, mesh.pMesh->normalBufferObject);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0); // in normal
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.pMesh->elementBufferObject);

				// TODO: Faster to have the cpu do this? Or maybe just give the gpu rotation, position, etc matrices?
				glUniformMatrix4fv(sharedPositionMatrixUnif, 1, GL_FALSE, &mStack.Top()[0][0]);
				glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, &normalMat[0][0]);
				glUniform3f(sunDirUnif, sunDir.x, sunDir.y, sunDir.z);
				glUniform4f(colorUnif, 1, 1, 1, 1);

				glDrawElements(GL_TRIANGLES, mesh.pMesh->indicesCount, GL_UNSIGNED_SHORT, 0);

				mStack.Pop();
			}
		}
	}

	mStack.Pop();
}
