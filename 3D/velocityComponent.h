#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct VelocityComponent
{
	glm::vec3 linear;
	glm::fquat angular;

	VelocityComponent(float a, float b, float c) : linear(a, b, c), angular(1, 0, 0, 0) {}
	VelocityComponent(float a, float b, float c, float x, float y, float z, float w) : linear(a, b, c), angular(x, y, z, w) {}
};