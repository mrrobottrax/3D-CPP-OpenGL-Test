#pragma once

#include <glm.hpp>

struct ScaleComponent
{
	glm::vec3 value = glm::vec3(1);

	ScaleComponent(float a, float b, float c) : value(a, b, c) {}
};