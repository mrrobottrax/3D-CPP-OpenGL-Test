#pragma once

#include <system.h>
#include <map>
#include <typeindex>

namespace systemManager {
	extern std::map<std::type_index, System*>* systems;

	void addSystem(System*);
	void registerSystems();
	void updateSystems();
	void deleteAllSystems();
}