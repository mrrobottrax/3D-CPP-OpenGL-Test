#pragma once
#include <memory/entitymanager.h>
#include <systems/systemmanager.h>
#include <input/inputmanager.h>

void InitManagers();
void DeleteManagers();

//extern EntityManager*	entityManager;
//extern SystemManager*	systemManager;
extern InputManager*	inputManager;