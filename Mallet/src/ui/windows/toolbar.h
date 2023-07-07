#pragma once

#include <ui/windows/malletwindow.h>
#include <tools/mallettool.h>

class Toolbar : public MalletWindow
{
public:
	Toolbar() {};
	~Toolbar() {};

private:
	//const std::vector<MalletTool> toolArray;

public:
	void Draw() override;
};