#include "pch.h"
#include "debugdraw.h"

#include <components/positioncomponent.h>
#include <common/matrixstack.h>

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>

void DebugDraw::DrawLine(const float start[3], const float end[3], glm::vec3 color)
{
	Vertex vstart = {
		start[0],
		start[1],
		start[2],

		color
	};

	Vertex vend = {
		end[0],
		end[1],
		end[2],

		color
	};

	lineVertices.push_back(vend);
	lineVertices.push_back(vstart);
}

void DebugDraw::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
	DrawLine(&start[0], &end[0], color);
}

void DebugDraw::DrawPlane(glm::vec3 offset, glm::vec3 normal, float dist, float width, float height, glm::vec3 color)
{
	DrawPlane(offset, normal, glm::vec3(0, 1, 0), dist, width, height, color);
}

void DebugDraw::DrawPlane(glm::vec3 offset, glm::vec3 normal, glm::vec3 upHint, float dist, float width, float height, glm::vec3 color)
{
	glm::normalize(normal);

	glm::vec3 rightVec = normal == upHint ? glm::vec3(1, 0, 0) : glm::cross(normal, upHint);
	rightVec = glm::normalize(rightVec);
	glm::vec3 upVec = glm::cross(normal, rightVec);
	upVec = glm::normalize(upVec);

	float sideExtent = width / 2.0f;
	float heightExtent = height / 2.0f;

	glm::vec3 corner1 = offset + normal * dist - rightVec * sideExtent - upVec * heightExtent;
	glm::vec3 corner2 = offset + normal * dist + rightVec * sideExtent + upVec * heightExtent;

	glm::vec3 point1;
	glm::vec3 point2;

	//DrawLine(corner1, corner2, color);

	// Left side
	point1 = corner1;
	point2 = corner1 + upVec * height;

	DrawLine(point1, point2, color);

	// Right side
	point1 = corner2;
	point2 = corner2 - upVec * height;

	DrawLine(point1, point2, color);

	// Top
	point1 = corner2;
	point2 = corner2 - rightVec * width;

	DrawLine(point1, point2, color);

	// Bottom
	point1 = corner1;
	point2 = corner1 + rightVec * width;

	DrawLine(point1, point2, color);
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

	DrawLines();
}

void DebugDraw::DrawLines()
{
	if (lineVertices.size() <= 0)
	{
		lineVertices.clear();
		return;
	}

	GLsizei vertByteSize = (GLsizei)(lineVertices.size() * sizeof(float) * 3);
	GLsizei colortByteSize = (GLsizei)(lineVertices.size() * sizeof(float) * 3);

	float* vertArray = new float[lineVertices.size() * 3];
	float* colorArray = new float[lineVertices.size() * 3];

	// Set arrays
	{
// Getting weird false positive here
#pragma warning( push )
#pragma warning( disable : 6386)
		int i = 0;
		for (auto it = lineVertices.begin(); it != lineVertices.end(); ++it)
		{
			vertArray[i]     = (*it).position[0];
			vertArray[i + 1] = (*it).position[1];
			vertArray[i + 2] = (*it).position[2];

			colorArray[i]     = (*it).color[2];
			colorArray[i + 1] = (*it).color[1];
			colorArray[i + 2] = (*it).color[0];

			i += 3;
		}
#pragma warning( pop ) 
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertByteSize, vertArray, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colortByteSize, colorArray, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, (GLsizei)lineVertices.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertArray;
	delete[] colorArray;
	lineVertices.clear();
}
