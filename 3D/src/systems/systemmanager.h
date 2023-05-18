#pragma once

#include <systems/system.h>

class SystemManager {
public:
	SystemManager();
	~SystemManager();

private:
	std::map<size_t, System*> systems;
	std::map<size_t, System*> tickSystems;

	double tickTimer = 0;

public:
	void UpdateSystems();
	void DeleteAllSystems();

public:
	template <class T>
	inline T& GetSystem()
	{
		return *(T*)systems.at(typeid(T).hash_code());
	}

	template <class T>
	inline T& GetTickSystem()
	{
		return *(T*)tickSystems.at(typeid(T).hash_code());
	}

	template <class T>
	inline T& AddSystem()
	{
		T* pSystem = new T();
		systems.emplace(typeid(T).hash_code(), pSystem);
		return *pSystem;
	}

	template <class T>
	inline T& AddTickSystem()
	{
		T* pSystem = new T();
		tickSystems.emplace(typeid(T).hash_code(), pSystem);
		return *pSystem;
	}
}; inline SystemManager systemManager;