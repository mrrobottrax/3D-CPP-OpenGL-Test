#include "pch.h"
#include "debugdraw.h"

#include <components/positioncomponent.h>
#include <common/matrixstack.h>

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>

void DebugDraw::DrawLine(const float start[3], const float end[3])
{
	lineVertices.insert(lineVertices.end(), start, start + 3);
	lineVertices.insert(lineVertices.end(), end, end + 3);
}

void DebugDraw::DrawLine(glm::vec3 start, glm::vec3 end)
{
	lineVertices.push_back(start[0]);
	lineVertices.push_back(start[1]);
	lineVertices.push_back(start[2]);
	
	lineVertices.push_back(end[0]);
	lineVertices.push_back(end[1]);
	lineVertices.push_back(end[2]);
}

void DebugDraw::DrawPlane(glm::vec3 offset, glm::vec3 normal, float dist, float width, float height)
{
	DrawPlane(offset, normal, glm::vec3(0, 1, 0), dist, width, height);
}

void DebugDraw::DrawPlane(glm::vec3 offset, glm::vec3 normal, glm::vec3 upHint, float dist, float width, float height)
{
	glm::normalize(normal);

	glm::vec3 rightVec = normal == upHint ? upHint : glm::cross(normal, glm::vec3(0, 1, 0));
	rightVec = glm::normalize(rightVec);
	glm::vec3 upVec = glm::cross(normal, rightVec);
	upVec = glm::normalize(upVec);

	float sideExtent = width / 2.0f;
	float heightExtent = height / 2.0f;

	glm::vec3 corner1 = offset + normal * dist - rightVec * sideExtent - upVec * heightExtent;
	glm::vec3 corner2 = offset + normal * dist + rightVec * sideExtent + upVec * heightExtent;

	glm::vec3 point1;
	glm::vec3 point2;

	DrawLine(corner1, corner2);

	// Left side
	point1 = corner1;
	point2 = corner1 + upVec * height;

	DrawLine(point1, point2);

	// Right side
	point1 = corner2;
	point2 = corner2 - upVec * height;

	DrawLine(point1, point2);

	// Top
	point1 = corner2;
	point2 = corner2 - rightVec * width;

	DrawLine(point1, point2);

	// Bottom
	point1 = corner1;
	point2 = corner1 + rightVec * width;

	DrawLine(point1, point2);
}

void DebugDraw::Update()
{
	glUseProgram(wireframeShaderProgram);
	glBindVertexArray(vao);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	Draw();

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
	glUseProgram(0);
}

void DebugDraw::Draw()
{
	EntityManager& em = entityManager;
	SystemManager& sm = systemManager;

	// Camera
	MatrixStack mStack;

	RenderSystem& renderSystem = sm.GetSystem<RenderSystem>();
	Entity& mainCameraEntity = renderSystem.mainCameraEntity;
	mStack.push();
	mStack.applyMatrix(glm::mat4_cast(em.GetComponent<RotationComponent>(mainCameraEntity).value));
	mStack.translate(-em.GetComponent<PositionComponent>(mainCameraEntity).value);

	glUniformMatrix4fv(sharedPerspectiveMatrixUnif, 1, GL_FALSE, &renderSystem.mainCamera->matrix[0][0]);
	glUniformMatrix4fv(sharedPositionMatrixUnif, 1, GL_FALSE, &mStack.top()[0][0]);

	// Draw lines
	{
		GLsizei byteSize = (GLsizei)(lineVertices.size() * sizeof(float));

		float* lineVertArray = new float[lineVertices.size()];
		std::copy(lineVertices.begin(), lineVertices.end(), lineVertArray);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, byteSize, lineVertArray, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_LINES, 0, (GLsizei)(lineVertices.size() / 3));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] lineVertArray;
	}

	lineVertices.clear();
}
