#pragma once

struct CameraComponent
{
	bool ortho;

	float frustumScale;
	float nearClip;
	float farClip;

	glm::mat4 matrix;

	CameraComponent(float nearClip, float farClip, bool ortho = false, float frustumScale = 1) :
		nearClip(nearClip), farClip(farClip), ortho(ortho), frustumScale(frustumScale)
	{
		matrix = glm::mat4(1.0f);
	}
};