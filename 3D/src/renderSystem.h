#pragma once

#include <system.h>
#include <cameraComponent.h>
#include <entity.h>
#include <glm/glm.hpp>

class RenderSystem : public System
{
public:
	RenderSystem(Entity&);
	RenderSystem();
	~RenderSystem();

	CameraComponent* mainCamera;
	Entity mainCameraEntity;

	void setMainCameraEntity(Entity&);
	void initMainCameraMatrix();
	float calcFrustumScale(float fov);
	glm::mat4 calcPerspectiveMatrix();
	void updateMatrixAspect(int width, int height);

	void update() override;

private:
	
};