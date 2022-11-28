#pragma once

#include <floats.h>

struct PositionComponent
{
	Float3 value;

	PositionComponent(float a, float b, float c) : value(a, b, c) {}
};