#pragma once

#include <ui/windows/malletwindow.h>
#include <ui/docking/dockingleaf.h>
#include <memory/entity.h>
#include <components/cameracomponent.h>
#include <rendering/rendersystem.h>
#include <components/freecamcomponent.h>
#include <components/positioncomponent.h>
#include <ui/tools/blocktool.h>

enum ViewportMode
{
	perspective,
	top,
	side,
	front
};

constexpr inline float maxGridSize = 65536.0f;
constexpr inline float minGridSize = 0.0000152587890625f;

const inline float quadArray[] = {
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
	void DrawBoxTool();
	void OnResize() override;
	void OnSelect() override;
	void OnDeselect() override;

	void KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) override;
	void MouseCallback(GLFWwindow* pWindow, int button, int action, int mods) override;
	void MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos) override;
	void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset) override;
	glm::vec2 Get2DPosition();
	void Set2DPosition(glm::vec2 position);
	glm::vec2 ScreenToWorld2D(float x, float y);
	glm::vec2 GetWorldMousePos2D();
	glm::vec2 GetGridMousePos2D();
	float GetPixelsPerUnit();

public:
	ViewportMode mode;
	static inline float baseGridSize = 1;

private:
	static inline bool glInit = false;
	static inline GLuint gridVao;
	static inline GLuint gridShaderProgram;
	static inline GLuint gridPositionBuffer;
	static inline GLuint gridColorUnif;
	static inline GLuint gridSizeUnif;
	static inline GLuint gridOffsetUnif;

	static inline GLuint boxVao;
	static inline GLuint boxPositionBuffer;

private:
	Entity cameraEntity;
	FreecamComponent* pFreeCam;
	CameraComponent* pCamera;
	PositionComponent* pPosition;
	GLint viewPosX;
	GLint viewPosY;
	GLsizei viewSizeX;
	GLsizei viewSizeY;

private:
	void CalculateViewportVars(int, int);
	void PanButton(int action);
	void BlockTool(int action);
	void ZoomIn(float multiplier = 1);
	void ZoomOut(float multiplier = 1);
};