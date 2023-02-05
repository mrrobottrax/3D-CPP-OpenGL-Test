#pragma once

struct CameraComponent
{
	bool ortho;

	float fov;
	float frustumScale;
	float nearClip;
	float farClip;

	glm::mat4 matrix;

	CameraComponent(float fov, float nearClip, float farClip, bool ortho = false, float frustumScale = 1) :
		fov(fov), nearClip(nearClip), farClip(farClip), ortho(ortho), frustumScale(frustumScale)
	{
		matrix = glm::mat4(1.0f);
	}
};