#pragma once

#include <gl/glutil.h>

#include <common/math.h>

class DebugDraw
{
public:
	DebugDraw() : vertexBuffer(), colorBuffer()
	{
	}

	~DebugDraw()
	{
	}

private:
	GLuint vertexBuffer;
	GLuint colorBuffer;

	struct Vertex
	{
		float position[3];
	};

	struct Line
	{
		Vertex pointA;
		Vertex pointB;

		glm::vec3 color;
		float timer;
	};

	std::list<Line> lines;

	void Draw();
	void DrawLines();

public:
	void Init()
	{
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &colorBuffer);
	};

	void Update();
	
	void DrawLine(const float start[3], const float end[3], const glm::vec3 color = { 0, 1, 1 }, const float time = 0);
	void DrawLine(const glm::vec3 start, const glm::vec3 end, const glm::vec3 color = { 0, 1, 1 }, const float time = 0);

	void DrawPlane(const glm::vec3 offset, const Plane plane, const float width, const float height, const glm::vec3 color = { 0, 1, 1 }, float time = 0);
	void DrawPlane(const glm::vec3 offset, const glm::vec3 normal, const glm::vec3 upHint, const float dist, const float width, const float height, const glm::vec3 color = { 0, 1, 1 }, float time = 0);
	void DrawPlane(const glm::vec3 offset, const glm::vec3 normal, const float dist, const float width, const float height, const glm::vec3 color = { 0, 1, 1 }, float time = 0);
}; inline DebugDraw debugDraw;