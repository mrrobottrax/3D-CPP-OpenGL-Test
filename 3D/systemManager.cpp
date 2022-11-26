#include <systemManager.h>

#include <iostream>

#include <renderSystem.h>
#include <entityManager.h>
#include <linearVelocitySystem.h>

namespace systemManager {
	std::map<size_t, System*>* systems = new std::map<size_t, System*>();

	void addSystem(System* s)
	{
		systems->emplace(typeid(*s).hash_code(), s);
	}

	void registerSystems()
	{
		addSystem(new EntityManager());
		addSystem(new LinearVelocitySystem());
		addSystem(new RenderSystem(new CameraComponent()));
	}

	void updateSystems()
	{
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

	System* getSystem(size_t hash)
	{
		return systems->at(hash);
	}
}