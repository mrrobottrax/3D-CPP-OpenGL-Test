#include "malletpch.h"
#include "viewport3d.h"

#include <inputmanager.h>

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>

#include <components/idcomponent.h>
#include <components/positioncomponent.h>

#include <gl/glutil.h>

Viewport3D::Viewport3D() : cameraEntity()
{
	EntityManager& em = EntityManager::GetInstance();

	Component components[] = {
		Component().init<IdComponent>(),
		Component().init<PositionComponent>(),
		Component().init<VelocityComponent>(),
		Component().init<CameraComponent>(),
		Component().init<RotationComponent>(),
		Component().init<FreecamComponent>(),
	};

	Entity entity = em.AddEntity(EntityArchetype(6, components));
	em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
	em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
	em.GetComponent<CameraComponent>(entity) = { 80.0f, 0.03f, 1000.0f };
	em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
	em.GetComponent<FreecamComponent>(entity) = { false, 6, 40, 20 };

	SystemManager::GetSystem<RenderSystem>()->SetMainCameraEntity(entity);

	cameraEntity = entity;
}

Viewport3D::~Viewport3D()
{
}

void Viewport3D::Draw(DockingLeaf& leaf, int leafIndex)
{
	
}

void Viewport3D::OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight)
{
	RenderSystem* rs = SystemManager::GetSystem<RenderSystem>();

	if (rs == nullptr)
		return;

	rs->UpdateMatrixAspect(leaf.absSize[0], leaf.absSize[1]);

	glViewport((GLint)leaf.absPos[0], windowHeight - (leaf.absPos[1] + leaf.absSize[1]), (GLsizei)leaf.absSize[0], (GLsizei)leaf.absSize[1]);
}

void Viewport3D::OnSelect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	EntityManager& em = EntityManager::GetInstance();
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = true;
}

void Viewport3D::OnDeselect(DockingLeaf& leaf)
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	EntityManager& em = EntityManager::GetInstance();
	FreecamComponent& freeCam = em.GetComponent<FreecamComponent>(cameraEntity);

	freeCam.enabled = false;
}

void Viewport3D::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	InputManager::KeyCallback(key, action);
}

void Viewport3D::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void Viewport3D::MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	
}
