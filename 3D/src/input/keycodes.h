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
	KEY_None = 0,

	KEY_UpArrow = 1,
	KEY_DownArrow = 2,
	KEY_LeftArrow = 3,
	KEY_RightArrow = 4,

	// 48 - 57 : Numbers

	// 65 - 90 : Letters
};

int KeycodeToLowercase(const int keycode);
bool IsPrintableASCII(const int keycode);

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
	IN_MoveForward,
	IN_MoveBack,
	IN_MoveLeft,
	IN_MoveRight,

	IN_LookUp,
	IN_LookDown,
	IN_LookLeft,
	IN_LookRight,
};