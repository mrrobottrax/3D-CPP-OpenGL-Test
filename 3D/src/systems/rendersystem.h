#pragma once

#include <systems/system.h>
#include <components/cameraComponent.h>
#include <memory/entity.h>

class RenderSystem : public System
{
public:
	RenderSystem();
	~RenderSystem();

	bool autoDraw;

	CameraComponent* mainCamera;
	Entity mainCameraEntity;

	void SetMainCameraEntity(Entity&);
	void InitMainCameraMatrix(int, int);

	static void CalcFrustumScale(CameraComponent&);
	static void CalcPerspectiveMatrix(CameraComponent&, int, int);
	static void UpdateMatrixAspect(CameraComponent&, int width, int height);

	void Update() override;
	void DrawShaded();
	void DrawWireframe();

private:
	void DrawBase();
};