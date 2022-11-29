#pragma once

#include <glm/glm.hpp>

struct VelocityComponent
{
	glm::vec3 linear;
	glm::vec3 angular;

	VelocityComponent(float a, float b, float c) : linear(a, b, c), angular(0, 0, 0) {}
	VelocityComponent(float a, float b, float c, float x, float y, float z) : linear(a, b, c), angular(x, y, z) {}
};