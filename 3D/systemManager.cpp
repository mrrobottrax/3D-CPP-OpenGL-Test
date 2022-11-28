#include <systemManager.h>

#include <iostream>

#include <renderSystem.h>
#include <entityManager.h>
#include <linearVelocitySystem.h>

namespace systemManager {
	std::map<size_t, System*>* systems = new std::map<size_t, System*>();

	void registerSystems()
	{
		addSystem<LinearVelocitySystem>();
		addSystem<RenderSystem>();
	}

	void updateSystems()
	{
		EntityManager::getInstance().update();

		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it).second->update();
		}
	}

	void deleteAllSystems()
	{
		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete[](*it).second;
		}

		delete systems;
	}
}