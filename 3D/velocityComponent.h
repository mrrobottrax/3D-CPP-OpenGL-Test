#pragma once

#include <floats.h>

struct VelocityComponent
{
	Float3 linear;

	VelocityComponent(float a, float b, float c) : linear(a, b, c) {}
};