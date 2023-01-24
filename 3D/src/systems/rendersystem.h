#pragma once

#include <systems/system.h>
#include <components/cameraComponent.h>
#include <memory/entity.h>

class RenderSystem : public System
{
public:
	RenderSystem(Entity&);
	RenderSystem();
	~RenderSystem();

	CameraComponent* mainCamera;
	Entity mainCameraEntity;

	void SetMainCameraEntity(Entity&);
	void InitMainCameraMatrix();
	float CalcFrustumScale(float fov);
	glm::mat4 CalcPerspectiveMatrix();
	void UpdateMatrixAspect(int width, int height);

	void Update() override;

private:
	
};