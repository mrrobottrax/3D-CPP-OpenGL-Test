#pragma once

#include <memory/entity.h>

#include <components/positioncomponent.h>
#include <components/freecamcomponent.h>
#include <components/cameracomponent.h>

#include "malletwindow.h"
#include <input/console.h>

enum ViewportMode
{
	perspective,
	top,
	side,
	front
};

constexpr inline float maxGridSize = 65536.0f;
constexpr inline float minGridSize = 0.0000152587890625f;

constexpr inline float dropDownX = 200;
constexpr inline float dropDownY = 200;

class Viewport : public MalletWindow
{
public:
	Viewport(ViewportMode = perspective);
	~Viewport()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &tex);
	};

	void Init();

	void Draw() override;
	void Draw2DWireframe();
	void Draw3DShaded();

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
	static inline bool toolInputFreeze;

private:
	bool hovered;
	static inline bool panHeld;
	static inline bool freecamming;

	GLuint fbo;
	GLuint rbo;
	GLuint tex;

	static inline bool glInit = false;
	static inline GLuint gridVao;
	static inline GLuint gridShaderProgram;
	static inline GLuint gridPositionBuffer;
	static inline GLuint gridColorUnif;
	static inline GLuint gridSizeUnif;
	static inline GLuint gridOffsetUnif;

	void UpdateSize(GLsizei x, GLsizei y);

	GLint viewPosX;
	GLint viewPosY;
	GLsizei viewSizeX;
	GLsizei viewSizeY;

public:
	Entity cameraEntity;

private:
	void PanButton(int action);
	void ZoomIn(float multiplier = 1);
	void ZoomOut(float multiplier = 1);
};