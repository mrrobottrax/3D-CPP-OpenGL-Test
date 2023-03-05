#pragma once

#include <ui/malletwindow.h>
#include <ui/docking/dockingleaf.h>
#include <memory/entity.h>
#include <components/cameracomponent.h>
#include <systems/rendersystem.h>

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
	 1, -1, 0.5f,
	 1,  1, 0.5f,

	-1, -1, 0.5f,
	 1,  1, 0.5f,
	-1,  1, 0.5f,
};

inline bool glInit = false;
inline GLuint gridVao;
inline GLuint positionBufferObject;
inline GLuint gridShaderProgram;
inline GLuint screenToWorldMatrixUnif;
inline GLuint onePixelDistanceUnif;
inline GLuint baseGridSizeUnif;

inline float baseGridSize = 1;

class Viewport : public MalletWindow
{
public:
	Viewport(ViewportMode);
	~Viewport();

	void Draw(DockingLeaf& leaf, int leafIndex);
	void Draw2DWireframe();
	void Draw3DShaded();
	void OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight);
	void OnSelect(DockingLeaf& leaf);
	void OnDeselect(DockingLeaf& leaf);

	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	void MousePosCallback(GLFWwindow* window, double xPos, double yPos);

public:
	ViewportMode mode;

private:
	Entity cameraEntity;
	FreecamComponent* freeCam;
	CameraComponent* camera;
	RenderSystem* renderSystem;
	GLint viewPosX;
	GLint viewPosY;
	GLsizei viewSizeX;
	GLsizei viewSizeY;

private:
	void CalculateViewportVars(DockingLeaf&, int, int);
	void PanButton(int action);
	void ZoomIn();
	void ZoomOut();
};