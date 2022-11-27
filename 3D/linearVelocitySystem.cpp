#include <linearVelocitySystem.h>

#include <systemManager.h>
#include <entityManager.h>
#include <positionComponent.h>
#include <linearVelocityComponent.h>

LinearVelocitySystem::LinearVelocitySystem()
{
}

LinearVelocitySystem::~LinearVelocitySystem()
{
}

void LinearVelocitySystem::update()
{
	EntityManager* em = (EntityManager*)systemManager::getSystem(typeid(EntityManager).hash_code());

	Component positionComponent;
	positionComponent.
	em->findChunkArchetypesWithComponent(PositionComponent());

	//em->findChunkArchetypesWithComponent();
}
