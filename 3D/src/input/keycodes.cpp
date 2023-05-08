#include <pch.h>
#include <input/keycodes.h>

Keyname keynames[] =
{
	{"up", KEY_UpArrow},
	{"down", KEY_DownArrow},
	{"left", KEY_LeftArrow},
	{"right", KEY_RightArrow},

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
	// 65 - 90 : Uppercase Letters
	if (IsUppercase(keycode))
	{
		return keycode + 32;
	}

	return keycode;
}

bool IsPrintableASCII(int keycode)
{
	if (IsNumber(keycode))
		return true;

	if (IsLowercase(keycode))
		return true;

	if (IsUppercase(keycode))
		return true;

	return false;
}
