#include <pch.h>

#include <main.h>

#include <systemManager.h>
#include <engine/meshObject.h>
#include <engine/idComponent.h>
#include <engine/positionComponent.h>
#include <engine/velocityComponent.h>
#include <engine/meshComponent.h>
#include <engine/memory/entityManager.h>
#include <engine/rotationComponent.h>
#include <engine/timeManager.h>
#include <engine/freecamComponent.h>
#include <engine/modelLoader.h>
#include <renderSystem.h>
#include <cameraComponent.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC

// TEMP
// TODO: Remove
// Cube mesh
MeshObject* monkeyMesh = new MeshObject();
MeshObject* testMap = new MeshObject();
MeshObject* testMesh = new MeshObject();

void Init()
{
	// Init OpenGL
	InitializeWindow();
	InitializeOpenGL();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Init systems
	systemManager::registerSystems();

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

		Entity entity = em.addEntity(EntityArchetype(6, components));
		em.getComponent<PositionComponent>(entity) = { 0, 2, 0 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
		em.getComponent<CameraComponent>(entity) = { 80.0f, 0.03f, 1000.0f };
		em.getComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
		em.getComponent<FreecamComponent>(entity) = { 6, 40, 20 };

		systemManager::getSystem<RenderSystem>()->setMainCameraEntity(entity);
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

		Entity entity = em.addEntity(EntityArchetype(5, components));
		em.getComponent<PositionComponent>(entity) = { 0, 2, -4 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 3.14f / 4.0f, 0 };
		em.getComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/models/monkey.glb";
		modelLoader::loadModel(*monkeyMesh, path);
		em.getComponent<MeshComponent>(entity) = { monkeyMesh };
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

		Entity entity = em.addEntity(EntityArchetype(5, components));
		em.getComponent<PositionComponent>(entity) = { 0, 0, -4 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
		em.getComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/models/test_map.glb";
		modelLoader::loadModel(*testMap, path);
		em.getComponent<MeshComponent>(entity) = { testMap };
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

		Entity entity = em.addEntity(EntityArchetype(5, components));
		em.getComponent<PositionComponent>(entity) = { -6, 0, 0 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, -0.1f, 0 };
		em.getComponent<RotationComponent>(entity) = { 0.7071068f, 0, 0.7071068f, 0 };

		const char* path = "data/models/teapot.glb";
		modelLoader::loadModel(*testMesh, path);
		em.getComponent<MeshComponent>(entity) = { testMesh };
	}
}

int main()
{
	Init();

	while (!glfwWindowShouldClose(window))
	{
		TimeManager::update();
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		// Rendering
		ImGui::Render();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		systemManager::updateSystems();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	delete monkeyMesh;
	delete testMesh;
	delete testMap;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	systemManager::deleteAllSystems();
	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}