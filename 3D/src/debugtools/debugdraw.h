#pragma once

#include <gl/glutil.h>

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
		glm::vec3 color;
	};

	std::list<Vertex> lineVertices;

	void Draw();
	void DrawLines();

public:
	void Init()
	{
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &colorBuffer);
	};

	void Update();
	
	void DrawLine(const float start[3], const float end[3], glm::vec3 color = { 0, 1, 1 });
	void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color = { 0, 1, 1 });

	void DrawPlane(glm::vec3 offset, glm::vec3 normal, glm::vec3 upHint, float dist, float width, float height, glm::vec3 color = { 0, 1, 1 });
	void DrawPlane(glm::vec3 offset, glm::vec3 normal, float dist, float width, float height, glm::vec3 color = { 0, 1, 1 });
}; inline DebugDraw debugDraw;