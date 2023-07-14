#include "malletpch.h"
#include "viewport.h"

#include <input/inputsystem.h>
#include <systems/systemmanager.h>

#include <rendering/rendersystem.h>

#include <components/idcomponent.h>
#include <components/positioncomponent.h>

#include <gl/glutil.h>
#include <gl/shaderloader.h>

#include <main.h>

#include <common/matrixstack.h>
#include <components/meshcomponent.h>
#include <components/rotationcomponent.h>
#include <components/unscaledvelocitycomponent.h>

Viewport::Viewport(ViewportMode mode) : cameraEntity(), viewPosX(), viewPosY(), viewSizeX(), viewSizeY(), mode(mode)
{
	EntityManager& em = entityManager;

	Component components[] = {
		Component().Init<IdComponent>(),
		Component().Init<PositionComponent>(),
		Component().Init<UnscaledVelocityComponent>(),
		Component().Init<CameraComponent>(),
		Component().Init<RotationComponent>(),
		Component().Init<FreecamComponent>(),
	};

	Entity entity = em.AddEntity(EntityArchetype(6, components));
	em.GetComponent<UnscaledVelocityComponent>(entity) = { { 0, 0, 0, 0, 0, 0 } };
	
	bool ortho = mode != ViewportMode::perspective;
	
	FreecamComponent& fc = em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20, false, ortho };
	freeCam = &fc;

	if (!ortho)
	{
		em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(0.03f, 1000.0f);
		cameraEntity = entity;
		camera = &cam;

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);
		RenderSystem::CalcFrustumScale(cam, 60);
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

		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(0.03f, 1000.0f, true, 1 / 10.0f);
		cameraEntity = entity;
		camera = &cam;

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);
		RenderSystem::CalcPerspectiveMatrix(cam, w, h);
	}

	// Init grid overlay
	if (!glInit)
	{
		glInit = true;

		glGenVertexArrays(1, &bgVao);
		glBindVertexArray(bgVao);

		glGenBuffers(1, &bgPositionBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, bgPositionBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadArray), quadArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		std::vector<GLuint> shaderList;
	}
}

void Viewport::Draw()
{
	glViewport(viewPosX, viewPosY, viewSizeX, viewSizeY);

	renderSystem.pMainCamera = camera;
	renderSystem.mainCameraEntity = cameraEntity;

	freeCam->viewPortSize[0] = viewSizeX;
	freeCam->viewPortSize[1] = viewSizeY;

	switch (mode)
	{
	case perspective:
		Draw3DShaded();
		break;
	case top:
		Draw2DWireframe();
		break;
	case side:
		Draw2DWireframe();
		break;
	case front:
		Draw2DWireframe();
		break;
	default:
		Draw3DShaded();
		break;
	};
}

