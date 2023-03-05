#include <pch.h>

#include <systems/systemmanager.h>
#include <systems/systemslist.h>

#include <memory/entitymanager.h>

SystemManager::SystemManager()
{
	systems = new std::map<size_t, System*>;
}

SystemManager::~SystemManager()
{
}

void SystemManager::UpdateSystems()
{
	for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
	{
		(*it).second->Update();
	}
}

void SystemManager::DeleteAllSystems()
{
	for (std::map<size_t, System*>::iterator it = systems->begin(); it != systems->end(); ++it)
	{
		delete (*it).second;
	}

	delete systems;
}