#pragma once

#include <systems/system.h>

class SystemManager {
public:
	SystemManager();
	~SystemManager();

public:
	std::map<size_t, System*>* systems;

public:
	void UpdateSystems();
	void DeleteAllSystems();

public:
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
};