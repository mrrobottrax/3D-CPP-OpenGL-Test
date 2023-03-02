#include <pch.h>
#include <managers.h>

//EntityManager* entityManager;
SystemManager* systemManager;
InputManager* inputManager;

void InitManagers()
{
	systemManager	= new SystemManager();
	//entityManager	= new EntityManager();
	inputManager	= new InputManager();
}

void DeleteManagers()
{
	delete systemManager;
	//delete entityManager;
	delete inputManager;
}