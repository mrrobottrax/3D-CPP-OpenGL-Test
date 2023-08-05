#include "malletpch.h"
#include "dockingtree.h"

#include <gl/glutil.h>
#include <ui/menubar.h>

DockingLeaf* DockingTree::AddLeaf(MalletWindow* pWindow, DockingLeaf* pLeafToSplit, SplitDirection direction,
	float ratio, bool flip)
{
	DockingLeaf* pNewLeaf = new DockingLeaf(pWindow);

	// If leaf is new base node
	if (!pLeafToSplit)
	{
		Clear();

		pBaseNode = pNewLeaf;

		return pNewLeaf;
	}

	DockingSplit* pSplit = new DockingSplit();

	pSplit->splitRatio = ratio;
	pSplit->direction = direction;

	if (!flip)
	{
		pSplit->back = pLeafToSplit;
		pSplit->front = pNewLeaf;
	}
	else
	{
		pSplit->back = pNewLeaf;
		pSplit->front = pLeafToSplit;
	}

	pSplit->pParentSplit = pLeafToSplit->pParentSplit;
	pLeafToSplit->pParentSplit = pSplit;
	pNewLeaf->pParentSplit = pSplit;

	// If split is base node
	if (!pSplit->pParentSplit)
	{
		pBaseNode = pSplit;
		return pNewLeaf;
	}

	// Set parent node to point to split
	if (pSplit->pParentSplit->back == pLeafToSplit)
	{
		pSplit->pParentSplit->back = pSplit;
	}
	else if (pSplit->pParentSplit->front == pLeafToSplit)
	{
		pSplit->pParentSplit->front = pSplit;
	}

	return pNewLeaf;
}

void UpdateSizeRecursive(DockingNode* pNode, int width, int height, int posX, int posY)
{
	// If leaf
	if (pNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pNode);

		leaf.size[0] = width;
		leaf.size[1] = height;

		leaf.position[0] = posX;
		leaf.position[1] = posY;

		leaf.pWindow->OnResize();

		return;
	}

	// If split
	DockingSplit& split = *reinterpret_cast<DockingSplit*>(pNode);

	switch (split.direction)
	{
	case SplitDirection::horizontal:
		{
			const int heightXRatio = static_cast<int>(height * split.splitRatio);

			split.min = posY;
			split.max = posY + height;
			split.splitPosition = posY + heightXRatio;

			UpdateSizeRecursive(split.back, width, heightXRatio, posX, posY);
			UpdateSizeRecursive(split.front, width, static_cast<int>(height * (1.0f - split.splitRatio)), posX, split.splitPosition);
		}
		break;

	case SplitDirection::vertical:
		{
			const int widthXRatio = static_cast<int>(width * split.splitRatio);

			split.min = posX;
			split.max = posX + width;
			split.splitPosition = posX + widthXRatio;

			UpdateSizeRecursive(split.back, widthXRatio, height, posX, posY);
			UpdateSizeRecursive(split.front, static_cast<int>(width * (1.0f - split.splitRatio)), height, split.splitPosition, posY);
		}
		break;

	default:
		break;
	}
}

void DockingTree::UpdateSize()
{
	if (!pBaseNode)
		return;

	int width, height;
	glfwGetWindowSize(pMainWindow, &width, &height);

	UpdateSizeRecursive(pBaseNode, width, height - menuBarSize, 0, menuBarSize);
}

void DrawTreeRecursive(DockingNode* pNode)
{
	if (pNode->isLeaf)
	{
		DockingLeaf& leaf = *reinterpret_cast<DockingLeaf*>(pNode);

		leaf.pWindow->Draw();

		return;
	}

	DockingSplit& split = *reinterpret_cast<DockingSplit*>(pNode);

	DrawTreeRecursive(split.back);
	DrawTreeRecursive(split.front);
}

void DockingTree::DrawTree()
{
	if (!pBaseNode)
		return;

	DrawTreeRecursive(pBaseNode);
}

DockingNode* GetNodeUnderPositionRecursive(DockingNode* pNode, double xPos, double yPos, bool selectSplits)
{
	// Stop when we hit a leaf
	if (pNode->isLeaf)
	{
		return pNode;
	}

	DockingSplit& split = *reinterpret_cast<DockingSplit*>(pNode);

	int dist = 0;

	switch (split.direction)
	{
	case SplitDirection::horizontal:
		dist = static_cast<int>(yPos - split.splitPosition);
		break;

	case SplitDirection::vertical:
		dist = static_cast<int>(xPos - split.splitPosition);
		break;
	}

	// Return split when mouse is pretty much on it
	if (selectSplits && abs(dist) <= splitSelectDistance)
	{
		return pNode;
	}

	if (dist > 0)
	{
		return GetNodeUnderPositionRecursive(split.front, xPos, yPos, selectSplits);
	}
	else
	{
		return GetNodeUnderPositionRecursive(split.back, xPos, yPos, selectSplits);
	}
}

DockingNode* DockingTree::GetNodeUnderMouse(bool selectSplits)
{
	if (!pBaseNode)
		return nullptr;

	double xPos, yPos;
	glfwGetCursorPos(pMainWindow, &xPos, &yPos);

	return GetNodeUnderPositionRecursive(pBaseNode, xPos, yPos, selectSplits);
}
