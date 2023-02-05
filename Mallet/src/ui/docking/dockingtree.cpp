#include <malletpch.h>

#include <gl/glutil.h>
#include <imgui/imguiutil.h>
#include <ui/docking/dockingtree.h>

#include <debug/debugcolorcycle.h>

// Windows - TEMP
#include <ui/windows/viewport.h>

//#define DRAW_DEBUG

#define NODE_MIN_MOUSE_DIST 5

DockingTree::DockingTree() : nodeArray(), leafArray()
{
	int leaf = AddLeaf(-1, new Viewport(ViewportMode::perspective));
	rootNode = -(leaf + 1);

	leaf = SplitLeaf(leaf, DockingDirection::vertical, 0.5, new Viewport(ViewportMode::top));
	leaf = SplitLeaf(0, DockingDirection::horizontal, 0.5, new Viewport(ViewportMode::front));
	leaf = SplitLeaf(1, DockingDirection::horizontal, 0.5, new Viewport(ViewportMode::side));

	RecalculateSizes();
}

DockingTree::~DockingTree()
{
}

void DockingTree::PrintTree()
{
	std::cout << "Start Node: " << rootNode << "\n\n";

	// Print nodes
	std::cout << "Nodes:" << "\n";
	for (int i = 0; i < MAX_PARTITIONS; i++)
	{
		if (!(nodeArray[i].flags & DockingNodeFlags::nodeIsUsed))
			continue;

		std::cout << "Node " << i << "\n";
		std::cout << "   " << "Flags: " << "\n";
		std::cout << "      " << nodeArray[i].flags << "\n";
		std::cout << "   " << "Direction: " << "\n";
		std::cout << "      " << nodeArray[i].direction << "\n";
		std::cout << "   " << "Ratio: " << "\n";
		std::cout << "      " << nodeArray[i].ratio << "\n";
		std::cout << "   " << "Back: " << "\n";
		std::cout << "      " << nodeArray[i].backIndex << "\n";
		std::cout << "   " << "Front: " << "\n";
		std::cout << "      " << nodeArray[i].frontIndex << "\n";
		std::cout << "   " << "Offset: " << "\n";
		std::cout << "      " << nodeArray[i].absOffset << "\n";
		std::cout << "   " << "Parent: " << "\n";
		std::cout << "      " << nodeArray[i].parentNodeIndex << "\n";
	}

	// Print leafs
	std::cout << "Leafs:" << "\n";
	for (int i = 0; i < MAX_PARTITIONS; i++)
	{
		if (!(leafArray[i].flags & DockingLeafFlags::leafIsUsed))
			continue;

		std::cout << "Leaf " << i << "\n";
		std::cout << "   " << "Flags: " << "\n";
		std::cout << "      " << leafArray[i].flags << "\n";
		std::cout << "   " << "Parent Node: " << "\n";
		std::cout << "      " << leafArray[i].parentNodeIndex << "\n";
	}
}

bool DockingTree::IsLeaf(int index)
{
	return index < 0;
}

int DockingTree::SplitLeaf(int leafIndex, DockingDirection dir, float ratio, MalletWindow* window)
{
	DockingLeaf& leafToSplit = leafArray[leafIndex];
	DockingNode& parentNode = nodeArray[leafToSplit.parentNodeIndex];
	bool isLeafToSplitBack = -(leafIndex + 1) == parentNode.backIndex;

	int newLeafIndex = AddLeaf(-1, window);
	int newNodeIndex = AddNode(leafToSplit.parentNodeIndex, -(leafIndex + 1), -(newLeafIndex + 1), dir, ratio);
	
	DockingLeaf& newLeaf = leafArray[newLeafIndex];

	// Leaf was not connected to any node
	if (leafToSplit.parentNodeIndex < 0)
	{
		rootNode = newNodeIndex;
	}

	leafToSplit.parentNodeIndex = newNodeIndex;
	newLeaf.parentNodeIndex = newNodeIndex;

	if (isLeafToSplitBack)
	{
		parentNode.backIndex = newNodeIndex;
	}
	else
	{
		parentNode.frontIndex = newNodeIndex;
	}

	return newLeafIndex;
}

