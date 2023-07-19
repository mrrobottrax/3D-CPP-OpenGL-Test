#include "malletpch.h"
#include "blocktool.h"

#include <gl/glutil.h>
#include <common/matrixstack.h>
#include <memory/entitymanager.h>
#include <components/rotationcomponent.h>
#include <components/positioncomponent.h>

#include <malletarchetypes.h>

void BlockTool::Draw(Viewport* pViewport)
{
	if (!creatingBlock)
		return;

	if (!glInit)
	{
		glGenVertexArrays(1, &boxVao);
		glBindVertexArray(boxVao);

		glGenBuffers(1, &boxPositionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, boxPositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(BlockTool::blockPreviewTemplate), BlockTool::blockPreviewTemplate, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glInit = true;
	}

	// Box being created
	glUseProgram(solidShaderProgram);
	glBindVertexArray(boxVao);

	// Set uniforms
	glUniformMatrix4fv(sharedPerspectiveMatrixUnif, 1, GL_FALSE, &pViewport->pCamera->matrix[0][0]);
	MatrixStack mStack;
	mStack.Push();
	mStack.ApplyMatrix(glm::mat4_cast(entityManager.GetComponent<RotationComponent>(pViewport->cameraEntity).value));
	mStack.Translate(-entityManager.GetComponent<PositionComponent>(pViewport->cameraEntity).value);
	glUniformMatrix4fv(sharedPositionMatrixUnif, 1, GL_FALSE, &mStack.Top()[0][0]);

	// Set vertex data
	for (int i = 0; i < sizeof(BlockTool::blockPreviewTemplate) / sizeof(float); ++i)
	{
		bool start = BlockTool::blockPreviewTemplate[i] < 0;

		if (i % 3 == 0)
		{
			BlockTool::blockPreview[i] = start ? BlockTool::blockStart.x : BlockTool::blockEnd.x;
		}
		else if (i % 3 == 1)
		{
			BlockTool::blockPreview[i] = start ? BlockTool::blockStart.y : BlockTool::blockEnd.y;
		}
		else if (i % 3 == 2)
		{
			BlockTool::blockPreview[i] = start ? BlockTool::blockStart.z : BlockTool::blockEnd.z;
		}
	}

	glNamedBufferSubData(boxPositionBuffer, 0, sizeof(BlockTool::blockPreview), BlockTool::blockPreview);

	glDisable(GL_DEPTH_TEST);

	glUniform3f(solidColorUnif, 1, 1, 0);
	glDrawArrays(GL_LINES, 0, 24);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
}

void BlockTool::MouseCallback(Viewport* pViewport, GLFWwindow* pWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			glm::vec2 pos = pViewport->GetGridMousePos2D();

			switch (pViewport->mode)
			{
			case top:
				blockStart.x = pos.x;
				blockStart.z = pos.y;

				blockEnd.x = pos.x;
				blockEnd.z = pos.y;
				break;
			case side:
				blockStart.z = pos.x;
				blockStart.y = pos.y;

				blockEnd.z = pos.x;
				blockEnd.y = pos.y;
				break;
			case front:
				blockStart.x = pos.x;
				blockStart.y = pos.y;

				blockEnd.x = pos.x;
				blockEnd.y = pos.y;
				break;
			default:
				break;
			}

			creatingBlock = true;
		}
	}
}

void BlockTool::MousePosCallback(Viewport* pViewport, GLFWwindow* pWindow, double xPos, double yPos)
{
	if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		return;

	glm::vec2 pos = pViewport->GetGridMousePos2D();

	switch (pViewport->mode)
	{
	case top:
		blockEnd.x = pos.x;
		blockEnd.z = pos.y;
		break;
	case side:
		blockEnd.z = pos.x;
		blockEnd.y = pos.y;
		break;
	case front:
		blockEnd.x = pos.x;
		blockEnd.y = pos.y;
		break;
	default:
		break;
	}
}

void BlockTool::KeyboardCallback(Viewport* pViewport, GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	// Finish block
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (creatingBlock)
		{
			CreateBlock();
		}
	}
}

void BlockTool::CreateBlock()
{
	creatingBlock = false;

	const Entity e = entityManager.AddEntity(malletMesh);

	const EntityManager& em = entityManager;
	em.GetComponent<MeshComponent>(e).pMesh = nullptr;
}