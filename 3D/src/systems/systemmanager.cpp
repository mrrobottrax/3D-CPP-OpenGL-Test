#include <pch.h>

#include <systems/systemmanager.h>
#include <systems/systemslist.h>

#include <memory/entitymanager.h>

namespace SystemManager {
	std::map<size_t, System*>* systems = new std::map<size_t, System*>();

	void RegisterSystems()
	{
		AddSystem<VelocitySystem>();
		AddSystem<FreecamSystem>();

		AddSystem<RenderSystem>();
	}

	void UpdateSystems()
	{
		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			(*it).second->Update();
		}
	}

	void DeleteAllSystems()
	{
		for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
		{
			delete (*it).second;
		}

		delete systems;
		delete &EntityManager::GetInstance();
	}
}