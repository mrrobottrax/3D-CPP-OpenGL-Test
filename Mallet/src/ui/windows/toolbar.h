#pragma once

#include <ui/windows/malletwindow.h>

enum MalletTool
{
	tool_select,
	tool_block,
};

class Toolbar : public MalletWindow
{
public:
	Toolbar()
	{};

	~Toolbar() {};

public:
	static inline MalletTool activeTool = tool_select;

public:
	void Draw() override;
	void SetActiveTool(MalletTool tool)
	{
		activeTool = tool;
	};
};