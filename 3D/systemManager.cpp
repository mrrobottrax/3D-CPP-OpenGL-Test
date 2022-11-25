#include <systemManager.h>

#include <iostream>

#include <renderSystem.h>
#include <entitySystem.h>

namespace systemManager {
	std::map<std::type_index, System*>* systems = new std::map<std::type_index, System*>();

	void addSystem(System* s)
	{
		systems->emplace(typeid(*s), s);
	}

	void registerSystems()
	{
		addSystem(new EntitySystem());
		addSystem(new RenderSystem(new CameraComponent()));
	}

	void updateSystems()
	{
		for (std::map<std::type_index, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it).second->update();
		}
	}

	void deleteAllSystems()
	{
		for (std::map<std::type_index, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete[](*it).second;
		}

		delete systems;
	}
}