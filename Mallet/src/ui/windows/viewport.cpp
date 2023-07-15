#include "malletpch.h"
#include "viewport.h"

#include <input/inputsystem.h>
#include <systems/systemmanager.h>

#include <rendering/rendersystem.h>

#include <components/idcomponent.h>
#include <components/meshcomponent.h>
#include <components/rotationcomponent.h>
#include <components/unscaledvelocitycomponent.h>

#include <gl/glutil.h>
#include <gl/shaderloader.h>

#include <common/matrixstack.h>

#include "toolbar.h"

Viewport::Viewport(ViewportMode mode) : cameraEntity(), viewPosX(), viewPosY(),
viewSizeX(), viewSizeY(), mode(mode), pPosition()
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
	pFreeCam = &fc;

	if (!ortho)
	{
		PositionComponent& pos = em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		pPosition = &pos;
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(0.03f, 1000.0f);
		cameraEntity = entity;
		pCamera = &cam;

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);
		RenderSystem::CalcFrustumScale(cam, 60);
		RenderSystem::CalcPerspectiveMatrix(cam, w, h);
	}
	else
	{
		PositionComponent& pos = em.GetComponent<PositionComponent>(entity) = { 0, 0, 0 };
		pPosition = &pos;

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
		pCamera = &cam;

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);
		RenderSystem::CalcPerspectiveMatrix(cam, w, h);
	}

	// Init grid overlay
	if (!glInit)
	{
		glInit = true;

		// Grid
		glGenVertexArrays(1, &gridVao);
		glBindVertexArray(gridVao);

		glGenBuffers(1, &gridPositionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gridPositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadArray), quadArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		std::vector<GLuint> shaderList;

		const char* strVertShader = shaderLoader::LoadShader("../data/shaders/grid.vert");
		const char* strFragShader = shaderLoader::LoadShader("../data/shaders/grid.frag");
		shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertShader));
		shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragShader));
		delete[] strVertShader;
		delete[] strFragShader;

		gridShaderProgram = CreateProgram(shaderList);
		std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

		gridColorUnif = glGetUniformLocation(gridShaderProgram, "fillColor");
		gridSizeUnif = glGetUniformLocation(gridShaderProgram, "gridSize");
		gridOffsetUnif = glGetUniformLocation(gridShaderProgram, "offset");
	}
}

void Viewport::Draw()
{
	glViewport(viewPosX, viewPosY, viewSizeX, viewSizeY);

	renderSystem.pMainCamera = pCamera;
	renderSystem.mainCameraEntity = cameraEntity;

	pFreeCam->viewPortSize[0] = viewSizeX;
	pFreeCam->viewPortSize[1] = viewSizeY;

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

	// Grid
	glUseProgram(gridShaderProgram);
	glBindVertexArray(gridVao);

	// Keep grid size at least 8px, but also a power of 2
	float minVisibleGridSize = GetPixelsPerUnit() * baseGridSize;
	while (minVisibleGridSize < 8)
	{
		minVisibleGridSize *= 2;
	}

	glUniform3f(gridColorUnif, 0.15f, 0.15f, 0.15f);
	glUniform1f(gridSizeUnif, minVisibleGridSize);

	glm::vec2 offset = Get2DPosition();
	offset *= GetPixelsPerUnit();
	offset.x -= viewPosX + viewSizeX * 0.5f;
	offset.y -= viewPosY + viewSizeY * 0.5f;

	glUniform2f(gridOffsetUnif, offset.x, offset.y);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);

	Toolbar::DrawActiveTool(this);
}

void Viewport::Draw3DShaded()
{
	renderSystem.DrawShaded();

	Toolbar::DrawActiveTool(this);
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

	RenderSystem::UpdateMatrixAspect(*pCamera, pLeaf->size[0], pLeaf->size[1]);
}

void Viewport::OnSelect()
{
	
}

void Viewport::OnDeselect()
{
	glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	pFreeCam->enabled = false;
}

bool cameraWasEnabledBeforePanning = false;
void Viewport::PanButton(int action)
{
	EntityManager& em = entityManager;

	if (action == GLFW_PRESS)
	{
		cameraWasEnabledBeforePanning = pFreeCam->enabled;

		pFreeCam->panning = true;
		pFreeCam->enabled = true;

		inputSystem.SetCursorLoop(true);
	}
	else if (action == GLFW_RELEASE)
	{
		pFreeCam->panning = false;

		if (!cameraWasEnabledBeforePanning)
		{
			pFreeCam->enabled = false;
		}

		inputSystem.SetCursorLoop(false);
	}
}

void Viewport::ZoomIn(float multiplier)
{
	pCamera->frustumScale *= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(*pCamera, viewSizeX, viewSizeY);
}

void Viewport::ZoomOut(float multiplier)
{
	pCamera->frustumScale /= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(*pCamera, viewSizeX, viewSizeY);
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

	Toolbar::MouseCallback(this, pWindow, button, action, mods);
}

void Viewport::MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos)
{
	Toolbar::MousePosCallback(this, pWindow, xPos, yPos);
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

	// Left handed coords
	if (mode == top)
		delta.y *= -1;

	Set2DPosition(Get2DPosition() + delta);
}

glm::vec2 Viewport::Get2DPosition()
{
	glm::vec2 pos{};

	switch (mode)
	{
	case top:
		pos.x = pPosition->value.x;
		pos.y = -pPosition->value.z;
		break;

	case side:
		pos.x = pPosition->value.z;
		pos.y = pPosition->value.y;
		break;

	case front:
		pos.x = pPosition->value.x;
		pos.y = pPosition->value.y;
		break;

	default:
		return glm::vec2(0);
	}

	return pos;
}

void Viewport::Set2DPosition(glm::vec2 position)
{
	switch (mode)
	{
	case top:
		pPosition->value.x = position.x;
		pPosition->value.z = -position.y;
		break;

	case side:
		pPosition->value.z = position.x;
		pPosition->value.y = position.y;
		break;

	case front:
		pPosition->value.x = position.x;
		pPosition->value.y = position.y;
		break;
	}
}

glm::vec2 Viewport::ScreenToWorld2D(float x, float y)
{
	float pixelsPerUnit = GetPixelsPerUnit();

	glm::vec2 worldSpace = glm::vec2(x, y);
	glm::vec2 offset = Get2DPosition();

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

glm::vec2 Viewport::GetWorldMousePos2D()
{
	double cursorX, cursorY;
	glfwGetCursorPos(pMainWindow, &cursorX, &cursorY);
	int windowHeight;
	glfwGetWindowSize(pMainWindow, NULL, &windowHeight);
	
	return ScreenToWorld2D((float)cursorX, windowHeight - (float)cursorY);
}

glm::vec2 Viewport::GetGridMousePos2D()
{
	glm::vec2 pos = GetWorldMousePos2D();
	pos.x = floorf(pos.x / baseGridSize + 0.5f) * baseGridSize;
	pos.y = floorf(pos.y / baseGridSize + 0.5f) * baseGridSize;

	return pos;
}

float Viewport::GetPixelsPerUnit()
{
	return pCamera->frustumScale * viewSizeY * 0.5f;
}
