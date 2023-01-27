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
	rootNode = 0;

	AddNode(-1, -2, DockingDirection::horizontal, 0.25f);

	AddLeaf(0);
	AddLeaf(0);

	SplitLeaf(1, DockingDirection::vertical, 0.75f);
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

int DockingTree::GetLiteralLeafIndex(int leafIndex)
{
	return abs(leafIndex) - 1;
}

void DockingTree::SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
{
	int newLeafIndex = AddLeaf(0);
	int newNodeIndex = AddNode(-(leafIndex + 1), -(newLeafIndex + 1), dir, ratio);

	leafArray[newLeafIndex].parentNodeIndex = newNodeIndex;

	DockingNode& parentNode = nodeArray[leafArray[leafIndex].parentNodeIndex];

	parentNode.frontIndex = newNodeIndex;

	std::cout << newNodeIndex << "\n";
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
	int windowSizeX, windowSizeY;
	int workingSizeX, workingSizeY;
	int workingPosX, workingPosY;
	glfwGetWindowSize(mainWindow, &windowSizeX, &windowSizeY);

	workingSizeX = windowSizeX;
	workingSizeY = windowSizeY;

	workingPosX = 0;
	workingPosY = 0;

	DrawNodeRecursiveDebug(0, workingPosX, workingPosY, workingSizeX, workingSizeY);
}

void DockingTree::DrawNodeRecursiveDebug(int nodeIndex, int workingPosX, int workingPosY, int workingSizeX, int workingSizeY)
{
	const int startSizeX = workingSizeX;
	const int startSizeY = workingSizeY;

	switch (nodeArray[nodeIndex].direction)
	{
	case horizontal:
		workingSizeX *= nodeArray[nodeIndex].ratio;
		break;

	case vertical:
		workingSizeY *= nodeArray[nodeIndex].ratio;
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
			workingPosX += workingSizeX;
			break;

		case vertical:
			workingPosY += workingSizeY;
			break;

		default:
			break;
		}
	}
	else
	{
		DrawNodeRecursiveDebug(nodeArray[nodeIndex].backIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}

	// Front
	if (IsLeaf(nodeArray[nodeIndex].frontIndex))
	{
		switch (nodeArray[nodeIndex].direction)
		{
		case horizontal:
			workingSizeX = startSizeX * (1 / nodeArray[nodeIndex].ratio);
			break;

		case vertical:
			workingSizeY = startSizeY * (1 / nodeArray[nodeIndex].ratio);
			break;

		default:
			break;
		}

		DrawLeafDebug(nodeArray[nodeIndex].frontIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}
	else
	{
		DrawNodeRecursiveDebug(nodeArray[nodeIndex].frontIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}
}

void DockingTree::DrawLeafDebug(int leafIndex, int workingPosX, int workingPosY, int workingSizeX, int workingSizeY)
{
	int literalLeafIndex = GetLiteralLeafIndex(leafIndex);

	ImGui::SetNextWindowPos(ImVec2(workingPosX, workingPosY));
	ImGui::SetNextWindowSize(ImVec2(workingSizeX, workingSizeY));
	bool pOpen = true;

	std::string name = std::to_string(literalLeafIndex);

	int R, G, B;
	RandomHueColor(literalLeafIndex, &R, &G, &B);
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(R, G, B, 0.5f));

	ImGui::Begin(name.c_str(), &pOpen, window_flags);
	ImGui::Text(name.c_str());
	ImGui::End();

	ImGui::PopStyleColor();
}