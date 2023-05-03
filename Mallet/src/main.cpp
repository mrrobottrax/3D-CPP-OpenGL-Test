#include <malletpch.h>

#include <main.h>

#include <gl/glutil.h>
#include <gl/malletglutil.h>
#include <input/malletinputlayer.h>

#include <meshobject.h>
#include <systems/systemmanager.h>
#include <memory/entitymanager.h>
#include <input/inputmanager.h>
#include <timemanager.h>
#include <modelloader.h>

#include <systems/rendersystem.h>
#include <systems/freecamsystem.h>
#include <systems/velocitysystem.h>

#include <components/cameracomponent.h>
#include <components/freecamcomponent.h>
#include <components/rotationcomponent.h>
#include <components/meshcomponent.h>
#include <components/velocitycomponent.h>
#include <components/positioncomponent.h>
#include <components/idcomponent.h>

#include <imgui/imguiutil.h>
#include <ui/malletui.h>

using namespace std;

// Show memory leaks TEMP
#define _CRTDBG_MAP_ALLOC

// TEMP
// TODO: Remove
// Cube mesh
MeshObject* monkeyMesh	= new MeshObject();
MeshObject* testMap		= new MeshObject();
MeshObject* testMesh	= new MeshObject();

void Init()
{
	// Init OpenGL
	InitializeWindow();

	SetupInputCallbacks(pMainWindow);

	InitializeOpenGL();

	// Init systems
	SystemManager& sm = systemManager;

	sm.AddSystem<FreecamSystem>();
	sm.AddSystem<VelocitySystem>();

	RenderSystem& rs = sm.AddSystem<RenderSystem>();
	rs.autoDraw = false;

	EntityManager& em = entityManager;

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

	// UI
	SetupImGui(mainWindow);
	MalletUi::Setup();
}

void End()
{
	ImGuiTerminate();
	MalletUi::Destroy();

	entityManager.DeleteAllEntities();
	systemManager.DeleteAllSystems();
	console.DeleteCommands();
	glfwTerminate();

	::exit(EXIT_SUCCESS);
}

int main()
{
	// Memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Init();

	while (!glfwWindowShouldClose(mainWindow))
	{
		timeManager.Update();
		glfwPollEvents();

		double xPos, yPos;
		glfwGetCursorPos(mainWindow, &xPos, &yPos);
		inputManager.UpdateCursorDelta(xPos, yPos);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		StartImGuiFrame();

		systemManager.UpdateSystems();

		MalletUi::DrawTree();

		EndImGuiFrame();

		glfwSwapBuffers(mainWindow);
	}

	delete monkeyMesh;
	delete testMesh;
	delete testMap;

	End();
}