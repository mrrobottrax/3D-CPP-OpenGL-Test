#include <malletpch.h>

#include <gl/glutil.h>
#include <imgui/imguiutil.h>
#include <ui/docking/dockingtree.h>

#include <debug/debugcolorcycle.h>

const static ImGuiWindowFlags window_flags =ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoMove;

DockingTree::DockingTree() : nodeArray(), leafArray()
{
	int leaf = AddLeaf(-1);
	rootNode = -(leaf + 1);

	leaf = SplitLeaf(leaf, DockingDirection::vertical, 0.8f);
	leaf = SplitLeaf(leaf, DockingDirection::vertical, 0.8f);
	//leaf = SplitLeaf(1, DockingDirection::horizontal, 0.75f);
}

DockingTree::~DockingTree()
{
	
}

void DockingTree::UpdateTree()
{
}

bool DockingTree::IsLeaf(int index)
{
	return index < 0;
}

int DockingTree::SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
{
	int newLeafIndex = AddLeaf(0);
	int newNodeIndex = AddNode(-(leafIndex + 1), -(newLeafIndex + 1), dir, ratio);

	leafArray[newLeafIndex].parentNodeIndex = newNodeIndex;

	if (leafArray[leafIndex].parentNodeIndex < 0)
	{
		rootNode = newNodeIndex;
		return newLeafIndex;
	}

	DockingNode& parentNode = nodeArray[leafArray[leafIndex].parentNodeIndex];

	parentNode.frontIndex = newNodeIndex;

	return newLeafIndex;
}

int DockingTree::AddNode(int backIndex, int frontIndex, DockingDirection dir, float ratio)
{
	// Find free space in array
	int index;
	for (index = 0; index < MAX_PARTITIONS; index++)
	{
		if (!(nodeArray[index].flags & DockingNodeFlags::nodeIsUsed))
		{
			nodeArray[index] = DockingNode(backIndex, frontIndex, dir, ratio);
			return index;
		}
	}
}

int DockingTree::AddLeaf(int parentNodeIndex)
{
	// Find free space in array
	int index;
	for (index = 0; index < MAX_PARTITIONS; index++)
	{
		if (!(leafArray[index].flags & DockingLeafFlags::leafIsUsed))
		{
			leafArray[index] = DockingLeaf(parentNodeIndex);
			return index;
		}
	}
}

void DockingTree::DrawTreeDebug()
{
	srand(0);

	int windowSizeX, windowSizeY;
	int workingSizeX, workingSizeY;
	int workingPosX, workingPosY;
	glfwGetWindowSize(mainWindow, &windowSizeX, &windowSizeY);

	workingSizeX = windowSizeX;
	workingSizeY = windowSizeY;

	workingPosX = 0;
	workingPosY = 0;

	DrawNodeRecursiveDebug(rootNode, workingPosX, workingPosY, workingSizeX, workingSizeY);
}

void DockingTree::DrawNodeRecursiveDebug(int nodeIndex, int workingPosX, int workingPosY, int workingSizeX, int workingSizeY)
{
	if (IsLeaf(nodeIndex))
	{
		DrawLeafDebug(nodeIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
		return;
	}

	const int startSizeX = workingSizeX;
	const int startSizeY = workingSizeY;

	// Scale window
	switch (nodeArray[nodeIndex].direction)
	{
	case horizontal:
		workingSizeY *= nodeArray[nodeIndex].ratio;
		break;

	case vertical:
		workingSizeX *= nodeArray[nodeIndex].ratio;
		break;

	default:
		break;
	}

	// Back
	if (IsLeaf(nodeArray[nodeIndex].backIndex))
	{
		DrawLeafDebug(nodeArray[nodeIndex].backIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);

		switch (nodeArray[nodeIndex].direction)
		{
		case horizontal:
			workingPosY += workingSizeY;
			break;

		case vertical:
			workingPosX += workingSizeX;
			break;

		default:
			break;
		}
	}
	else
	{
		DrawNodeRecursiveDebug(nodeArray[nodeIndex].backIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}

	// Inverse scale window
	switch (nodeArray[nodeIndex].direction)
	{
	case horizontal:
		workingSizeY = startSizeY * (1.0f - nodeArray[nodeIndex].ratio);
		break;

	case vertical:
		workingSizeX = startSizeX * (1.0f - nodeArray[nodeIndex].ratio);
		break;

	default:
		break;
	}

	// Front
	if (IsLeaf(nodeArray[nodeIndex].frontIndex))
	{
		DrawLeafDebug(nodeArray[nodeIndex].frontIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}
	else
	{
		DrawNodeRecursiveDebug(nodeArray[nodeIndex].frontIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}
}

void DockingTree::DrawLeafDebug(int leafIndex, int workingPosX, int workingPosY, int workingSizeX, int workingSizeY)
{
	int literalLeafIndex = abs(leafIndex) - 1;

	ImGui::SetNextWindowPos(ImVec2(workingPosX, workingPosY));
	ImGui::SetNextWindowSize(ImVec2(workingSizeX, workingSizeY));
	bool pOpen = true;

	std::string name = std::to_string(literalLeafIndex);

	int R, G, B;
	RandomHueColor(&R, &G, &B);
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(R, G, B, 0.7f));

	ImGui::Begin(name.c_str(), &pOpen, window_flags);
	ImGui::Text(name.c_str());
	ImGui::End();

	ImGui::PopStyleColor();
}