#include <matrixStack.h>

void MatrixStack::Push(float* matrix)
{
	mStack.push(matrix);
}

void MatrixStack::Pop()
{
	mStack.pop();
}

float* MatrixStack::TranslationMatrix(Float3 offset)
{
	float matrix[16] = {
		1, 0, 0, offset.x,
		0, 1, 0, offset.y,
		0, 0, 1, offset.z,
		0, 0, 0, 1,
	};

	return matrix;
}