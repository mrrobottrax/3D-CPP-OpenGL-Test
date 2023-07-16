#include "pch.h"
#include "debugdraw.h"

#include "rendersystem.h"

#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>

#include <common/matrixstack.h>
#include <gl/glutil.h>
#include <common/types.h>

void DebugDraw::DrawLine(const float start[3], const float end[3], glm::vec3 color, float time)
{
	Vertex vstart = {
		start[0],
		start[1],
		start[2],
	};

	Vertex vend = {
		end[0],
		end[1],
		end[2],
	};

	Line line = {
		vstart,
		vend,

		color,
		time
	};

	std::lock_guard<std::mutex> guard(linesMutex);
	lines.push_back(line);
}

void DebugDraw::DrawLine(const glm::vec3 start, const glm::vec3 end, const glm::vec3 color, const float time)
{
	DrawLine(&start[0], &end[0], color, time);
}

void DebugDraw::DrawPlane(const glm::vec3 offset, const glm::vec3 normal, const float dist,
	const float width, const float height, const glm::vec3 color, const float time)
{
	DrawPlane(offset, normal, glm::vec3(0, 1, 0), dist, width, height, color, time);
}

void DebugDraw::DrawPlane(const glm::vec3 offset, const gmath::Plane plane, const float width, const float height, const glm::vec3 color, float time)
{
	DrawPlane(offset, plane.normal, plane.dist, width, height, color, time);
}

void DebugDraw::DrawPlane(const glm::vec3 offset, const glm::vec3 normal, const glm::vec3 upHint, const float dist,
	const float width, const float height, const glm::vec3 color, const float time)
{
	glm::normalize(normal);

	glm::vec3 rightVec = abs(normal) == abs(upHint) ? glm::vec3(1, 0, 0) : glm::cross(normal, upHint);
	rightVec = glm::normalize(rightVec);
	glm::vec3 upVec = glm::cross(normal, rightVec);
	upVec = glm::normalize(upVec);

	float sideExtent = width / 2.0f;
	float heightExtent = height / 2.0f;

	glm::vec3 corner1 = offset + normal * dist - rightVec * sideExtent - upVec * heightExtent;
	glm::vec3 corner2 = offset + normal * dist + rightVec * sideExtent + upVec * heightExtent;

	glm::vec3 point1;
	glm::vec3 point2;

	DrawLine(offset + normal * dist, offset + normal * (dist + 0.1f), color, time);

	// Left side
	point1 = corner1;
	point2 = corner1 + upVec * height;

	DrawLine(point1, point2, color, time);

	// Right side
	point1 = corner2;
	point2 = corner2 - upVec * height;

	DrawLine(point1, point2, color, time);

	// Top
	point1 = corner2;
	point2 = corner2 - rightVec * width;

	DrawLine(point1, point2, color, time);

	// Bottom
	point1 = corner1;
	point2 = corner1 + rightVec * width;

	DrawLine(point1, point2, color, time);
}

void DebugDraw::Render()
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

void DebugDraw::Update()
{
	// Tick timers
	auto it = lines.begin();
	while (it != lines.end())
	{
		// Remove expired lines
		if ((*it).timer < 0)
		{
			lines.erase(it++);
		}
		else
		{
			(*it).timer -= timeManager.GetDeltaTime();
			++it;
		}
	}
}

void DebugDraw::Draw()
{
	if (lines.size() <= 0)
	{
		return;
	}

	EntityManager& em = entityManager;

	// Camera
	MatrixStack mStack;

	Entity& mainCameraEntity = renderSystem.mainCameraEntity;
	mStack.Push();
	mStack.ApplyMatrix(glm::mat4_cast(em.GetComponent<RotationComponent>(mainCameraEntity).value));
	mStack.Translate(-em.GetComponent<PositionComponent>(mainCameraEntity).value);

	glUniformMatrix4fv(sharedPerspectiveMatrixUnif, 1, GL_FALSE, &renderSystem.pMainCamera->matrix[0][0]);
	glUniformMatrix4fv(sharedPositionMatrixUnif, 1, GL_FALSE, &mStack.Top()[0][0]);

	DrawLines();
}

void DebugDraw::DrawLines()
{
	std::lock_guard<std::mutex> guard(linesMutex);

	if (lines.size() <= 0)
	{
		return;
	}

	size_t vertCount = lines.size() * 2;
	size_t vertFloatCount = vertCount * 3;
	size_t colorFloatCount = vertCount * 3;

	float* vertArray = new float[vertFloatCount];
	float* colorArray = new float[colorFloatCount];
	GLsizei vertByteSize = (GLsizei)(sizeof(float) * vertFloatCount);
	GLsizei colortByteSize = (GLsizei)(sizeof(float) * colorFloatCount);

	// Set arrays
	{
		gSize_t i = 0;
		auto it = lines.begin();
		while (it != lines.end())
		{
			assert(i + 5 < vertFloatCount);

			vertArray[i] = (*it).pointA.position[0];
			vertArray[i + 1] = (*it).pointA.position[1];
			vertArray[i + 2] = (*it).pointA.position[2];

			vertArray[i + 3] = (*it).pointB.position[0];
			vertArray[i + 4] = (*it).pointB.position[1];
			vertArray[i + 5] = (*it).pointB.position[2];

			colorArray[i] = (*it).color[0];
			colorArray[i + 1] = (*it).color[1];
			colorArray[i + 2] = (*it).color[2];

			colorArray[i + 3] = (*it).color[0];
			colorArray[i + 4] = (*it).color[1];
			colorArray[i + 5] = (*it).color[2];

			i += 6;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertByteSize, vertArray, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colortByteSize, colorArray, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, (GLsizei)vertCount);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertArray;
	delete[] colorArray;
}
