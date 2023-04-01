#pragma once

#include <systems/system.h>
#include <gl/glutil.h>

class DebugDraw : public System
{
public:
	DebugDraw()
	{
		glGenBuffers(1, &vertexBuffer);
	}

	~DebugDraw()
	{
	}

private:
	GLuint vertexBuffer;

	std::list<float> lineVertices;

public:
	void Update() override;
	void Draw();
	
	void DrawLine(const float start[3], const float end[3]);
	void DrawLine(glm::vec3 start, glm::vec3 end);

	void DrawPlane(glm::vec3 offset, glm::vec3 normal, glm::vec3 upHint, float dist, float width, float height);
	void DrawPlane(glm::vec3 offset, glm::vec3 normal, float dist, float width, float height);
};