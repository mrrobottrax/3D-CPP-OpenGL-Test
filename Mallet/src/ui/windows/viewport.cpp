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
viewSizeX(), viewSizeY(), mode(mode)
{
	Init();
}

void Viewport::UpdateSize(GLsizei x, GLsizei y)
{
	if (x == viewSizeX && y == viewSizeY)
		return;

	viewSizeX = x;
	viewSizeY = y;

	EntityPointer ep = entityManager.GetEntityPointer(cameraEntity);
	FreecamComponent& freeCam = entityManager.GetComponent<FreecamComponent>(ep);
	CameraComponent& cam = entityManager.GetComponent<CameraComponent>(ep);

	freeCam.viewPortSize[0] = viewSizeX;
	freeCam.viewPortSize[1] = viewSizeY;

	RenderSystem::UpdateMatrixAspect(cam, viewSizeX, viewSizeY);

	// Texture
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewSizeX, viewSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewSizeX, viewSizeY);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Viewport::Init()
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

	cameraEntity = em.AddEntity(EntityArchetype(6, components));
	EntityPointer entity = em.GetEntityPointer(cameraEntity);
	em.GetComponent<UnscaledVelocityComponent>(entity) = { { 0, 0, 0, 0, 0, 0 } };

	bool ortho = mode != ViewportMode::perspective;

	em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20, false, ortho };

	if (!ortho)
	{
		em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		auto& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(0.03f, 1000.0f);

		RenderSystem::CalcFrustumScale(cam, 60);
		RenderSystem::CalcPerspectiveMatrix(cam, viewSizeX, viewSizeY);
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

		auto& cam = em.GetComponent<CameraComponent>(entity) = CameraComponent(0.03f, 1000.0f, true, 1 / 10.0f);
		RenderSystem::CalcPerspectiveMatrix(cam, viewSizeX, viewSizeY);
	}

	// Gen texture buffer
	glGenTextures(1, &tex);

	// Gen renderbuffer
	glGenRenderbuffers(1, &rbo);

	UpdateSize(1, 1);

	// Gen framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	MalletWindowBeginAndPop(Viewport, 1);

	ImGui::BeginChild("View", ImVec2(0, 0), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 wsize = ImGui::GetWindowSize();
	ImVec2 wpos = ImGui::GetWindowPos();
	viewPosX = static_cast<GLint>(wpos.x);
	viewPosY = static_cast<GLint>(wpos.y);
	UpdateSize(static_cast<GLsizei>(wsize.x), static_cast<GLsizei>(wsize.y));

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, viewSizeX, viewSizeY);

	RenderSystem::ClearScreen();

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tex)), 
		ImVec2(static_cast<float>(viewSizeX), static_cast<float>(viewSizeY)), ImVec2(0, 1), ImVec2(1, 0));

	if (!freecamming && !panHeld && !toolInputFreeze)
		hovered = ImGui::IsItemHovered();

	ImGui::SetCursorScreenPos(wpos);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
	ImGui::BeginChild("Dropdown", ImVec2(dropDownX, dropDownY), false, ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoBackground);

	// Hack to prevent input blocking
	if (!freecamming && !panHeld && !toolInputFreeze)
		hovered |= ImGui::IsWindowHovered();

	if (glfwGetInputMode(pMainWindow, GLFW_CURSOR) == GLFW_CURSOR_NORMAL
		&& ImGui::IsMouseHoveringRect(wpos, ImVec2(wpos.x + wsize.x, wpos.y + wsize.y), false))
	{
		constexpr char* items[] = { "Perspective", "Top", "Side", "Front" };
		if (ImGui::Combo("##combo", reinterpret_cast<int*>(&mode), items, IM_ARRAYSIZE(items)))
		{
			Init();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar(1);

	ImGui::EndChild();

	ImGui::End();

	ImGui::PopStyleVar(1);
}

void Viewport::Draw2DWireframe()
{
	renderSystem.DrawWireframe(cameraEntity);

	// Grid
	glUseProgram(gridShaderProgram);
	glBindVertexArray(gridVao);

	const float ppu = GetPixelsPerUnit();

	// Keep grid size at least 8px, but also a power of 2
	float minVisibleGridSize = ppu * baseGridSize;
	while (minVisibleGridSize < 8)
	{
		minVisibleGridSize *= 2;
	}

	glUniform3f(gridColorUnif, 0.15f, 0.15f, 0.15f);
	glUniform1f(gridSizeUnif, minVisibleGridSize);

	glm::vec2 offset = Get2DPosition();
	offset *= ppu;
	offset.x -= viewSizeX * 0.5f;
	offset.y -= viewSizeY * 0.5f;

	glUniform2f(gridOffsetUnif, offset.x, offset.y);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);

	Toolbar::DrawActiveTool(this);
}

