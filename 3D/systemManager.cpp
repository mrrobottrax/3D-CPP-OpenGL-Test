#include <systemManager.h>

#include <iostream>
#include <set>

#include <renderSystem.h>
#include <entitySystem.h>

namespace systemManager {
	set<System*>* systems = new set<System*>();
	unsigned short systemCount;

	void addSystem(System* s)
	{
		systems->emplace(s);
		++systemCount;
	}

	void registerSystems()
	{
		addSystem(new EntitySystem());
		addSystem(new RenderSystem());
	}

	template <class T>
	T getSystem()
	{
		set<System>::iterator it = systems->find(T);
	}

	void updateSystems()
	{
		for (set<System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it)->update();
		}
	}

	void deleteAllSystems()
	{
		for (set<System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete (*it);
		}

		delete systems;
	}
}