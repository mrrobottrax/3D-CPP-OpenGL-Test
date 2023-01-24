#include <pch.h>

#include <engine/common/matrixStack.h>

void MatrixStack::push()
{
	mStack.push(glm::mat4(1.0f));
}

void MatrixStack::push(glm::mat4& matrix)
{
	mStack.push(matrix);
}

void MatrixStack::pushCpy()
{
	mStack.push(mStack.top());
}

void MatrixStack::pop()
{
	mStack.pop();
}

glm::mat4& MatrixStack::top()
{
	return mStack.top();
}

void MatrixStack::translate(glm::vec3 offset)
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix[3][0] = offset.x;
	matrix[3][1] = offset.y;
	matrix[3][2] = offset.z;

	applyMatrix(matrix);
}

void MatrixStack::applyMatrix(glm::mat4 matrix)
{
	mStack.top() *= matrix;
}