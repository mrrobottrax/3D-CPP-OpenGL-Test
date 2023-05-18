#pragma once

#include <systems/system.h>
#include <components/cameraComponent.h>
#include <memory/entity.h>
#include <debugtools/debugdraw.h>
#include <input/console.h>

extern Cvar r_draw;

class RenderSystem : public System
{
public:
	RenderSystem() : pMainCamera(), autoDraw(true)
	{
		console.AddCvar(r_draw);

		debugDraw.Init();
	};
	~RenderSystem() {};

	bool autoDraw;

	CameraComponent* pMainCamera;
	Entity mainCameraEntity;

	void SetMainCameraEntity(Entity&);

	static void CalcFrustumScale(CameraComponent&, const float fov);
	static void CalcPerspectiveMatrix(CameraComponent&, const int width, const int height);
	static void UpdateMatrixAspect(CameraComponent&, const int width, const int height);

	void Update() override;
	void DrawBase();
	void DrawShaded();
	void DrawWireframe();
};