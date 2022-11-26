#pragma once

#include <system.h>
#include <map>
#include <typeindex>

namespace systemManager {
	extern std::map<size_t, System*>* systems;

	void addSystem(System*);
	void registerSystems();
	void updateSystems();
	void deleteAllSystems();
	System* getSystem(size_t);
}