#include "malletpch.h"
#include "viewport.h"

#include <input/inputmanager.h>
#include <systems/systemmanager.h>

#include <systems/rendersystem.h>

#include <components/idcomponent.h>
#include <components/positioncomponent.h>

#include <gl/glutil.h>
#include <gl/shaderloader.h>

#include <main.h>

#include <common/matrixstack.h>
#include <components/meshcomponent.h>

Viewport::Viewport(ViewportMode mode) : cameraEntity(), viewPosX(), viewPosY(), viewSizeX(), viewSizeY(), mode(mode)
{
	EntityManager& em = entityManager;

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
	
	FreecamComponent& fc = em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20, false, ortho };
	freeCam = &fc;

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

	renderSystem = &systemManager.GetSystem<RenderSystem>();

	// Init grid overlay
	if (!glInit)
	{
		glInit = true;

		glGenVertexArrays(1, &gridVao);
		glBindVertexArray(gridVao);

		glGenBuffers(1, &positionBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gridQuadPositionArray), gridQuadPositionArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		std::vector<GLuint> shaderList;

		// Grid shader
		{
			const char* strVertShader = shaderLoader::loadShader("data/shaders/grid.vert");
			const char* strFragShader = shaderLoader::loadShader("data/shaders/grid.frag");
			shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertShader));
			shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragShader));
			delete[] strVertShader;
			delete[] strFragShader;

			gridShaderProgram = CreateProgram(shaderList);
			std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
		}

		screenToWorldMatrixUnif = glGetUniformLocation(gridShaderProgram, "screenToWorldMatrix");
		onePixelDistanceUnif = glGetUniformLocation(gridShaderProgram, "onePixelDistance");
		baseGridSizeUnif = glGetUniformLocation(gridShaderProgram, "baseGridSize");
	}
}

Viewport::~Viewport()
{
}

void Viewport::Draw(DockingLeaf& leaf, int leafIndex)
{
	glViewport(viewPosX, viewPosY, viewSizeX, viewSizeY);

	renderSystem->mainCamera = camera;
	renderSystem->mainCameraEntity = cameraEntity;

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
	renderSystem->DrawWireframe();

	// Grid
	glUseProgram(gridShaderProgram);
	glBindVertexArray(gridVao);

	EntityManager& em = entityManager;
	MatrixStack mStack;
	mStack.push();

	// Get inverse camera matrix
	glm::vec3 pos = em.GetComponent<PositionComponent>(cameraEntity).value;
	glm::vec3 newPos = glm::mat4_cast(em.GetComponent<RotationComponent>(cameraEntity).value) * glm::vec4(pos.x, pos.y, pos.z, 1);
	mStack.applyMatrix(camera->matrix);
	mStack.translate(-newPos);
	mStack.invert();

	glUniformMatrix4fv(screenToWorldMatrixUnif, 1, GL_FALSE, &mStack.top()[0][0]);

	// Get world distance needed to move 1 pixel
	glm::vec4 delta = glm::vec4(1 / (float)viewSizeX, 1 / (float)viewSizeY, 0, 1);
	delta = glm::inverse(camera->matrix) * delta * 2.0f;
	glUniform2f(onePixelDistanceUnif, delta.x, delta.y);

	glUniform1f(baseGridSizeUnif, baseGridSize);

	glDrawArrays(GL_TRIANGLES, 0, quadVertCount);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Viewport::Draw3DShaded()
{
	renderSystem->DrawShaded();
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

	EntityManager& em = entityManager;
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = false;
}

bool cameraWasEnabledBeforePanning = false;
void Viewport::PanButton(int action)
{
	EntityManager& em = entityManager;
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	if (action == GLFW_PRESS)
	{
		cameraWasEnabledBeforePanning = freeCam.enabled;

		freeCam.panning = true;
		freeCam.enabled = true;

		inputManager.SetCursorLoop(true);
	}
	else if (action == GLFW_RELEASE)
	{
		freeCam.panning = false;

		if (!cameraWasEnabledBeforePanning)
		{
			freeCam.enabled = false;
		}

		inputManager.SetCursorLoop(false);
	}
}

void Viewport::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// 3D look
	if (key == GLFW_KEY_Z && action == GLFW_PRESS && mode == ViewportMode::perspective)
	{
		EntityManager& em = entityManager;
		FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

		freeCam.enabled = !freeCam.enabled;

		glfwSetInputMode(mainWindow, GLFW_CURSOR, freeCam.enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		return;
	}

	// Panning
	if (key == GLFW_KEY_P || (key == GLFW_KEY_Z && mode != ViewportMode::perspective))
	{
		PanButton(action);
		return;
	}
}

void Viewport::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Panning
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		PanButton(action);
		return;
	}
}

void Viewport::MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	
}
