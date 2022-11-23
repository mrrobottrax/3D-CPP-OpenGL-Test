#pragma once

#include <system.h>
#include <cameraComponent.h>

class RenderSystem : public System
{
public:
	RenderSystem();
	~RenderSystem();

	void update() override;

	float calcFrustumScale(float fov);
	float* calcPerspectiveMatrix();

	CameraComponent mainCamera;
	float frustumScale, nearClip, farClip;

private:
	
};