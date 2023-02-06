#include "malletpch.h"
#include "viewport.h"

#include <inputmanager.h>

#include <managers.h>
#include <systems/rendersystem.h>

#include <components/idcomponent.h>
#include <components/positioncomponent.h>

#include <gl/glutil.h>

#include <main.h>

#include <common/matrixstack.h>
#include <components/meshcomponent.h>

Viewport::Viewport(ViewportMode mode) : cameraEntity(), viewPosX(), viewPosY(), viewSizeX(), viewSizeY(), mode(mode)
{
	EntityManager& em = *entityManager;

	Component components[] = {
		Component().init<IdComponent>(),
		Component().init<PositionComponent>(),
		Component().init<VelocityComponent>(),
		Component().init<CameraComponent>(),
		Component().init<RotationComponent>(),
		Component().init<FreecamComponent>(),
	};

	Entity entity = em.AddEntity(EntityArchetype(6, components));
	em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
	
	bool ortho = mode != ViewportMode::perspective;
	
	em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20, false, ortho };

	if (!ortho)
	{
		em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(60.0f, 0.03f, 1000.0f);
		cameraEntity = entity;
		camera = &cam;

		int w, h;
		glfwGetWindowSize(mainWindow, &w, &h);
		RenderSystem::CalcFrustumScale(cam);
		RenderSystem::CalcPerspectiveMatrix(cam, w, h);
	}
	else
	{
		em.GetComponent<PositionComponent>(entity) = { 0, 0, 0 };

		switch (mode)
		{
		case top:
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0.7071068f, 0, 0 };
			break;
		case side:
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			break;
		case front:
			em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
			break;
		default:
			em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
			break;
		};

		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(60.0f, 0.03f, 1000.0f, true, 1 / 10.0f);
		cameraEntity = entity;
		camera = &cam;

		int w, h;
		glfwGetWindowSize(mainWindow, &w, &h);
		RenderSystem::CalcPerspectiveMatrix(cam, w, h);
	}

	renderSystem = &systemManager->GetSystem<RenderSystem>();
}

Viewport::~Viewport()
{
}

void Viewport::Draw(DockingLeaf& leaf, int leafIndex)
{
	glViewport(viewPosX, viewPosY, viewSizeX, viewSizeY);

	renderSystem->mainCamera = camera;
	renderSystem->mainCameraEntity = cameraEntity;

	switch (mode)
	{
	case perspective:
		renderSystem->DrawShaded();
		break;
	case top:
		renderSystem->DrawWireframe();
		break;
	case side:
		renderSystem->DrawWireframe();
		break;
	case front:
		renderSystem->DrawWireframe();
		break;
	default:
		renderSystem->DrawShaded();
		break;
	};
}

void Viewport::CalculateViewportVars(DockingLeaf& leaf, int fullWindowWidth, int fullWindowHeight)
{
	viewPosX  = (GLint)leaf.absPos[0];
	viewPosY  = (GLint)(fullWindowHeight - (leaf.absPos[1] + leaf.absSize[1]));
	viewSizeX = (GLsizei)leaf.absSize[0];
	viewSizeY = (GLsizei)leaf.absSize[1];
}

void Viewport::OnResize(DockingLeaf& leaf, int fullWindowWidth, int fullWindowHeight)
{
	CalculateViewportVars(leaf, fullWindowWidth, fullWindowHeight);

	RenderSystem::UpdateMatrixAspect(*camera, leaf.absSize[0], leaf.absSize[1]);
}

void Viewport::OnSelect(DockingLeaf& leaf)
{
	
}

void Viewport::OnDeselect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	EntityManager& em = *entityManager;
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = false;
}

void Viewport::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		EntityManager& em = *entityManager;
		FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

		freeCam.enabled = !freeCam.enabled;

		glfwSetInputMode(mainWindow, GLFW_CURSOR, freeCam.enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

bool cameraWasEnabledBeforePanning = false;

void Viewport::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Panning
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		EntityManager& em = *entityManager;
		FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

		if (action == GLFW_PRESS)
		{
			cameraWasEnabledBeforePanning = freeCam.enabled;

			freeCam.panning = true;

			freeCam.enabled = true;
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (action == GLFW_RELEASE)
		{
			freeCam.panning = false;

			if (!cameraWasEnabledBeforePanning)
			{
				freeCam.enabled = false;
				glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}
}

void Viewport::MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	
}
