#include <gamepch.h>

#include <main.h>

#include <gl/glutil.h>
#include <input/gameinputlayer.h>

#include <meshobject.h>
#include <timemanager.h>
#include <modelloader.h>

#include <systems/rendersystem.h>
#include <systems/debugdraw.h>
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

#include <physics/halfedgemesh.h>
#include <physics/quickhull.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC

// TEMP DEV MESHES
MeshObject monkeyMesh = MeshObject();
MeshObject testMap = MeshObject();
MeshObject testMesh = MeshObject();
MeshObject boxMesh = MeshObject();

HalfEdgeMesh boxHull;

void Init()
{
	// Init OpenGL
	InitializeWindow();
	glfwSetKeyCallback(pMainWindow, GameKeyCallback);

	InitializeOpenGL();

	SetupImGui(pMainWindow);

	// Init systems
	SystemManager& sm = systemManager;

	sm.AddTickSystem<PhysicsSystem>(physicsSystem);
	sm.AddTickSystem<VelocitySystem>(velocitySystem);

	sm.AddSystem<FreecamSystem>(freecamSystem);
	sm.AddSystem<UnscaledVelocitySystem>(unscaledVelocitySystem);
	sm.AddSystem<RenderSystem>(renderSystem);
	sm.AddSystem<DebugDraw>(debugDraw);

	sm.InitSystems();

	EntityManager& em = entityManager;

	// Create player
	{
		Component components[] = {
			Component().Init<IdComponent>(),
			Component().Init<PositionComponent>(),
			Component().Init<UnscaledVelocityComponent>(),
			Component().Init<CameraComponent>(),
			Component().Init<RotationComponent>(),
			Component().Init<FreecamComponent>(),
		};

		Entity entity = em.AddEntity(EntityArchetype(6, components));
		em.GetComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.GetComponent<UnscaledVelocityComponent>(entity) = { { 0, 0, 0, 0, 0, 0 } };
		CameraComponent& cam = em.GetComponent<CameraComponent>(entity) = { 0.03f, 1000.0f };
		em.GetComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
		em.GetComponent<FreecamComponent>(entity) = { 6, 40, 20 };

		int w, h;
		glfwGetWindowSize(pMainWindow, &w, &h);

		RenderSystem& rs = renderSystem;
		rs.SetMainCameraEntity(entity);
		rs.CalcFrustumScale(cam, 80);
		rs.CalcPerspectiveMatrix(cam, w, h);
	}
	// Create monkey
	{
		Component components[] = {
			Component().Init<IdComponent>(),
			Component().Init<PositionComponent>(),
			Component().Init<VelocityComponent>(),
			Component().Init<MeshComponent>(),
			Component().Init<RotationComponent>(),
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
			Component().Init<IdComponent>(),
			Component().Init<PositionComponent>(),
			Component().Init<VelocityComponent>(),
			Component().Init<MeshComponent>(),
			Component().Init<RotationComponent>(),
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
		Component().Init<IdComponent>(),
		Component().Init<PositionComponent>(),
		Component().Init<VelocityComponent>(),
		Component().Init<MeshComponent>(),
		Component().Init<RotationComponent>(),
		Component().Init<HullCollider>(),
		Component().Init<RigidBodyComponent>(),
		Component().Init<MassComponent>(),
		Component().Init<ScaleComponent>(),
		};
		EntityArchetype boxArchetype = EntityArchetype(9, components);

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

		// Quickhull
		{
			QuickHull qh = QuickHull();
			qh.Algorithm(8, vertices, boxHull);
		}

		boxHull.Draw(20);

		// Create box 1
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 0, -10, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 200, 20, 200 };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 0, 0.6f, Average };
			PhysicsSystem::ComputeInertia(entity);
			em.GetComponent<MassComponent>(entity).SetMass(INFINITY);
		}
		// Create box 2
		for (int i = 0; i < 6; ++i)
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 0, i * 0.6f + 0.5f, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 0.5f, 0.5f, 0.5f };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 1, 0.6f, Average };
			PhysicsSystem::ComputeInertia(entity);
			em.GetComponent<MassComponent>(entity).SetMass(1);
		}
		{
			Entity entity = em.AddEntity(boxArchetype);
			em.GetComponent<PositionComponent>(entity) = { 1, 1, -5 };
			em.GetComponent<VelocityComponent>(entity) = { 0, 10, 0, 5, 5, 0 };
			em.GetComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };
			em.GetComponent<ScaleComponent>(entity) = { 0.5f, 0.5f, 0.5f };

			em.GetComponent<MeshComponent>(entity) = { &boxMesh };

			em.GetComponent<HullCollider>(entity) = { &boxHull };
			em.GetComponent<RigidBodyComponent>(entity) = { false, ColliderType::Hull, 1, 0.6f, Average };
			PhysicsSystem::ComputeInertia(entity);
			em.GetComponent<MassComponent>(entity).SetMass(1);
		}
	}

	// Hide cursor
	glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void End()
{
	ImGuiTerminate();

	entityManager.DeleteAllEntities();
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