void DockingTree::RecalculateSizes()
{
	int windowWidth, windowHeight;
	glfwGetWindowSize(mainWindow, &windowWidth, &windowHeight);

	if (IsLeaf(rootNode))
	{
		DockingLeaf& leaf = leafArray[abs(rootNode) - 1];

		leaf.absPos[0] = 0;
		leaf.absPos[1] = 0;

		leaf.absSize[0] = windowWidth;
		leaf.absSize[1] = windowHeight;

		leaf.window->OnResize(leaf, windowWidth, windowHeight);

		return;
	}

	GetSizesRecursive(rootNode, 0, 0, windowWidth, windowHeight, windowWidth, windowHeight);
}

void DockingTree::GetSizesRecursive(int index, int lowerBoundX, int lowerBoundY, int higherBoundX, int higherBoundY,
	int& windowWidth, int& windowHeight)
{
	if (IsLeaf(index))
	{
		DockingLeaf& leaf = leafArray[abs(index) - 1];

		leaf.absPos[0] = lowerBoundX;
		leaf.absPos[1] = lowerBoundY;

		leaf.absSize[0] = higherBoundX - lowerBoundX;
		leaf.absSize[1] = higherBoundY - lowerBoundY;

		if (leaf.window != nullptr)
			leaf.window->OnResize(leaf, windowWidth, windowHeight);

		return;
	}

	DockingNode& node = nodeArray[index];

	switch (node.direction)
	{
	case horizontal:
		node.absOffset = int(node.ratio * (higherBoundY - lowerBoundY)) + lowerBoundY;
		break;

	case vertical:
		node.absOffset = int(node.ratio * (higherBoundX - lowerBoundX)) + lowerBoundX;
		break;

	default:
		break;
	}

	// Back
	int backHigherBoundX = higherBoundX;
	int backHigherBoundY = higherBoundY;
	{
		switch (node.direction)
		{
		case horizontal:
			backHigherBoundY = node.absOffset;
			break;

		case vertical:
			backHigherBoundX = node.absOffset;
			break;

		default:
			break;
		}

		GetSizesRecursive(node.backIndex, lowerBoundX, lowerBoundY, backHigherBoundX, backHigherBoundY, windowWidth, windowHeight);
	}

	// Front
	{
		int frontHigherBoundX = lowerBoundX;
		int frontHigherBoundY = lowerBoundY;

		switch (node.direction)
		{
		case horizontal:
			frontHigherBoundY = backHigherBoundY;
			break;

		case vertical:
			frontHigherBoundX = backHigherBoundX;
			break;

		default:
			break;
		}

		GetSizesRecursive(node.frontIndex, frontHigherBoundX, frontHigherBoundY, higherBoundX, higherBoundY, windowWidth, windowHeight);
	}
}

int DockingTree::AddNode(int parentNodeIndex, int backIndex, int frontIndex, DockingDirection dir, float ratio)
{
	// Find free space in array
	int index;
	for (index = 0; index < MAX_PARTITIONS; index++)
	{
		if (!(nodeArray[index].flags & DockingNodeFlags::nodeIsUsed))
		{
			nodeArray[index] = DockingNode(parentNodeIndex, backIndex, frontIndex, dir, ratio);
			return index;
		}
	}

	return -1;
}

int DockingTree::AddLeaf(int parentNodeIndex, MalletWindow* window)
{
	// Find free space in array
	int index;
	for (index = 0; index < MAX_PARTITIONS; index++)
	{
		if (!(leafArray[index].flags & DockingLeafFlags::leafIsUsed))
		{
			DockingLeaf leaf(parentNodeIndex, window);
			leafArray[index] = leaf;
			leaf.window = nullptr;
			return index;
		}
	}

	return -1;
}

void DockingTree::DrawTree()
{
	srand(0);

	int windowSizeX, windowSizeY;
	float workingSizeX, workingSizeY;
	float workingPosX, workingPosY;
	glfwGetWindowSize(mainWindow, &windowSizeX, &windowSizeY);

	workingSizeX = float(windowSizeX);
	workingSizeY = float(windowSizeY);

	workingPosX = 0;
	workingPosY = 0;

	DrawNodeRecursive(rootNode, workingPosX, workingPosY, workingSizeX, workingSizeY);
}

