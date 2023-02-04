#include "malletpch.h"
#include "viewport3d.h"

#include <inputmanager.h>

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>

#include <components/idcomponent.h>
#include <components/positioncomponent.h>

#include <gl/glutil.h>

#include <main.h>

#include <common/matrixstack.h>
#include <components/meshcomponent.h>

Viewport3D::Viewport3D() : cameraEntity(), viewPosX(), viewPosY(), viewSizeX(), viewSizeY()
{
	EntityManager& em = EntityManager::GetInstance();

	Component components[] = {
		Component().init<IdComponent>(),
		Component().init<PositionComponent>(),
		Component().init<VelocityComponent>(),
		Component().init<CameraComponent>(),
		Component().init<RotationComponent>(),
		Component().init<FreecamComponent>(),
	};

	Entity entity = em.AddEntity(EntityArchetype(6, components));
	em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
	em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
	CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = { 80.0f, 0.03f, 1000.0f };
	em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
	em.GetComponent<FreecamComponent>(entity) = { false, 6, 40, 20 };

	cameraEntity = entity;

	int w, h;
	glfwGetWindowSize(mainWindow, &w, &h);

	RenderSystem::CalcFrustumScale(&cam);
	RenderSystem::CalcPerspectiveMatrix(&cam, w, h);
}

Viewport3D::~Viewport3D()
{
}

void Viewport3D::Draw(DockingLeaf& leaf, int leafIndex)
{
	glViewport(viewPosX, viewPosY, viewSizeX, viewSizeY);

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
	mStack.applyMatrix(glm::mat4_cast(-em.GetComponent<RotationComponent>(cameraEntity).value));
	mStack.translate(-em.GetComponent<PositionComponent>(cameraEntity).value);

	glm::vec3 sunDir(1, 2, .2f);
	sunDir = glm::normalize(sunDir);

	float sunIntensity = 1.3f;
	float ambientIntensity = 0.1f;

	CameraComponent* camera = &em.GetComponent<CameraComponent>(cameraEntity);

	glUniformMatrix4fv(perspectiveMatrix, 1, GL_FALSE, &camera->matrix[0][0]);
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
				MeshComponent& mesh = em.GetComponent<MeshComponent>(entity);
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

void Viewport3D::CalculateViewportVars(DockingLeaf& leaf, int fullWindowWidth, int fullWindowHeight)
{
	viewPosX  = (GLint)leaf.absPos[0];
	viewPosY  = (GLint)(fullWindowHeight - (leaf.absPos[1] + leaf.absSize[1]));
	viewSizeX = (GLsizei)leaf.absSize[0];
	viewSizeY = (GLsizei)leaf.absSize[1];
}

void Viewport3D::OnResize(DockingLeaf& leaf, int fullWindowWidth, int fullWindowHeight)
{
	CalculateViewportVars(leaf, fullWindowWidth, fullWindowHeight);

	EntityManager& em = EntityManager::GetInstance();
	RenderSystem::UpdateMatrixAspect(&em.GetComponent<CameraComponent>(cameraEntity), leaf.absSize[0], leaf.absSize[1]);
}

void Viewport3D::OnSelect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	EntityManager& em = EntityManager::GetInstance();
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = true;
}

void Viewport3D::OnDeselect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	EntityManager& em = EntityManager::GetInstance();
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = false;
}

void Viewport3D::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
}

void Viewport3D::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void Viewport3D::MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	
}
