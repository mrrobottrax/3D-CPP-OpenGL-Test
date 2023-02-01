#include "malletpch.h"
#include "viewport3d.h"

#include <inputmanager.h>

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>
#include <gl/glutil.h>

void Viewport3D::Draw(DockingLeaf& leaf, int leafIndex)
{
	
}

void Viewport3D::OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight)
{
	RenderSystem* rs = SystemManager::GetSystem<RenderSystem>();

	if (rs == nullptr)
		return;

	rs->UpdateMatrixAspect(leaf.absSize[0], leaf.absSize[1]);

	glViewport((GLint)leaf.absPos[0], windowHeight - (leaf.absPos[1] + leaf.absSize[1]), (GLsizei)leaf.absSize[0], (GLsizei)leaf.absSize[1]);
}

void Viewport3D::OnSelect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Viewport3D::OnDeselect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Viewport3D::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	InputManager::KeyCallback(key, action);
}

void Viewport3D::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void Viewport3D::MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	
}
