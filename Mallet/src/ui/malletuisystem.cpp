#include "malletpch.h"
#include "malletuisystem.h"

#include <gl/glutil.h>

#include "windows/viewport.h"
#include "windows/toolbar.h"

void MalletUiSystem::Init()
{
	DockingLeaf* leaf  = tree.AddLeaf(new Viewport(perspective), nullptr);
	DockingLeaf* leaf2 = tree.AddLeaf(new Viewport(ViewportMode::top), leaf, vertical, 0.5f);
	tree.AddLeaf(new Toolbar(), leaf, vertical, 0.1f, true);
	tree.AddLeaf(new Viewport(ViewportMode::side), leaf, horizontal, 0.5f);
	tree.AddLeaf(new Viewport(ViewportMode::front), leaf2, horizontal, 0.5f);
	tree.UpdateSize();
}

void MalletUiSystem::Update()
{
	tree.DrawTree();
}

void MalletUiSystem::DeselectNode()
{
	if (!pSelectedNode)
		return;

	if (pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);
		leaf.pWindow->OnDeselect();
	}

	pSelectedNode = nullptr;
}

void MalletUiSystem::SelectNodeUnderMouse(bool selectSplits)
{
	DeselectNode();
	pSelectedNode = tree.GetNodeUnderMouse(selectSplits);

	if (!pSelectedNode)
		return;

	if (pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);
		leaf.pWindow->OnSelect();
	}
	else
	{
		DockingSplit& split = *reinterpret_cast<DockingSplit*>(pSelectedNode);
	}
}

void MalletUiSystem::WindowSizeCallback(GLFWwindow*, int width, int height)
{
	tree.UpdateSize();
}

void MalletUiSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Deselect with escape
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		DeselectNode();
		return;
	}

	// Select when key pressed
	if (action == GLFW_PRESS)
	{
		if (glfwGetInputMode(pMainWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			SelectNodeUnderMouse();
	}

	// Send key presses to window
	if (pSelectedNode && pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);

		if (leaf.pWindow)
			leaf.pWindow->KeyboardCallback(window, key, scancode, action, mods);
	}
}

void MalletUiSystem::MouseCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
	// Select when mouse clicked
	if (action == GLFW_PRESS)
	{
		if (glfwGetInputMode(pMainWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			SelectNodeUnderMouse(true);
	}

	// When selecting a split, deselect on release
	if (action == GLFW_RELEASE && pSelectedNode && !pSelectedNode->isLeaf)
	{
		DeselectNode();
	}

	// Send mouse clicks to window
	if (pSelectedNode && pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);

		if (leaf.pWindow)
			leaf.pWindow->MouseCallback(pWindow, button, action, mods);
	}
}

constexpr int minWindowSize = 30;
void MalletUiSystem::MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos)
{
	// Move split with mouse
	if (pSelectedNode && !pSelectedNode->isLeaf)
	{
		DockingSplit& split = *reinterpret_cast<DockingSplit*>(pSelectedNode);

		switch (split.direction)
		{
		case SplitDirection::horizontal:
		{
			float mousePos = clamp((float)yPos, (float)split.min + minWindowSize, (float)split.max - minWindowSize);
			float ratio = (mousePos - split.min) / (split.max - split.min);
			ratio = clamp<float>(ratio, 0.0f, 1.0f);

			split.splitRatio = ratio;

			break;
		}

		case SplitDirection::vertical:
		{
			float mousePos = clamp((float)xPos, (float)split.min + minWindowSize, (float)split.max - minWindowSize);
			float ratio = (mousePos - split.min) / (split.max - split.min);
			ratio = clamp<float>(ratio, 0.0f, 1.0f);

			split.splitRatio = ratio;

			break;
		}

		default:
			break;
		}

		tree.UpdateSize();
	}

	// Send mouse movement to window
	if (pSelectedNode && pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);

		if (leaf.pWindow)
			leaf.pWindow->MousePosCallback(pWindow, xPos, yPos);
	}
}

void MalletUiSystem::ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset)
{
	if (yOffset != 0)
	{
		if (glfwGetInputMode(pMainWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			SelectNodeUnderMouse();
	}

	// Send to window
	if (pSelectedNode && pSelectedNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pSelectedNode);

		if (leaf.pWindow)
			leaf.pWindow->ScrollCallback(pWindow, xOffset, yOffset);
	}
}