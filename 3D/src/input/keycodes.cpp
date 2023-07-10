#include "pch.h"
#include "keycodes.h"

Keyname keynames[] =
{
	{"up", KEY_UpArrow},
	{"down", KEY_DownArrow},
	{"left", KEY_LeftArrow},
	{"right", KEY_RightArrow},

	{"scrollup", KEY_ScrollUp},
	{"scrolldown", KEY_ScrollDown},
	{"mouseleft", KEY_MouseLeft},
	{"mousemiddle", KEY_MouseMiddle},
	{"mouseright", KEY_MouseRight},
	{"mouse4", KEY_Mouse4},
	{"mouse5", KEY_Mouse5},
	{"mouse6", KEY_Mouse6},
	{"mouse7", KEY_Mouse7},
	{"mouse8", KEY_Mouse8},

	{NULL, 0} // Terminate when looping
};

bool IsNumber(int keycode)
{
	// 48 - 57 : Numbers
	return keycode >= 48 && keycode <= 57;
}

bool IsLowercase(int keycode)
{
	// 97 - 122 : Lowercase Letters
	return keycode >= 97 && keycode <= 122;
}

bool IsUppercase(int keycode)
{
	// 65 - 90 : Uppercase Letters
	return keycode >= 65 && keycode <= 90;
}

int KeycodeToLowercase(int keycode)
{
	if (IsUppercase(keycode))
	{
		return keycode + 32;
	}

	return keycode;
}

bool IsPrintableKeycode(int keycode)
{
	if (IsNumber(keycode))
		return true;

	if (IsLowercase(keycode))
		return true;

	// Uppercase is not used for keycodes
	//if (IsUppercase(keycode))
	//	return true;

	return false;
}
