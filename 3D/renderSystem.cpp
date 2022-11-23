#include <renderSystem.h>

#include <iostream>

RenderSystem::RenderSystem()
{
	frustumScale = calcFrustumScale(90);

	nearClip = 0.3f;
	farClip = 1000.0f;

	float* matrix = calcPerspectiveMatrix();
	memcpy(mainCamera.matrix, matrix, sizeof(mainCamera.matrix));
	delete[] matrix;
}

RenderSystem::~RenderSystem()
{
}

float RenderSystem::calcFrustumScale(float fov)
{
	return fov;
}

float* RenderSystem::calcPerspectiveMatrix()
{
	float* matrix = new float[16];
	memset(matrix, 0, sizeof(matrix));

	matrix[0] = frustumScale;
	matrix[5] = frustumScale;
	matrix[10] = (farClip + nearClip) / (nearClip - farClip);
	matrix[11] = -1;
	matrix[14] = (2 * farClip * nearClip) / (nearClip - farClip);

	return matrix;
}

void RenderSystem::update()
{
	float matrix[16];
	memcpy(matrix, mainCamera.matrix, sizeof(matrix));
}
