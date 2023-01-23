#pragma once

#include <glm.hpp>

struct PositionComponent
{
	glm::vec3 value;

	PositionComponent(float a, float b, float c) : value(a, b, c) {}
};