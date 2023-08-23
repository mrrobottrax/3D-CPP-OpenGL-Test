#include <malletpch.h>

#include <main.h>

#include <gl/glutil.h>
#include <input/malletinputlayer.h>

#include <meshobject.h>
#include <timemanager.h>
#include <modelloader.h>

#include <systems/systemmanager.h>

#include <rendering/rendersystem.h>
#include <rendering/debugdraw.h>
#include <physics/velocitysystem.h>
#include <systems/freecamsystem.h>
#include <physics/physicssystem.h>
#include <physics/unscaledvelocitysystem.h>

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
#include <input/inputsystem.h>
#include <ui/malletuisystem.h>

#include <physics/halfedgemesh.h>
#include <physics/quickhull.h>
#include <world/worldutil.h>

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
	SetupInputCallbacks(pMainWindow);

	InitializeOpenGL();
	SetupImGui(pMainWindow);

	// Init systems
	SystemManager& sm = systemManager;

	sm.AddTickSystem<PhysicsSystem>(physicsSystem);
	sm.AddTickSystem<VelocitySystem>(velocitySystem);

	sm.AddSystem<InputSystem>(inputSystem);
	sm.AddSystem<FreecamSystem>(freecamSystem);
	sm.AddSystem<UnscaledVelocitySystem>(unscaledVelocitySystem);
	sm.AddSystem<RenderSystem>(renderSystem);
	sm.AddSystem<DebugDraw>(debugDraw);
	sm.AddSystem<MalletUiSystem>(malletUI);

	renderSystem.autoDraw = false;

	sm.InitSystems();
}

void End()
{
	ImGuiTerminate();

	worldUtil.UnloadWorld();

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