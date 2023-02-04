#include <gamepch.h>

#include <main.h>

#include <gl/glutil.h>
#include <input/gameinputlayer.h>

#include <meshobject.h>
#include <memory/entitymanager.h>
#include <timemanager.h>
#include <modelloader.h>

#include <systems/rendersystem.h>
#include <systems/velocitysystem.h>
#include <systems/freecamsystem.h>

#include <components/cameracomponent.h>
#include <components/freecamcomponent.h>
#include <components/rotationcomponent.h>
#include <components/meshcomponent.h>
#include <components/velocitycomponent.h>
#include <components/positioncomponent.h>
#include <components/idcomponent.h>

#include <imgui/imguiutil.h>
#include <inputmanager.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC

// TEMP
// TODO: Remove
// Cube mesh
MeshObject* monkeyMesh = new MeshObject();
MeshObject* testMap = new MeshObject();
MeshObject* testMesh = new MeshObject();

SystemManager* sm;

void Init()
{
	// Init OpenGL
	InitializeWindow();
	glfwSetKeyCallback(mainWindow, GameKeyCallback);

	InitializeOpenGL();

	SetupImGui(mainWindow);

	// Init systems
	sm = new SystemManager();

	sm->AddSystem<VelocitySystem>();
	sm->AddSystem<FreecamSystem>();
	sm->AddSystem<RenderSystem>();

	//sm->RegisterSystems();

	EntityManager& em = EntityManager::GetInstance();

	// Create player
	{
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
		em.GetComponent<FreecamComponent>(entity) = { true, 6, 40, 20 };

		sm->GetSystem<RenderSystem>()->SetMainCameraEntity(entity);
	}
	// Create monkey
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(5, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 2, -4 };
		em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 3.14f / 4.0f, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/models/monkey.glb";
		modelLoader::LoadModel(*monkeyMesh, path);
		em.GetComponent<MeshComponent>(entity) = { monkeyMesh };
	}
	// Create map
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(5, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 0, -4 };
		em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/models/test_map.glb";
		modelLoader::LoadModel(*testMap, path);
		em.GetComponent<MeshComponent>(entity) = { testMap };
	}
	// Create teapot
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(5, components));
		em.GetComponent<PositionComponent>(entity) = { -6, 0, 0 };
		em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, -0.1f, 0 };
		em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };

		const char* path = "data/models/teapot.glb";
		modelLoader::LoadModel(*testMesh, path);
		em.GetComponent<MeshComponent>(entity) = { testMesh };
	}
}

int main()
{
	Init();

	while (!glfwWindowShouldClose(mainWindow))
	{
		TimeManager::Update();
		glfwPollEvents();

		double xPos, yPos;
		glfwGetCursorPos(mainWindow, &xPos, &yPos);
		InputManager::UpdateCursorDelta(xPos, yPos);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		StartImGuiFrame();

		sm->UpdateSystems();

		EndImGuiFrame();

		glfwSwapBuffers(mainWindow);
	}

	delete monkeyMesh;
	delete testMesh;
	delete testMap;

	ImGuiTerminate();

	delete sm;
	//sm->DeleteAllSystems();
	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}