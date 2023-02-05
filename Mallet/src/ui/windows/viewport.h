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

class Viewport : public MalletWindow
{
public:
	Viewport(ViewportMode);
	~Viewport();

	void Draw(DockingLeaf& leaf, int leafIndex);
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
	CameraComponent* camera;
	RenderSystem* renderSystem;
	GLint viewPosX;
	GLint viewPosY;
	GLsizei viewSizeX;
	GLsizei viewSizeY;

private:
	void CalculateViewportVars(DockingLeaf&, int, int);
};