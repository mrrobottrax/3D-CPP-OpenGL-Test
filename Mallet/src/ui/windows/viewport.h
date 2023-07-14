#pragma once

#include <ui/windows/malletwindow.h>
#include <ui/docking/dockingleaf.h>
#include <memory/entity.h>
#include <components/cameracomponent.h>
#include <rendering/rendersystem.h>
#include <components/freecamcomponent.h>

enum ViewportMode
{
	perspective,
	top,
	side,
	front
};

constexpr inline float maxGridSize = 65536.0f;
constexpr inline float minGridSize = 0.0000152587890625f;

inline float baseGridSize = 1;

inline bool glInit = false;
inline GLuint bgVao;
inline GLuint bgPositionBufferObject;

constexpr inline float quadArray[] = {
	-1, -1, 1,
	 1,  1, 1,
	-1,  1, 1,

	 1,  1, 1,
	-1, -1, 1,
	 1, -1, 1,
};

class Viewport : public MalletWindow
{
public:
	Viewport(ViewportMode);
	~Viewport() {};

	void Draw() override;
	void Draw2DWireframe();
	void Draw3DShaded();
	void OnResize() override;
	void OnSelect() override;
	void OnDeselect() override;

	void KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) override;
	void MouseCallback(GLFWwindow* pWindow, int button, int action, int mods) override;
	void MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos) override;
	void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset) override;
	glm::vec2 GetWorldMousePos2D();
	float GetPixelsPerUnit();

public:
	ViewportMode mode;

private:
	Entity cameraEntity;
	FreecamComponent* freeCam;
	CameraComponent* camera;
	GLint viewPosX;
	GLint viewPosY;
	GLsizei viewSizeX;
	GLsizei viewSizeY;

private:
	void CalculateViewportVars(int, int);
	void PanButton(int action);
	void ZoomIn(float multiplier = 1);
	void ZoomOut(float multiplier = 1);
};