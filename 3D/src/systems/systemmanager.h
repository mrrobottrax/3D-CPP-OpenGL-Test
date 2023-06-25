#pragma once

#include <systems/system.h>

class SystemManager {
public:
	SystemManager();
	~SystemManager();

private:
	std::vector<System*> systems;
	std::vector<System*> tickSystems;

	double tickTimer = 0;

public:
	void InitSystems();
	void UpdateSystems();

public:
	template <class T>
	inline void AddSystem(System& pSystem)
	{
		systems.push_back(&pSystem);
	}

	template <class T>
	inline void AddTickSystem(System& pSystem)
	{
		tickSystems.push_back(&pSystem);
	}
}; inline SystemManager systemManager;