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
	mmHalfEdge* pNegXTop;
	mmHalfEdge* pNegXBottom;
	mmHalfEdge* pNegXLeft;
	mmHalfEdge* pNegXRight;
	{
		const glm::vec3 normal(-1, 0, 0);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = new glm::vec3(min.x, min.y, min.z);
		firstEdge.pFace = pFace1;

		mesh.pFirstEdge = &firstEdge;;
		pNegXBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = new glm::vec3(min.x, min.y, max.z);
		secondEdge.pFace = pFace1;

		pNegXRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = new glm::vec3(min.x, max.y, max.z);
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;
		
		pNegXTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = new glm::vec3(min.x, max.y, min.z);
		sixthEdge.pFace = pFace2;

		pNegXLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}

	// +Y Face
	mmHalfEdge* pPosYTop;
	mmHalfEdge* pPosYBottom;
	mmHalfEdge* pPosYLeft;
	mmHalfEdge* pPosYRight;
	{
		const glm::vec3 normal(0, 1, 0);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pNegXTop->pTail->pPosition;
		firstEdge.pFace = pFace1;

		pPosYBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = new glm::vec3(max.x, max.y, max.z);
		secondEdge.pFace = pFace1;

		pPosYRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = new glm::vec3(max.x, max.y, min.z);
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;

		pPosYTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = pNegXLeft->pTail->pPosition;
		sixthEdge.pFace = pFace2;

		pPosYLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}
	pNegXTop->pTwin = pPosYLeft;
	pPosYLeft->pTwin = pNegXTop;

	// +X Face
	mmHalfEdge* pPosXTop;
	mmHalfEdge* pPosXBottom;
	mmHalfEdge* pPosXLeft;
	mmHalfEdge* pPosXRight;
	{
		const glm::vec3 normal(1, 0, 0);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = new glm::vec3(max.x, min.y, max.z);
		firstEdge.pFace = pFace1;

		pPosXBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = new glm::vec3(max.x, min.y, min.z);
		secondEdge.pFace = pFace1;

		pPosXRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pPosYTop->pTail->pPosition;
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;

		pPosXTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = pPosYRight->pTail->pPosition;
		sixthEdge.pFace = pFace2;

		pPosXLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}
	pPosXTop->pTwin = pPosYRight;
	pPosYRight->pTwin = pPosXTop;

	// -Y Face
	mmHalfEdge* pNegYTop;
	mmHalfEdge* pNegYBottom;
	mmHalfEdge* pNegYLeft;
	mmHalfEdge* pNegYRight;
	{
		const glm::vec3 normal(0, -1, 0);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pPosXBottom->pTail->pPosition;
		firstEdge.pFace = pFace1;

		pNegYBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pNegXRight->pTail->pPosition;
		secondEdge.pFace = pFace1;

		pNegYRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pNegXBottom->pTail->pPosition;
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;

		pNegYTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = pPosXRight->pTail->pPosition;
		sixthEdge.pFace = pFace2;

		pNegYLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}
	pNegYRight->pTwin = pNegXBottom;
	pNegXBottom->pTwin = pNegYRight;
	pNegYLeft->pTwin = pPosXBottom;
	pPosXBottom->pTwin = pNegYLeft;

	// +Z Face
	mmHalfEdge* pPosZTop;
	mmHalfEdge* pPosZBottom;
	mmHalfEdge* pPosZLeft;
	mmHalfEdge* pPosZRight;
	{
		const glm::vec3 normal(0, 0, 1);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pNegXRight->pTail->pPosition;
		firstEdge.pFace = pFace1;

		pPosZBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pPosXBottom->pTail->pPosition;
		secondEdge.pFace = pFace1;

		pPosZRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pPosXLeft->pTail->pPosition;
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;

		pPosZTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = pNegXTop->pTail->pPosition;
		sixthEdge.pFace = pFace2;

		pPosZLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}
	pPosZLeft->pTwin = pNegXRight;
	pNegXRight->pTwin = pPosZLeft;
	pPosZBottom->pTwin = pNegYBottom;
	pNegYBottom->pTwin = pPosZBottom;
	pPosZRight->pTwin = pPosXLeft;
	pPosXLeft->pTwin = pPosZRight;
	pPosZTop->pTwin = pPosYBottom;
	pPosYBottom->pTwin = pPosZTop;

	// -Z Face
	mmHalfEdge* pNegZTop;
	mmHalfEdge* pNegZBottom;
	mmHalfEdge* pNegZLeft;
	mmHalfEdge* pNegZRight;
	{
		const glm::vec3 normal(0, 0, -1);

		// First tri
		mmFace* pFace1 = new mmFace();

		mmHalfEdge& firstEdge = *new mmHalfEdge();
		firstEdge.pTail = new mmVertex();
		firstEdge.pTail->pPosition = pNegYLeft->pTail->pPosition;
		firstEdge.pFace = pFace1;

		pNegZBottom = &firstEdge;

		firstEdge.pNext = new mmHalfEdge();
		mmHalfEdge& secondEdge = *firstEdge.pNext;
		secondEdge.pPrev = &firstEdge;
		secondEdge.pTail = new mmVertex();
		secondEdge.pTail->pPosition = pNegYTop->pTail->pPosition;
		secondEdge.pFace = pFace1;

		pNegZRight = &secondEdge;

		secondEdge.pNext = new mmHalfEdge();
		mmHalfEdge& thirdEdge = *secondEdge.pNext;
		thirdEdge.pPrev = &secondEdge;
		thirdEdge.pTail = new mmVertex();
		thirdEdge.pTail->pPosition = pNegXLeft->pTail->pPosition;
		thirdEdge.pFace = pFace1;

		thirdEdge.pNext = &firstEdge;
		firstEdge.pPrev = &thirdEdge;

		pFace1->pEdge = &firstEdge;
		pFace1->normal = normal;

		// Second tri
		mmFace* pFace2 = new mmFace();

		thirdEdge.pTwin = new mmHalfEdge();
		mmHalfEdge& fourthEdge = *thirdEdge.pTwin;
		fourthEdge.pTwin = &thirdEdge;
		fourthEdge.pTail = new mmVertex();
		fourthEdge.pTail->pPosition = firstEdge.pTail->pPosition;
		fourthEdge.pFace = pFace2;

		fourthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& fifthEdge = *fourthEdge.pNext;
		fifthEdge.pPrev = &fourthEdge;
		fifthEdge.pTail = new mmVertex();
		fifthEdge.pTail->pPosition = thirdEdge.pTail->pPosition;
		fifthEdge.pFace = pFace2;

		pNegZTop = &fifthEdge;

		fifthEdge.pNext = new mmHalfEdge();
		mmHalfEdge& sixthEdge = *fifthEdge.pNext;
		sixthEdge.pPrev = &fifthEdge;
		sixthEdge.pTail = new mmVertex();
		sixthEdge.pTail->pPosition = pPosXTop->pTail->pPosition;
		sixthEdge.pFace = pFace2;

		pNegZLeft = &sixthEdge;

		sixthEdge.pNext = &fourthEdge;
		fourthEdge.pPrev = &sixthEdge;

		pFace2->pEdge = &fourthEdge;
		pFace2->normal = normal;
	}
	pNegZLeft->pTwin = pPosXRight;
	pPosXRight->pTwin = pNegZLeft;
	pNegZBottom->pTwin = pNegYTop;
	pNegYTop->pTwin = pNegZBottom;
	pNegZRight->pTwin = pNegXLeft;
	pNegXLeft->pTwin = pNegZRight;
	pNegZTop->pTwin = pPosYTop;
	pPosYTop->pTwin = pNegZTop;

	mesh.Validate();
	mesh.UpdateRenderMesh(e);
}