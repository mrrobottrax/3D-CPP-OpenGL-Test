#include "pch.h"

#include <common/matrixstack.h>

void MatrixStack::Push()
{
	mStack.push(glm::mat4(1.0f));
}

void MatrixStack::Push(glm::mat4& matrix)
{
	mStack.push(matrix);
}

void MatrixStack::PushCpy()
{
	mStack.push(mStack.top());
}

void MatrixStack::Pop()
{
	mStack.pop();
}

glm::mat4& MatrixStack::Top()
{
	return mStack.top();
}

void MatrixStack::Translate(glm::vec3 offset)
{
	glm::mat4 matrix = glm::mat4(0);

	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
	matrix[3][3] = 1;

	matrix[3][0] = offset.x;
	matrix[3][1] = offset.y;
	matrix[3][2] = offset.z;

	ApplyMatrix(matrix);
}

void MatrixStack::Scale(glm::vec3 scale)
{
	glm::mat4 matrix = glm::mat4(0);

	matrix[0][0] = scale.x;
	matrix[1][1] = scale.y;
	matrix[2][2] = scale.z;
	matrix[3][3] = 1;

	ApplyMatrix(matrix);
}

void MatrixStack::ApplyMatrix(glm::mat4 matrix)
{
	mStack.top() *= matrix;
}

void MatrixStack::Invert()
{
	mStack.top() = glm::inverse(mStack.top());
}