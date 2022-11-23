#include <systemManager.h>
#include <iostream>

#include <renderSystem.h>
#include <entitySystem.h>

namespace systemManager {
	forward_list<System*>* systems = new forward_list<System*>();
	unsigned short systemCount;

	void registerSystems()
	{
		System* s;

		s = new RenderSystem();
		(*systems).push_front(s);
		++systemCount;

		s = new EntitySystem();
		(*systems).push_front(s);
		++systemCount;
	}

	void updateSystems()
	{
		for (forward_list<System*>::iterator it = (*systems).begin(); it != (*systems).end(); ++it)
		{
			(*it)->update();
		}
	}

	void deleteAllSystems()
	{
		for (forward_list<System*>::iterator it = (*systems).begin(); it != (*systems).end(); ++it)
		{
			delete (*it);
		}

		delete systems;
	}
}