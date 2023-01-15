#include <engine/main.h>

#include <engine/systemManager.h>
#include <engine/common/floats.h>
#include <engine/meshObject.h>
#include <engine/idComponent.h>
#include <engine/positionComponent.h>
#include <engine/velocityComponent.h>
#include <engine/meshComponent.h>
#include <engine/memory/entityManager.h>
#include <algorithm>
#include <engine/rotationComponent.h>
#include <engine/timeManager.h>
#include <engine/freecamComponent.h>
#include <engine/modelLoader.h>
#include <engine/gl/shaderLoader.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// TEMP
// TODO: Remove
// Cube mesh
MeshObject* monkeyMesh = new MeshObject();
MeshObject* testMap = new MeshObject();
MeshObject* testMesh = new MeshObject();

GLuint shaderProgram;
GLuint vao;
GLuint perspectiveMatrix;
GLuint positionMatrix;
GLuint normalMatrix;
GLuint sunDirUnif;
GLuint sunIntensityUnif;
GLuint ambientIntensityUnif;
GLuint colorUnif;

void init()
{
	// Init OpenGL
	initializeWindow();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	std::vector<GLuint> shaderList;
	const char* strVertShader = shaderLoader::loadShader("data/shaders/normal.vert");
	const char* strFragShader = shaderLoader::loadShader("data/shaders/normal.frag");
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragShader));
	delete[] strVertShader;
	delete[] strFragShader;

	shaderProgram = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	perspectiveMatrix = glGetUniformLocation(shaderProgram, "perspectiveMatrix");
	positionMatrix = glGetUniformLocation(shaderProgram, "positionMatrix");
	normalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	sunDirUnif = glGetUniformLocation(shaderProgram, "sunDir");
	sunIntensityUnif = glGetUniformLocation(shaderProgram, "sunIntensity");
	ambientIntensityUnif = glGetUniformLocation(shaderProgram, "ambientIntensity");
	colorUnif = glGetUniformLocation(shaderProgram, "diffuseColor");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

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

	glBindVertexArray(0);
}

int main()
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		timeManager::update();
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		systemManager::updateSystems();

		glfwSwapBuffers(window);
	}

	delete monkeyMesh;
	delete testMesh;
	delete testMap;

	systemManager::deleteAllSystems();
	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}