#include "malletpch.h"
#include "viewport3d.h"

#include <systems/systemmanager.h>
#include <systems/rendersystem.h>

void Viewport3D::Draw(DockingLeaf& leaf, int leafIndex)
{
	
}

void Viewport3D::OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight)
{
	RenderSystem* rs = SystemManager::GetSystem<RenderSystem>();

	if (rs == nullptr)
		return;

	rs->UpdateMatrixAspect(leaf.absSize[0], leaf.absSize[1]);

	glViewport((GLint)leaf.absPos[0], windowHeight - (leaf.absPos[1] + leaf.absSize[1]), (GLsizei)leaf.absSize[0], (GLsizei)leaf.absSize[1]);
}