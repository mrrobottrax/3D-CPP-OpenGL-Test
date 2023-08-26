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
	EntityPointer cameraEntity = entityManager.GetEntityPointer(pViewport->cameraEntity);
	glUniformMatrix4fv(sharedPerspectiveMatrixUnif, 1, GL_FALSE, &entityManager.GetComponent<CameraComponent>(cameraEntity).matrix[0][0]);
	MatrixStack mStack;
	mStack.Push();
	mStack.ApplyMatrix(glm::mat4_cast(entityManager.GetComponent<RotationComponent>(cameraEntity).value));
	mStack.Translate(-entityManager.GetComponent<PositionComponent>(cameraEntity).value);
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

			pViewport->toolInputFreeze = true;
		}
		else if (action == GLFW_RELEASE)
		{
			pViewport->toolInputFreeze = false;
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

	EntityManager& em = entityManager;
	const EntityPointer e = em.GetEntityPointer(em.AddEntity(malletMesh));

	em.GetComponent<MeshComponent>(e).pMesh = nullptr;
	MalletMesh& mesh = em.GetComponent<MalletMeshComponent>(e).mesh;

	glm::vec3 min = glm::min(blockStart, blockEnd);
	glm::vec3 max = glm::max(blockStart, blockEnd);

	const glm::vec3 center = ((max - min) / 2.0f) + min;

	min -= center;
	max -= center;

	entityManager.GetComponent<PositionComponent>(e).value = center;
	entityManager.GetComponent<RotationComponent>(e).value = glm::identity<glm::fquat>();
	entityManager.GetComponent<ScaleComponent>(e).value = glm::vec3(1);

	// Create box
	// -X Face
	mmHalfEdge* pNXBottomEdge;
	mmHalfEdge* pNXRightEdge;
	mmHalfEdge* pNXTopEdge;
	mmHalfEdge* pNXLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(-1, 0, 0);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = new glm::vec3(min.x, min.y, min.z);

		pNXBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = new glm::vec3(min.x, min.y, max.z);

		pNXRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = new glm::vec3(min.x, max.y, max.z);

		pNXTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = new glm::vec3(min.x, max.y, min.z);

		pNXLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}

	// +X Face
	mmHalfEdge* pPXBottomEdge;
	mmHalfEdge* pPXRightEdge;
	mmHalfEdge* pPXTopEdge;
	mmHalfEdge* pPXLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(1, 0, 0);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = new glm::vec3(max.x, min.y, max.z);

		pPXBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = new glm::vec3(max.x, min.y, min.z);

		pPXRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = new glm::vec3(max.x, max.y, min.z);

		pPXTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = new glm::vec3(max.x, max.y, max.z);

		pPXLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}

	// -Y Face
	mmHalfEdge* pNYBottomEdge;
	mmHalfEdge* pNYRightEdge;
	mmHalfEdge* pNYTopEdge;
	mmHalfEdge* pNYLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(0, -1, 0);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pPXBottomEdge->pTail->pPosition;

		pNYBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pNXRightEdge->pTail->pPosition;

		pNYRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pNXBottomEdge->pTail->pPosition;

		pNYTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = pPXRightEdge->pTail->pPosition;

		pNYLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}
	pNYLeftEdge->pTwin = pPXBottomEdge;
	pPXBottomEdge->pTwin = pNYLeftEdge;
	pNYRightEdge->pTwin = pNXBottomEdge;
	pNXBottomEdge->pTwin = pNYRightEdge;

	// +Y Face
	mmHalfEdge* pPYBottomEdge;
	mmHalfEdge* pPYRightEdge;
	mmHalfEdge* pPYTopEdge;
	mmHalfEdge* pPYLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(0, -1, 0);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pNXTopEdge->pTail->pPosition;

		pPYBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pPXLeftEdge->pTail->pPosition;

		pPYRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pPXTopEdge->pTail->pPosition;

		pPYTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = pNXLeftEdge->pTail->pPosition;

		pPYLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}
	pPYLeftEdge->pTwin = pNXTopEdge;
	pNXTopEdge->pTwin = pPYLeftEdge;
	pPYRightEdge->pTwin = pPXTopEdge;
	pPXTopEdge->pTwin = pPYRightEdge;

	// -Z Face
	mmHalfEdge* pNZBottomEdge;
	mmHalfEdge* pNZRightEdge;
	mmHalfEdge* pNZTopEdge;
	mmHalfEdge* pNZLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(0, 0, -1);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pPXRightEdge->pTail->pPosition;

		pNZBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pNXBottomEdge->pTail->pPosition;

		pNZRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pNXLeftEdge->pTail->pPosition;

		pNZTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = pPXTopEdge->pTail->pPosition;

		pNZLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}
	pNZBottomEdge->pTwin = pNYTopEdge;
	pNYTopEdge->pTwin = pNZBottomEdge;
	pNZLeftEdge->pTwin = pPXRightEdge;
	pPXRightEdge->pTwin = pNZLeftEdge;
	pNZRightEdge->pTwin = pNXLeftEdge;
	pNXLeftEdge->pTwin = pNZRightEdge;
	pNZTopEdge->pTwin = pPYTopEdge;
	pPYTopEdge->pTwin = pNZTopEdge;

	// +Z Face
	mmHalfEdge* pPZBottomEdge;
	mmHalfEdge* pPZRightEdge;
	mmHalfEdge* pPZTopEdge;
	mmHalfEdge* pPZLeftEdge;
	{
		mmFace* pFace = new mmFace();
		pFace->normal = glm::vec3(0, 0, 1);

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pFace = pFace;
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pNXRightEdge->pTail->pPosition;

		pPZBottomEdge = &firstEdge;

		pFace->pEdge = &firstEdge;
		mesh.pFirstEdge = &firstEdge;

		mmHalfEdge& secondEdge = *new mmHalfEdge();
		firstEdge.pNext = &secondEdge;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pFace = pFace;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pPXBottomEdge->pTail->pPosition;

		pPZRightEdge = &secondEdge;

		mmHalfEdge& thirdEdge = *new mmHalfEdge();
		secondEdge.pNext = &thirdEdge;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pFace = pFace;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pPXLeftEdge->pTail->pPosition;

		pPZTopEdge = &thirdEdge;

		mmHalfEdge& fourthEdge = *new mmHalfEdge();
		thirdEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &thirdEdge;
		fourthEdge.pFace = pFace;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = pNXTopEdge->pTail->pPosition;

		pPZLeftEdge = &fourthEdge;

		fourthEdge.pNext = &firstEdge;
		firstEdge.pPrev = &fourthEdge;
	}
	pPZBottomEdge->pTwin = pNYBottomEdge;
	pNYBottomEdge->pTwin = pPZBottomEdge;
	pPZLeftEdge->pTwin = pNXRightEdge;
	pNXRightEdge->pTwin = pPZLeftEdge;
	pPZRightEdge->pTwin = pPXLeftEdge;
	pPXLeftEdge->pTwin = pPZRightEdge;
	pPZTopEdge->pTwin = pPYBottomEdge;
	pPYBottomEdge->pTwin = pPZTopEdge;

	mesh.Validate(false);
	mesh.UpdateRenderMesh(e);
}