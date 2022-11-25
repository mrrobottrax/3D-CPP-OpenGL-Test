#include <systemManager.h>

#include <iostream>
#include <typeindex>
#include <map>

#include <renderSystem.h>
#include <entitySystem.h>

namespace systemManager {
	using namespace std;

	map<type_index, System*>* systems = new map<type_index, System*>();
	unsigned short systemCount;

	void addSystem(System* s)
	{
		systems->emplace(typeid(*s), s);
		++systemCount;
	}

	void registerSystems()
	{
		addSystem(new EntitySystem());
		addSystem(new RenderSystem(new CameraComponent()));
	}

	void updateSystems()
	{
		for (map<type_index, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it).second->update();
		}
	}

	void deleteAllSystems()
	{
		for (map<type_index, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete[](*it).second;
		}

		delete systems;
	}
}