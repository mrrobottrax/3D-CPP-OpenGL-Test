#pragma once

#include <engine/system.h>

namespace systemManager {
	extern std::map<size_t, System*>* systems;

	void registerSystems();
	void updateSystems();
	void deleteAllSystems();

	template <class T>
	inline T* getSystem()
	{
		return (T*)systems->at(typeid(T).hash_code());
	}

	template <class T>
	inline void addSystem()
	{
		systems->emplace(typeid(T).hash_code(), new T());
	}
}