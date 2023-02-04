#pragma once

struct CameraComponent
{
	float fov = 70;
	float frustumScale = 1.0f;
	float nearClip = 0.03f, farClip = 1000.0f;

	glm::mat4 matrix;

	CameraComponent(float _fov, float _nearClip, float _farClip) : fov(_fov), nearClip(_nearClip), farClip(_farClip)
	{
		matrix = glm::mat4(1.0f);
	}
};