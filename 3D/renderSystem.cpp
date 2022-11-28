#include <renderSystem.h>

#include <iostream>
#include <matrixStack.h>
#include <entityManager.h>

RenderSystem::RenderSystem(Entity& mainEntity)
{
	RenderSystem::mainCamera = &EntityManager::getInstance().getComponent<CameraComponent>(mainEntity);

	initMainCameraMatrix();
}

RenderSystem::RenderSystem()
{
	mainCamera = nullptr;
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::initMainCameraMatrix()
{
	float* matrix = calcPerspectiveMatrix();

	for (int i = 0; i < 16; i++)
	{
		mainCamera->matrix[i] = matrix[i];
	}

	delete[] matrix;
}

float RenderSystem::calcFrustumScale(float fov)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fovRad = fov * degToRad;
	return 1.0f / tan(fovRad / 2.0f);
}

void RenderSystem::updateMatrixAspect(int width, int height)
{
	if (mainCamera == nullptr)
		return;

	mainCamera->matrix[0] = mainCamera->frustumScale / (width / (float)height);
	mainCamera->matrix[5] = mainCamera->frustumScale;
}

float* RenderSystem::calcPerspectiveMatrix()
{
	float* matrix = new float[16];
	memset(matrix, 0, sizeof(matrix));

	matrix[0] = mainCamera->frustumScale;
	matrix[5] = mainCamera->frustumScale;
	matrix[10] = (mainCamera->farClip + mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);
	matrix[11] = -1;
	matrix[14] = (2 * mainCamera->farClip * mainCamera->nearClip) / (mainCamera->nearClip - mainCamera->farClip);

	return matrix;
}

void RenderSystem::update()
{
	if (mainCamera == nullptr)
	{
		return;
	}

	MatrixStack mStack;

	mStack.Push(mainCamera->matrix);

	//TODO: Draw frame

	mStack.Pop();
}