void Viewport::Draw3DShaded()
{
	renderSystem.DrawShaded(cameraEntity);

	Toolbar::DrawActiveTool(this);
}

void Viewport::PanButton(int action)
{
	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(ep);

	if (action == GLFW_PRESS)
	{
		freeCam.panning = true;
		freeCam.enabled = true;

		inputSystem.SetCursorLoop(true);

		panHeld = true;
	}
	else if (action == GLFW_RELEASE)
	{
		freeCam.panning = false;

		if (!freecamming)
		{
			freeCam.enabled = false;
		}

		inputSystem.SetCursorLoop(false);

		panHeld = false;
	}
}

void Viewport::ZoomIn(float multiplier)
{
	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	CameraComponent& cam = em.GetComponent<CameraComponent>(ep);

	cam.frustumScale *= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(cam, viewSizeX, viewSizeY);
}

void Viewport::ZoomOut(float multiplier)
{
	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	CameraComponent& cam = em.GetComponent<CameraComponent>(ep);

	cam.frustumScale /= 1.1f * abs(multiplier);
	renderSystem.CalcPerspectiveMatrix(cam, viewSizeX, viewSizeY);
}

glm::vec2 Viewport::Get2DPosition()
{
	glm::vec2 pos{};

	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	PositionComponent& position = em.GetComponent<PositionComponent>(ep);

	switch (mode)
	{
	case top:
		pos.x = position.value.x;
		pos.y = -position.value.z;
		break;

	case side:
		pos.x = position.value.z;
		pos.y = position.value.y;
		break;

	case front:
		pos.x = position.value.x;
		pos.y = position.value.y;
		break;

	default:
		return glm::vec2(0);
	}

	return pos;
}

void Viewport::Set2DPosition(glm::vec2 position)
{
	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	PositionComponent& pos = em.GetComponent<PositionComponent>(ep);

	switch (mode)
	{
	case top:
		pos.value.x = position.x;
		pos.value.z = -position.y;
		break;

	case side:
		pos.value.z = position.x;
		pos.value.y = position.y;
		break;

	case front:
		pos.value.x = position.x;
		pos.value.y = position.y;
		break;
	}
}

glm::vec2 Viewport::ScreenToWorld2D(float x, float y)
{
	glm::vec2 worldSpace = glm::vec2(x, y);
	glm::vec2 offset = Get2DPosition();

	worldSpace.x -= viewPosX;
	worldSpace.x -= viewSizeX * 0.5f;

	worldSpace.y -= viewPosY;
	worldSpace.y -= viewSizeY * 0.5f;

	worldSpace.y *= -1;

	worldSpace /= GetPixelsPerUnit();

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
	
	return ScreenToWorld2D((float)cursorX, (float)cursorY);
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
	EntityManager& em = entityManager;
	EntityPointer ep = em.GetEntityPointer(cameraEntity);
	CameraComponent& cam = em.GetComponent<CameraComponent>(ep);

	return cam.frustumScale * viewSizeY * 0.5f;
}

void Viewport::KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	if (!hovered)
		return;

	// 3D look
	if (key == GLFW_KEY_Z && action == GLFW_PRESS && mode == ViewportMode::perspective)
	{
		EntityManager& em = entityManager;
		FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(em.GetEntityPointer(cameraEntity));

		freeCam.enabled = !freeCam.enabled;
		freecamming = freeCam.enabled;

		if (freeCam.enabled)
			InputSystem::DisableMouse();
		else
			InputSystem::EnableMouse();
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

	// Tool
	Toolbar::KeyboardCallback(this, pWindow, key, scancode, action, mods);
}

void Viewport::MouseCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
	if (!hovered)
		return;

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
	if (!hovered)
		return;

	Toolbar::MousePosCallback(this, pWindow, xPos, yPos);
}

void Viewport::ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset)
{
	if (!hovered)
		return;

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