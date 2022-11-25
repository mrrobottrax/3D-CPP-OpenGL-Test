#pragma once

#include <component.h>

struct CameraComponent : Component
{
	float fov = 70;
	float frustumScale = 1.0f;
	float nearClip = 0.3f, farClip = 1000.0f;

	float matrix[16]{};
};