void Viewport::Draw2DWireframe()
{
	renderSystem.DrawWireframe();

	// Background
	glUseProgram(solidShaderProgram);
	glBindVertexArray(bgVao);

	glUniform3f(solidColorUnif, 0, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Viewport::Draw3DShaded()
{
	renderSystem.DrawShaded();
}

void Viewport::CalculateViewportVars(int fullWindowWidth, int fullWindowHeight)
{
	viewPosX  = (GLint)pLeaf->position[0];
	viewPosY  = (GLint)(fullWindowHeight - (pLeaf->position[1] + pLeaf->size[1]));
	viewSizeX = (GLsizei)pLeaf->size[0];
	viewSizeY = (GLsizei)pLeaf->size[1];
}

void Viewport::OnResize()
{
	int fullWindowWidth, fullWindowHeight;

	glfwGetWindowSize(pMainWindow, &fullWindowWidth, &fullWindowHeight);

	CalculateViewportVars(fullWindowWidth, fullWindowHeight);

	RenderSystem::UpdateMatrixAspect(*camera, pLeaf->size[0], pLeaf->size[1]);
}

void Viewport::OnSelect()
{
	
}

void Viewport::OnDeselect()
{
	glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	freeCam->enabled = false;
}

bool cameraWasEnabledBeforePanning = false;
void Viewport::PanButton(int action)
{
	EntityManager& em = entityManager;

	if (action == GLFW_PRESS)
	{
		cameraWasEnabledBeforePanning = freeCam->enabled;

		freeCam->panning = true;
		freeCam->enabled = true;

		inputSystem.SetCursorLoop(true);
	}
	else if (action == GLFW_RELEASE)
	{
		freeCam->panning = false;

		if (!cameraWasEnabledBeforePanning)
		{
			freeCam->enabled = false;
		}

		inputSystem.SetCursorLoop(false);
	}
}

void Viewport::ZoomIn(float multiplier)
{
	camera->frustumScale *= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(*camera, viewSizeX, viewSizeY);
}

void Viewport::ZoomOut(float multiplier)
{
	camera->frustumScale /= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(*camera, viewSizeX, viewSizeY);
}

void Viewport::KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	// 3D look
	if (key == GLFW_KEY_Z && action == GLFW_PRESS && mode == ViewportMode::perspective)
	{
		EntityManager& em = entityManager;
		FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

		freeCam.enabled = !freeCam.enabled;

		glfwSetInputMode(pMainWindow, GLFW_CURSOR, freeCam.enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		return;
	}

	// Panning
	if (key == GLFW_KEY_P || (key == GLFW_KEY_Z && mode != ViewportMode::perspective))
	{
		PanButton(action);
		return;
	}

	// + or - zoom in/out
	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		ZoomIn();
		return;
	}

	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		ZoomOut();
		return;
	}
}

void Viewport::MouseCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
	// Panning
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		PanButton(action);
		return;
	}
}

void Viewport::MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos)
{
	
}

void Viewport::ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset)
{
	if (mode != top && mode != side && mode != front)
	{
		if (yOffset > 0)
			ZoomIn((float)yOffset);
		else
			ZoomOut((float)yOffset);

		return;
	}

	// In 2d modes, zoom into the cursor
	glm::vec2 delta = GetWorldMousePos2D();

	if (yOffset > 0)
		ZoomIn((float)yOffset);
	else
		ZoomOut((float)yOffset);

	delta -= GetWorldMousePos2D();

	PositionComponent& position = entityManager.GetComponent<PositionComponent>(cameraEntity);

	switch (mode)
	{
	case top:
		position.value.x += delta.x;
		position.value.z += delta.y;
		break;

	case side:
		position.value.z += delta.x;
		position.value.y += delta.y;
		break;

	case front:
		position.value.x += delta.x;
		position.value.y += delta.y;
		break;
	}
}

glm::vec2 Viewport::GetWorldMousePos2D()
{
	float pixelsPerUnit = GetPixelsPerUnit();
	double cursorX, cursorY;
	glfwGetCursorPos(pMainWindow, &cursorX, &cursorY);
	int windowHeight;
	glfwGetWindowSize(pMainWindow, NULL, &windowHeight);
	glm::vec2 worldSpace = glm::vec2(cursorX, windowHeight - cursorY);
	glm::vec2 offset{};

	PositionComponent& position = entityManager.GetComponent<PositionComponent>(cameraEntity);

	switch (mode)
	{
	case top:
		offset.x = position.value.x;
		offset.y = position.value.z;
		break;

	case side:
		offset.x = position.value.z;
		offset.y = position.value.y;
		break;

	case front:
		offset.x = position.value.x;
		offset.y = position.value.y;
		break;

	default:
		return glm::vec2(0);
	}

	worldSpace.x -= viewPosX;
	worldSpace.x -= viewSizeX * 0.5f;

	worldSpace.y -= viewPosY;
	worldSpace.y -= viewSizeY * 0.5f;

	worldSpace /= pixelsPerUnit;

	worldSpace += offset;

	// Because we are using left hand coords, top view up is -Z
	if (mode == top)
		worldSpace.y *= -1;

	return worldSpace;
}

float Viewport::GetPixelsPerUnit()
{
	return camera->frustumScale * viewSizeY * 0.5f;
}
