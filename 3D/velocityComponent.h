#pragma once

#include <floats.h>

struct VelocityComponent
{
	Float3 linear;
	Float4 angular;

	VelocityComponent(float a, float b, float c) : linear(a, b, c), angular(0, 0, 0, 0) {}
	VelocityComponent(float a, float b, float c, float x, float y, float z, float w) : linear(a, b, c), angular(x, y, z, w) {}
};