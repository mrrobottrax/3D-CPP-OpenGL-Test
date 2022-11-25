#pragma once

#include <stack>

class MatrixStack
{
public:
	void Push(float*);
	void Pop();

private:
	std::stack<float*> mStack;
};