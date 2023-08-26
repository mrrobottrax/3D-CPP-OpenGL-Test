#pragma once

#include <systems/system.h>
#include <components/cameracomponent.h>

extern Cvar r_draw;

class RenderSystem : public System
{
public:
	RenderSystem() : autoDraw(true)
	{};

	~RenderSystem()
	{};

private:
	bool mainCameraHasBeenSet = false;

public:
	void Init() override
	{
		console.AddCvar(r_draw);
	}

	bool HasMainCamera()
	{
		return mainCameraHasBeenSet;
	}

	bool autoDraw;

	Entity mainCameraEntity;

	void SetMainCameraEntity(const Entity& entity)
	{
		mainCameraEntity = entity;
		mainCameraHasBeenSet = true;
	}

	void ClearMainCameraEntity()
	{
		mainCameraHasBeenSet = false;
	}

	static void CalcFrustumScale(CameraComponent&, const float fov);
	static void CalcPerspectiveMatrix(CameraComponent&, const int width, const int height);
	static void UpdateMatrixAspect(CameraComponent&, const int width, const int height);
	static void ClearScreen()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	void Update() override;
	void DrawBase(const Entity& entity);
	void DrawShaded(const Entity& entity);
	void DrawWireframe(const Entity& entity);
}; inline RenderSystem renderSystem;