#include <matrixStack.h>

void MatrixStack::Push(float* matrix)
{
	mStack.push(matrix);
}

void MatrixStack::Pop()
{
	mStack.pop();
}