void DockingTree::DrawNodeRecursive(int nodeIndex, float workingPosX, float workingPosY, float workingSizeX, float workingSizeY)
{
	if (IsLeaf(nodeIndex))
	{
		#ifdef DRAW_DEBUG
			DrawLeafDebug(abs(nodeIndex) - 1, workingPosX, workingPosY, workingSizeX, workingSizeY);
		#else
			DrawLeaf(abs(nodeIndex) - 1, workingPosX, workingPosY, workingSizeX, workingSizeY);
		#endif // DRAW_DEBUG

		return;
	}

	const float startSizeX = workingSizeX;
	const float startSizeY = workingSizeY;

	// Back
	{
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

		DrawNodeRecursive(nodeArray[nodeIndex].backIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}

	// Move window
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

	// Front
	{
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

		DrawNodeRecursive(nodeArray[nodeIndex].frontIndex, workingPosX, workingPosY, workingSizeX, workingSizeY);
	}
}

void DockingTree::DrawLeafDebug(int leafIndex, float workingPosX, float workingPosY, float workingSizeX, float workingSizeY)
{
	DockingLeaf& leaf = leafArray[leafIndex];

	float posX, posY;
	float sizeX, sizeY;

	posX = roundf(workingPosX);
	posY = roundf(workingPosY);

	sizeX = roundf(workingSizeX);
	sizeY = roundf(workingSizeY);

	ImGui::SetNextWindowPos(ImVec2(posX, posY));
	ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY));
	bool pOpen = true;

	std::string name = std::to_string(leafIndex);

	float R, G, B;
	RandomHueColor(&R, &G, &B);
	ImVec4 color = ImVec4(R, G, B, 0.5f);
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, color);

	ImGui::Begin(name.c_str(), &pOpen, window_flags);
	ImGui::Text(name.c_str());
	ImGui::Text("Color: %f, %f, %f", R, G, B);
	ImGui::Text("Position: %d, %d", leaf.absPos[0], leaf.absPos[1]);
	ImGui::Text("Size: %d, %d", leaf.absSize[0], leaf.absSize[1]);
	ImGui::End();

	ImGui::PopStyleColor();
}

void DockingTree::DrawLeaf(int leafIndex, float workingPosX, float workingPosY, float workingSizeX, float workingSizeY)
{
	DockingLeaf& leaf = leafArray[leafIndex];
	
	if (leaf.window == nullptr)
	{
		ImGui::SetNextWindowPos(ImVec2(float(leaf.absPos[0]), float(leaf.absPos[1])));
		ImGui::SetNextWindowSize(ImVec2(float(leaf.absSize[0]), float(leaf.absSize[1])));

		std::string name = std::to_string(leafIndex);

		ImGui::Begin(name.c_str(), (bool*)0, window_flags);
		ImGui::End();

		return;
	}

	leaf.window->Draw(leaf, leafIndex);
}

int DockingTree::SelectNodeRecursive(int nodeIndex, float mousePosX, float mousePosY)
{
	if (IsLeaf(nodeIndex))
	{
		return nodeIndex;
	}

	DockingNode& node = nodeArray[nodeIndex];

	float usedPos;

	switch (node.direction)
	{
	case horizontal:
		usedPos = mousePosY;
		break;

	case vertical:
		usedPos = mousePosX;
		break;

	default:
		usedPos = 0;
		break;
	}

	if (abs(usedPos - node.absOffset) <= NODE_MIN_MOUSE_DIST)
	{
		return nodeIndex;
	}

	int nextIndex;
	if (usedPos > node.absOffset)
	{
		nextIndex = node.frontIndex;
	}
	else
	{
		nextIndex = node.backIndex;
	}

	return SelectNodeRecursive(nextIndex, mousePosX, mousePosY);
}

int DockingTree::SelectNode(float mousePosX, float mousePosY)
{
	return SelectNodeRecursive(rootNode, mousePosX, mousePosY);
}
