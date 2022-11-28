#pragma once

#include <stack>
#include <floats.h>

class MatrixStack
{
public:
	void Push(float*);
	void Pop();
	static float* TranslationMatrix(Float3);

private:
	std::stack<float*> mStack;
};