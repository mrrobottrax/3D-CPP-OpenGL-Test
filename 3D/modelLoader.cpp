#include <modelLoader.h>

#include <iostream>

ModelLoader* ModelLoader::instance{ nullptr };
std::mutex ModelLoader::mutex;

/*
ModelLoader& ModelLoader::GetInstance()
{
	std::lock_guard<std::mutex> lock(mutex);
	if (instance == nullptr)
	{
		instance = new ModelLoader();
	}

	return *instance;
};
*/