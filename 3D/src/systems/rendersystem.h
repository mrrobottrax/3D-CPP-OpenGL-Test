#pragma once

#include <systems/system.h>
#include <components/cameraComponent.h>
#include <memory/entity.h>
#include <debugtools/debugdraw.h>

class RenderSystem : public System
{
public:
	RenderSystem() : pMainCamera(), autoDraw(true)
	{
#ifdef DEBUG
		debugDraw.Init();
#endif // DEBUG
	};
	~RenderSystem() {};

	bool autoDraw;

	CameraComponent* pMainCamera;
	Entity mainCameraEntity;

	void SetMainCameraEntity(Entity&);

	static void CalcFrustumScale(CameraComponent&, float fov);
	static void CalcPerspectiveMatrix(CameraComponent&, int width, int height);
	static void UpdateMatrixAspect(CameraComponent&, int width, int height);

	void Update() override;
	void DrawBase();
	void DrawShaded();
	void DrawWireframe();
};