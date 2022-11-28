#pragma once

#include <stack>
#include <floats.h>
#include <glm/glm.hpp>

class MatrixStack
{
public:
	void push(glm::mat4&);
	void push();
	void pushCpy();
	void pop();
	void translate(Float3&);
	glm::mat4& top();

private:
	std::stack<glm::mat4> mStack;
};