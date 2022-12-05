#include <glutil.cpp>

#include <main.h>

#include <systemManager.h>
#include <floats.h>
#include <meshObject.h>
#include <idComponent.h>
#include <positionComponent.h>
#include <velocityComponent.h>
#include <meshComponent.h>
#include <entityManager.h>
#include <algorithm>
#include <rotationComponent.h>
#include <timeManager.h>
#include <freecamComponent.h>
#include <modelLoader.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// TEMP
// TODO: Remove
// Cube mesh
MeshObject mesh;

GLuint shaderProgram;
GLuint positionBufferObject;
GLuint normalBufferObject;
GLuint elementBufferObject;
GLuint vao;
GLuint offsetUniform;
GLuint colorUniform;
GLuint matrixUniform;

void init()
{
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
		em.getComponent<PositionComponent>(entity) = { 0, 0, 0 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 0, 0 };
		em.getComponent<CameraComponent>(entity) = { 80.0f, 0.03f, 1000.0f };
		em.getComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };
		em.getComponent<FreecamComponent>(entity) = { 6, 40, 20 };

		systemManager::getSystem<RenderSystem>()->setMainCameraEntity(entity);
	}
	// Create cube
	{
		Component components[] = {
			Component().init<IdComponent>(),
			Component().init<PositionComponent>(),
			Component().init<VelocityComponent>(),
			Component().init<MeshComponent>(),
			Component().init<RotationComponent>(),
		};

		Entity entity = em.addEntity(EntityArchetype(5, components));
		em.getComponent<PositionComponent>(entity) = { 0, -3, 0 };
		em.getComponent<VelocityComponent>(entity) = { 0, 0, 0, 0, 3.14f / 2.0f, 0 };
		em.getComponent<RotationComponent>(entity) = { 1, 0, 0, 0 };

		const char* path = "data/test_model.glb";
		modelLoader::loadModel(mesh, path);
		em.getComponent<MeshComponent>(entity) = { &mesh };
	}

	// Init OpenGL
	initializeWindow();

	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	shaderProgram = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	offsetUniform = glGetUniformLocation(shaderProgram, "offset");
	colorUniform = glGetUniformLocation(shaderProgram, "color");
	matrixUniform = glGetUniformLocation(shaderProgram, "perspectiveMatrix");

	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertCount * sizeof(float), mesh.verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, mesh.normalCount * sizeof(float), mesh.normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indicesCount * sizeof(GLshort), mesh.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
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

	delete[] mesh.indices;
	delete[] mesh.verts;

	systemManager::deleteAllSystems();
	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}