#include <malletpch.h>

#include <ui/malletuilayer.h>

MalletUiLayer::MalletUiLayer() : tree()
{

}

MalletUiLayer::~MalletUiLayer()
{

}

void MalletUiLayer::DrawTreeDebug()
{
	tree.DrawTreeDebug();
}

void MalletUiLayer::SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
{
	tree.SplitLeaf(leafIndex, dir, ratio);
}