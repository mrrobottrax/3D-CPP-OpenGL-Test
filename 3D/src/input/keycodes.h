#pragma once

constexpr auto maxBindLength = 512;
struct Key
{
	bool pressed;

	char binding[maxBindLength];
};

constexpr auto maxKeys = 256;
enum KeyCode
{
	KEY_None = 0,

	KEY_UpArrow,
	KEY_DownArrow,
	KEY_LeftArrow,
	KEY_RightArrow,

	KEY_ScrollUp,
	KEY_ScrollDown,

	KEY_MouseLeft,
	KEY_MouseMiddle,
	KEY_MouseRight,
	KEY_Mouse4,
	KEY_Mouse5,
	KEY_Mouse6,
	KEY_Mouse7,
	KEY_Mouse8,

	// 48 - 57 : Numbers
	KEY_NumbersStart = 48,
	KEY_NumbersEnd = 57,

	// 97 - 122 : Letters
	KEY_LettersStart = 97,
	KEY_LettersEnd = 122,
};

int KeycodeToLowercase(const int keycode);
bool IsLowercase(const int keycode);
bool IsUppercase(const int keycode);
bool IsPrintableKeycode(const int keycode);

constexpr auto maxKeycodeNameLength = 32;
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