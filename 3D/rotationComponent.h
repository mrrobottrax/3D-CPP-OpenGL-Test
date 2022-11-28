#pragma once

#include <floats.h>

struct RotationComponent
{
	Float4 value;

	RotationComponent(float a, float b, float c, float d) : value(a, b, c, d) {}
};