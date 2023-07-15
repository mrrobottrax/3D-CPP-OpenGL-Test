#pragma once
#include "malletwindow.h"
#include <ui/tools/mallettool.h>
#include <ui/tools/blocktool.h>

class Toolbar : public MalletWindow
{
public:
	Toolbar()
	{};

	~Toolbar() {};

public:
	static inline MalletTool* pActiveTool;
	static inline BlockTool blockTool = BlockTool();

public:
	void Draw() override;
	static inline void SetActiveTool(MalletTool* pTool)
	{
		pActiveTool = pTool;
	};

	static inline void DrawActiveTool(Viewport* pMalletWindow)
	{
		if (pActiveTool)
			pActiveTool->Draw(pMalletWindow);
	};

	static inline void KeyboardCallback(Viewport* pViewport, GLFWwindow* pWindow, int key, int scancode, int action, int mods)
	{
		if (pActiveTool)
			pActiveTool->KeyboardCallback(pViewport, pWindow, key, scancode, action, mods);
	};

	static inline void MouseCallback(Viewport* pViewport, GLFWwindow* pWindow, int button, int action, int mods)
	{
		if (pActiveTool)
			pActiveTool->MouseCallback(pViewport, pWindow, button, action, mods);
	};

	static inline void MousePosCallback(Viewport* pViewport, GLFWwindow* pWindow, double xPos, double yPos)
	{
		if (pActiveTool)
			pActiveTool->MousePosCallback(pViewport, pWindow, xPos, yPos);
	};

	static inline void ScrollCallback(Viewport* pViewport, GLFWwindow* pWindow, double xOffset, double yOffset)
	{
		if (pActiveTool)
			pActiveTool->ScrollCallback(pViewport, pWindow, xOffset, yOffset);
	};
};