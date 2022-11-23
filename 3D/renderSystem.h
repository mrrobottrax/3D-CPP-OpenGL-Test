#pragma once

#include <system.h>
#include <cameraComponent.h>

class RenderSystem : public System
{
public:
	RenderSystem();
	~RenderSystem();

	void update() override;

private:
	CameraComponent mainCamera;
};