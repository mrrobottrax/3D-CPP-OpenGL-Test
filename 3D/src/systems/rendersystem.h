#pragma once

#include <systems/system.h>
#include <components/cameracomponent.h>

extern Cvar r_draw;

class RenderSystem : public System
{
public:
	RenderSystem() : pMainCamera(), autoDraw(true)
	{};

	~RenderSystem()
	{};

public:
	void Init() override
	{
		console.AddCvar(r_draw);
	}

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
}; inline RenderSystem renderSystem;