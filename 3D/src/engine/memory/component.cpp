#pragma once

#include <pch.h>

#include <engine/memory/component.h>

Component::Component()
{
	Component::size = 0;
	Component::hash = 0;
	Component::offset = 0;

	Component::name[0] = 0;
}