#pragma once

#include <ui/windows/malletwindow.h>

enum MalletTool
{
	tool_select,
	tool_box,
};

class Toolbar : public MalletWindow
{
public:
	Toolbar() {};
	~Toolbar() {};

private:
	MalletTool tool;

public:
	void Draw() override;
	void SetActiveTool(MalletTool tool)
	{
		this->tool = tool;
	};
};