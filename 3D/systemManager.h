#pragma once

#include <system.h>
#include <forward_list>

namespace systemManager {
	using namespace std;

	void registerSystems();
	void updateSystems();
	void deleteAllSystems();
}