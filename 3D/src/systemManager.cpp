#include <systemManager.h>

#include <iostream>

#include <renderSystem.h>
#include <entityManager.h>
#include <velocitySystem.h>
#include <freecamSystem.h>

namespace systemManager {
	std::map<size_t, System*>* systems = new std::map<size_t, System*>();

	void registerSystems()
	{
		addSystem<VelocitySystem>();
		addSystem<FreecamSystem>();

		addSystem<RenderSystem>();
	}

	void updateSystems()
	{
		EntityManager::GetInstance().update();

		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it).second->update();
		}
	}

	void deleteAllSystems()
	{
		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete (*it).second;
		}

		delete systems;
		delete &EntityManager::GetInstance();
	}
}