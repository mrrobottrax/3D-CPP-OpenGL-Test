#include <pch.h>
#include <systems/unscaledvelocitysystem.h>
#include <systems/velocitysystem.h>
#include <components/unscaledvelocitycomponent.h>

void UnscaledVelocitySystem::Update()
{
	EntityManager& em = entityManager;

	const std::vector<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().Init<UnscaledVelocityComponent>());
	VelocitySystem::UpdatePositions(archetypes, timeManager.GetUnscaledDeltaTime(), false);
	delete archetypes;
}