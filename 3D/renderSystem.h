#pragma once

#include <system.h>
#include <cameraComponent.h>
#include <entity.h>

class RenderSystem : public System
{
public:
	RenderSystem(Entity&);
	RenderSystem();
	~RenderSystem();

	CameraComponent* mainCamera;

	void initMainCameraMatrix();
	float calcFrustumScale(float fov);
	float* calcPerspectiveMatrix();
	void updateMatrixAspect(int width, int height);

	void update() override;

private:
	
};