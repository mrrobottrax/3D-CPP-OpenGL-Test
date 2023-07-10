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

const inline int quadVertCount = 6;
const inline float gridQuadPositionArray[] = {
	-1, -1, 0.5f,
	 1,  1, 0.5f,
	-1,  1, 0.5f,

	 1,  1, 0.5f,
	-1, -1, 0.5f,
	 1, -1, 0.5f,
};

inline bool glInit = false;
inline GLuint gridVao;
inline GLuint positionBufferObject;
inline GLuint gridShaderProgram;
inline GLuint screenToWorldMatrixUnif;
inline GLuint unitScreenSizeUnif;
inline GLuint baseGridSizeUnif;

inline float baseGridSize = 1;

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

	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void MouseCallback(GLFWwindow* window, int button, int action, int mods) override;
	void MousePosCallback(GLFWwindow* window, double xPos, double yPos) override;

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
	void ZoomIn();
	void ZoomOut();
};