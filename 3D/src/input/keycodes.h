#pragma once

#define MAX_BIND_LENGTH 512
struct Key
{
	bool pressed;

	char binding[MAX_BIND_LENGTH];
};

#define MAX_KEYS 256
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

bool IsPrintableASCII(int keycode);

#define MAX_KEYCODE_NAME_LENGTH 32
struct Keyname
{
	const char* name;
	int keycode;
};

extern Keyname keynames[];

struct Button
{
	int down[2];
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