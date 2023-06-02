#include <pch.h>

#include <systems/systemmanager.h>
#include <systems/systemslist.h>

#include <memory/entitymanager.h>

SystemManager::SystemManager()
{
	systems = std::map<size_t, System*>();
	tickSystems = std::map<size_t, System*>();

	auto advance = [this](Console& console)
	{
		tickTimer = 0;
		UpdateSystems();
		return;
	};
	console.AddCommand("advance", advance);
}

SystemManager::~SystemManager()
{
}

void SystemManager::UpdateSystems()
{
	tickTimer -= timeManager.GetDeltaTimeDouble();
	if (tickTimer <= 0)
	{
		tickTimer = timeManager.GetFixedDeltaTimeDouble();

		for (std::map<size_t, System*>::iterator it = tickSystems.begin(); it != tickSystems.end(); ++it)
		{
			(*it).second->Update();
		}
	}

	for (std::map<size_t, System*>::iterator it = systems.begin(); it != systems.end(); ++it)
	{
		(*it).second->Update();
	}
}

void SystemManager::DeleteAllSystems()
{
	for (std::map<size_t, System*>::iterator it = systems.begin(); it != systems.end(); ++it)
	{
		delete (*it).second;
	}

	for (std::map<size_t, System*>::iterator it = tickSystems.begin(); it != tickSystems.end(); ++it)
	{
		delete (*it).second;
	}
}