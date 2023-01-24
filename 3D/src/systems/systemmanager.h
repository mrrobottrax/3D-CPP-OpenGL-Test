#pragma once

#include <systems/system.h>

namespace systemManager {
	extern std::map<size_t, System*>* systems;

	void RegisterSystems();
	void UpdateSystems();
	void DeleteAllSystems();

	template <class T>
	inline T* GetSystem()
	{
		return (T*)systems->at(typeid(T).hash_code());
	}

	template <class T>
	inline void AddSystem()
	{
		systems->emplace(typeid(T).hash_code(), new T());
	}
}