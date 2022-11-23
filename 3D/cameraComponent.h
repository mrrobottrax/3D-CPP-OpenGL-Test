#pragma once

#include <component.h>

struct CameraComponent : Component
{
	float fov = 70;

	float matrix[16];
};