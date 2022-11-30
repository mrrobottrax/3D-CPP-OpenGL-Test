#pragma once

#include <tinygltf/tiny_gltf.h>
#include <mutex>

class ModelLoader {

private:
	static ModelLoader* instance;
	static std::mutex mutex;

public:
	//~ModelLoader();

	// Singleton
	//ModelLoader(ModelLoader& other) = delete;
	//void operator=(const ModelLoader&) = delete;
	//static ModelLoader& GetInstance();

private:
	tinygltf::TinyGLTF loader;
};