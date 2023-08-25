#pragma once

#include <Windows.h>
#include <glm.hpp>
#include <algorithm>
#include <gtc/quaternion.hpp>
#include <forward_list>
#include <unordered_set>
#include <map>
#include <typeindex>
#include <iostream>
#include <fstream>
#include <json.hpp>
#include <string>
#include <typeindex>
#include <iostream>
#include <mutex>
#include <vector>
#include <string>
#include <stack>
#include <list>

// IMGUI should be loaded before OpenGL
#include <imgui/imguiload.h>
#include <gl/glload.h>

// I want to kill myself
#ifdef WIN32
#undef min
#undef max
#endif