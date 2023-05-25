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
#include <systems/unscaledvelocitysystem.h>

#include <components/cameracomponent.h>
#include <components/freecamcomponent.h>
#include <components/rotationcomponent.h>
#include <components/meshcomponent.h>
#include <components/velocitycomponent.h>
#include <components/unscaledvelocitycomponent.h>
#include <components/positioncomponent.h>
#include <components/idcomponent.h>
#include <components/hullcollider.h>
#include <components/rigidbodycomponent.h>
#include <components/masscomponent.h>
#include <components/scalecomponent.h>

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

glm::vec3 vertices[] = {
	{ -0.5f, -0.5f, -0.5f },
	{  0.5f, -0.5f, -0.5f },
	{ -0.5f,  0.5f, -0.5f },
	{  0.5f,  0.5f, -0.5f },

	{ -0.5f, -0.5f,  0.5f },
	{  0.5f, -0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{  0.5f,  0.5f,  0.5f },
};
ConvexHull boxHull = ConvexHull(8, vertices);

void Init()
{
	// Init OpenGL
	InitializeWindow();
	glfwSetKeyCallback(pMainWindow, GameKeyCallback);

	InitializeOpenGL();

	SetupImGui(pMainWindow);

	// Init systems
	SystemManager& sm = systemManager;

	sm.AddTickSystem<PhysicsSystem>();
	sm.AddTickSystem<VelocitySystem>();

	sm.AddSystem<FreecamSystem>();
	sm.AddSystem<UnscaledVelocitySystem>();
	sm.AddSystem<RenderSystem>();

	EntityManager& em = entityManager;

	// Create player
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<UnscaledVelocityComponent>(),
			Component().init<CameraComponent>(),
			Component().init<RotationComponent>(),
			Component().init<FreecamComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(6, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.GetComponent<UnscaledVelocityComponent>(entity) = { { 0, 0, 0, 0, 0, 0 } };
		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = { 0.03f, 1000.0f };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
		em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20 };

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);

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
	{
		modelLoader::LoadModel(boxMesh, "data/models/box.glb");
		Component components[] = {
		Component().init<IdComponent>(),
		Component().init<PositionComponent>(),
		Component().init<VelocityComponent>(),
		Component().init<MeshComponent>(),
		Component().init<RotationComponent>(),
		Component().init<HullCollider>(),
		Component().init<RigidBodyComponent>(),
		Component().init<MassComponent>(),
		Component().init<ScaleComponent>(),
		};
		EntityArchetype boxArchetype = EntityArchetype(9, components);
		// Create box 1
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 0, -1, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 25, 0.5f, 25 };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 0, 0.6f, Average };
			em.GetComponent<MassComponent>(entity).SetMassAndInertia(INFINITY);
		}
		// Create box 2
		for (int i = 0; i < 20; ++i)
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 0, i * 0.6f, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 0.5f, 0.5f, 0.5f };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 1, 0.6f, Average };
			em.GetComponent<MassComponent>(entity).SetMassAndInertia(1);
			//em.GetComponent<MassComponent>(entity).SetInertia(INFINITY);
		}
		// Throw box
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 80, 10, -5 };
			em.GetComponent<VelocityComponent>(entity) = { -10, 0, 0, 0, 0, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 0.5f, 0.5f, 0.5f };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 0, 0.6f, Average };
			em.GetComponent<MassComponent>(entity).SetMassAndInertia(1);
			//em.GetComponent<MassComponent>(entity).SetInertia(INFINITY);
		}
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 2, 2, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 10, 0, 5, 5, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 0.5f, 0.5f, 0.5f };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 1, 0.6f, Average };
			em.GetComponent<MassComponent>(entity).SetMassAndInertia(1);
			//em.GetComponent<MassComponent>(entity).SetInertia(INFINITY);
		}
	}

	// Hide cursor
	glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	while (!glfwWindowShouldClose(pMainWindow))
	{
		timeManager.Update();
		glfwPollEvents();

		double xPos, yPos;
		glfwGetCursorPos(pMainWindow, &xPos, &yPos);
		inputManager.UpdateCursorDelta(xPos, yPos);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		StartImGuiFrame();

		systemManager.UpdateSystems();

		EndImGuiFrame();

		glfwSwapBuffers(pMainWindow);
	}

	End();
}