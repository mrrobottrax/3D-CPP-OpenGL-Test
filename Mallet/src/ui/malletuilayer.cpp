#include <malletpch.h>

#include <ui/malletuilayer.h>

MalletUiLayer::MalletUiLayer() : tree()
{

}

MalletUiLayer::~MalletUiLayer()
{

}

void MalletUiLayer::DrawTree()
{
	tree.DrawTree();
}

void MalletUiLayer::SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
{
	tree.SplitLeaf(leafIndex, dir, ratio);
}