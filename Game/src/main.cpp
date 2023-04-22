#include <gamepch.h>

#include <main.h>

#include <gl/glutil.h>
#include <input/gameinputlayer.h>

#include <meshobject.h>
#include <timemanager.h>
#include <modelloader.h>

#include <systems/rendersystem.h>
#include <systems/velocitysystem.h>
#include <systems/freecamsystem.h>
#include <systems/physicssystem.h>

#include <components/cameracomponent.h>
#include <components/freecamcomponent.h>
#include <components/rotationcomponent.h>
#include <components/meshcomponent.h>
#include <components/velocitycomponent.h>
#include <components/positioncomponent.h>
#include <components/idcomponent.h>
#include <components/hullcollider.h>
#include <components/rigidbodycomponent.h>
#include <components/masscomponent.h>

#include <imgui/imguiutil.h>
#include <input/inputmanager.h>

#include <thread>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC

// TEMP DEV MESHES
MeshObject monkeyMesh = MeshObject();
MeshObject testMap = MeshObject();
MeshObject testMesh = MeshObject();
MeshObject boxMesh = MeshObject();

//glm::vec3 vertices[] = {
//	{ -0.5f, -0.5f, -0.5f },
//	{  0.5f, -0.5f, -0.5f },
//	{ -0.5f,  0.5f, -0.5f },
//	{  0.5f,  0.5f, -0.5f },
//
//	{ -0.5f, -0.5f,  0.5f },
//	{  0.5f, -0.5f,  0.5f },
//	{ -0.5f,  0.5f,  0.5f },
//	{  0.5f,  0.5f,  0.5f },
//};
ConvexHull boxHull = ConvexHull();

void Init()
{
	// Init OpenGL
	InitializeWindow();
	glfwSetKeyCallback(mainWindow, GameKeyCallback);

	InitializeOpenGL();

	SetupImGui(mainWindow);

	// Init systems
	SystemManager& sm = systemManager;
	sm.AddSystem<FreecamSystem>();

	sm.AddSystem<PhysicsSystem>();
	sm.AddSystem<VelocitySystem>();
	sm.AddSystem<RenderSystem>();

	EntityManager& em = entityManager;

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
		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = { 0.03f, 1000.0f };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
		em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20 };

		int w, h;
		glfwGetWindowSize(mainWindow, &w, &h);

		RenderSystem& rs = sm.GetSystem<RenderSystem>();
		rs.SetMainCameraEntity(entity);
		rs.CalcFrustumScale(cam, 80);
		rs.CalcPerspectiveMatrix(cam, w, h);
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
		em.GetComponent<PositionComponent>(entity) = { 0, 2, -8 };
		em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 3.14f / 4.0f, 0 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/models/monkey.glb";
		modelLoader::LoadModel(monkeyMesh, path);
		em.GetComponent<MeshComponent>(entity) = { &monkeyMesh };
	}
	// Create map
	/*{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(4, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 0, -4 };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		modelLoader::LoadModel(testMap, "data/models/test_map.glb");
		em.GetComponent<MeshComponent>(entity) = { &testMap };
	}*/
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

		modelLoader::LoadModel(testMesh, "data/models/teapot.glb");
		em.GetComponent<MeshComponent>(entity) = { &testMesh };
	}
	// Create box 1
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
			Component().init<HullCollider>(),
			Component().init<RigidBodyComponent>(),
			Component().init<MassComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(8, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 6, -5 };
		em.GetComponent<VelocityComponent>(entity) = { 0, -0.1f, 0, 0.05f, 0.05f, 0 };
		em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };

		modelLoader::LoadModel(boxMesh, "data/models/box.glb");
		em.GetComponent<MeshComponent>(entity) = { &boxMesh };

		em.GetComponent<HullCollider>(entity) = { &boxHull };
		em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull };
		em.GetComponent<MassComponent>(entity).SetMass(1);
	}
	// Create box 2
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
			Component().init<HullCollider>(),
			Component().init<RigidBodyComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(7, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 3, -5 };
		em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, -0.05f, 0, 0 };
		em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };

		em.GetComponent<MeshComponent>(entity) = { &boxMesh };

		em.GetComponent<HullCollider>(entity) = { &boxHull };
		em.GetComponent<RigidBodyComponent>(entity) = { true, ColliderType::Hull };
	}

	// Hide cursor
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void End()
{
	ImGuiTerminate();

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

	const static int vertCount = 1000;
	const static float hullSize = 10;
	glm::vec3* vertices = new glm::vec3[vertCount];

	for (int i = 0; i < vertCount; ++i)
	{
		vertices[i] = {
			rand() / (float)RAND_MAX * hullSize,
			rand() / (float)RAND_MAX * hullSize,
			rand() / (float)RAND_MAX * hullSize,
		};
	}

	std::thread qh(&ConvexHull::QuickHull, &boxHull, vertCount, vertices);

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

		EndImGuiFrame();

		glfwSwapBuffers(mainWindow);
	}

	qh.join();
	delete[] vertices;

	End();
}