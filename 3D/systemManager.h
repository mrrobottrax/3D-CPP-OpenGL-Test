#pragma once

#include <system.h>

namespace systemManager {
	using namespace std;

	void addSystem(System*);
	void registerSystems();
	void updateSystems();
	void deleteAllSystems();
}