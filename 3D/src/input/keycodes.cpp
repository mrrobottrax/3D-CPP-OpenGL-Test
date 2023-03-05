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

bool IsPrintableASCII(int keycode)
{
	// 48 - 57 : Numbers
	if (keycode >= 48 && keycode <= 57)
	{
		return true;
	}

	// 65 - 90 : Letters
	if (keycode >= 65 && keycode <= 90)
	{
		return true;
	}

	return false;
}
