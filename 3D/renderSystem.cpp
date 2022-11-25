#include <renderSystem.h>

#include <iostream>

RenderSystem::RenderSystem()
{
	mainCamera = nullptr;
}

RenderSystem::RenderSystem(CameraComponent* mainCamera)
{
	RenderSystem::mainCamera = mainCamera;

	initMainCameraMatrix();
}

RenderSystem::~RenderSystem()
{
	// TODO: This is temporary
	// Theoretically the entityManager should delete the main camera when it deletes all entities
	delete[] mainCamera;
}

void RenderSystem::initMainCameraMatrix()
{
	float* matrix = calcPerspectiveMatrix();
	memcpy(RenderSystem::mainCamera->matrix, matrix, sizeof(matrix));
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

	float matrix[16];
	memcpy(matrix, mainCamera->matrix, sizeof(matrix));
}
