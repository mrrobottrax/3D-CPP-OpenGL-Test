#include "pch.h"
#include "systemmanager.h"

SystemManager::SystemManager()
{
	systems = std::vector<System*>();
	tickSystems = std::vector<System*>();

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

void SystemManager::InitSystems()
{
	for (auto it = systems.begin(); it != systems.end(); ++it)
	{
		(*it)->Init();
	}
}

void SystemManager::UpdateSystems()
{
	tickTimer -= timeManager.GetDeltaTimeDouble();
	if (tickTimer <= 0)
	{
		tickTimer = timeManager.GetFixedDeltaTimeDouble();

		for (auto it = tickSystems.begin(); it != tickSystems.end(); ++it)
		{
			(*it)->Update();
		}
	}

	for (auto it = systems.begin(); it != systems.end(); ++it)
	{
		(*it)->Update();
	}
}