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
	None = 0,

	UpArrow = 1,
	DownArrow = 2,
	LeftArrow = 3,
	RightArrow = 4,

	// 48 - 57 : Numbers

	// 65 - 90 : Letters
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