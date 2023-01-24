#pragma once

struct RotationComponent
{
	glm::fquat value;

	RotationComponent(float a, float b, float c, float d) : value(a, b, c, d) {}
};