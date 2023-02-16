#pragma once

#include <input/console.h>

#define MAX_KEYS 256

struct Key
{
	bool pressed;

	char binding[MAX_CONSOLE_INPUT_LENGTH];
};

enum KeyCode
{

};

struct Button
{
	char down[2];
};

#define MAX_BUTTONS 8

enum ButtonCode
{
	MoveForward,
	MoveBack,
	MoveLeft,
	MoveRight,

	LookUp,
	LookDown,
	LookLeft,
	LookRight,
};