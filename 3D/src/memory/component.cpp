#include "pch.h"
#include "component.h"

Component::Component()
{
	Component::size = 0;
	Component::hash = 0;
	Component::offset = 0;

#ifdef DEBUG
	Component::name[0] = 0;
#endif // DEBUG
}