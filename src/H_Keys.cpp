
#include "H_Keys.h"
#include "H_Standard.h"
#include <string.h>

struct KeyDesc
{
	SDLKey key;
	const char* name;
};

static KeyDesc KeyMapValues[] =
{
	{SDLK_UNKNOWN, "UNKNOWN"},
	{SDLK_BACKSPACE, "BACKSPACE"},
	{SDLK_TAB, "TAB"},
	{SDLK_CLEAR, "CLEAR"},
	{SDLK_RETURN, "RETURN"},
	{SDLK_PAUSE, "PAUSE"},
	{SDLK_ESCAPE, "ESCAPE"},
	{SDLK_SPACE, "SPACE"},
	{SDLK_EXCLAIM, "EXCLAIM"},
	{SDLK_QUOTEDBL, "QUOTEDBL"},
	{SDLK_HASH, "HASH"},
	{SDLK_DOLLAR, "DOLLAR"},
	{SDLK_AMPERSAND, "AMPERSAND"},
	{SDLK_QUOTE, "QUOTE"},
	{SDLK_LEFTPAREN, "LEFTPAREN"},
	{SDLK_RIGHTPAREN, "RIGHTPAREN"},
	{SDLK_ASTERISK, "ASTERISK"},
	{SDLK_PLUS, "PLUS"},
	{SDLK_COMMA, "COMMA"},
	{SDLK_MINUS, "MINUS"},
	{SDLK_PERIOD, "PERIOD"},
	{SDLK_SLASH, "SLASH"},
	{SDLK_0, "0"},
	{SDLK_1, "1"},
	{SDLK_2, "2"},
	{SDLK_3, "3"},
	{SDLK_4, "4"},
	{SDLK_5, "5"},
	{SDLK_6, "6"},
	{SDLK_7, "7"},
	{SDLK_8, "8"},
	{SDLK_9, "9"},
	{SDLK_COLON, "COLON"},
	{SDLK_SEMICOLON, "SEMICOLON"},
	{SDLK_LESS, "LESS"},
	{SDLK_EQUALS, "EQUALS"},
	{SDLK_GREATER, "GREATER"},
	{SDLK_QUESTION, "QUESTION"},
	{SDLK_AT, "AT"},
	{SDLK_LEFTBRACKET, "LEFTBRACKET"},
	{SDLK_BACKSLASH, "BACKSLASH"},
	{SDLK_RIGHTBRACKET, "RIGHTBRACKET"},
	{SDLK_CARET, "CARET"},
	{SDLK_UNDERSCORE, "UNDERSCORE"},
	{SDLK_BACKQUOTE, "BACKQUOTE"},
	{SDLK_a, "a"},
	{SDLK_b, "b"},
	{SDLK_c, "c"},
	{SDLK_d, "d"},
	{SDLK_e, "e"},
	{SDLK_f, "f"},
	{SDLK_g, "g"},
	{SDLK_h, "h"},
	{SDLK_i, "i"},
	{SDLK_j, "j"},
	{SDLK_k, "k"},
	{SDLK_l, "l"},
	{SDLK_m, "m"},
	{SDLK_n, "n"},
	{SDLK_o, "o"},
	{SDLK_p, "p"},
	{SDLK_q, "q"},
	{SDLK_r, "r"},
	{SDLK_s, "s"},
	{SDLK_t, "t"},
	{SDLK_u, "u"},
	{SDLK_v, "v"},
	{SDLK_w, "w"},
	{SDLK_x, "x"},
	{SDLK_y, "y"},
	{SDLK_z, "z"},
	{SDLK_DELETE, "DELETE"},
	{SDLK_KP0, "KP0"},
	{SDLK_KP1, "KP1"},
	{SDLK_KP2, "KP2"},
	{SDLK_KP3, "KP3"},
	{SDLK_KP4, "KP4"},
	{SDLK_KP5, "KP5"},
	{SDLK_KP6, "KP6"},
	{SDLK_KP7, "KP7"},
	{SDLK_KP8, "KP8"},
	{SDLK_KP9, "KP9"},
	{SDLK_KP_PERIOD, "KP_PERIOD"},
	{SDLK_KP_DIVIDE, "KP_DIVIDE"},
	{SDLK_KP_MULTIPLY, "KP_MULTIPLY"},
	{SDLK_KP_MINUS, "KP_MINUS"},
	{SDLK_KP_PLUS, "KP_PLUS"},
	{SDLK_KP_ENTER, "KP_ENTER"},
	{SDLK_KP_EQUALS, "KP_EQUALS"},
	{SDLK_UP, "UP"},
	{SDLK_DOWN, "DOWN"},
	{SDLK_RIGHT, "RIGHT"},
	{SDLK_LEFT, "LEFT"},
	{SDLK_INSERT, "INSERT"},
	{SDLK_HOME, "HOME"},
	{SDLK_END, "END"},
	{SDLK_PAGEUP, "PAGEUP"},
	{SDLK_PAGEDOWN, "PAGEDOWN"},
	{SDLK_F1, "F1"},
	{SDLK_F2, "F2"},
	{SDLK_F3, "F3"},
	{SDLK_F4, "F4"},
	{SDLK_F5, "F5"},
	{SDLK_F6, "F6"},
	{SDLK_F7, "F7"},
	{SDLK_F8, "F8"},
	{SDLK_F9, "F9"},
	{SDLK_F10, "F10"},
	{SDLK_F11, "F11"},
	{SDLK_F12, "F12"},
	{SDLK_F13, "F13"},
	{SDLK_F14, "F14"},
	{SDLK_F15, "F15"},
	{SDLK_NUMLOCK, "NUMLOCK"},
	{SDLK_CAPSLOCK, "CAPSLOCK"},
	{SDLK_SCROLLOCK, "SCROLLOCK"},
	{SDLK_RSHIFT, "RSHIFT"},
	{SDLK_LSHIFT, "LSHIFT"},
	{SDLK_RCTRL, "RCTRL"},
	{SDLK_LCTRL, "LCTRL"},
	{SDLK_RALT, "RALT"},
	{SDLK_LALT, "LALT"},
	{SDLK_RMETA, "RMETA"},
	{SDLK_LMETA, "LMETA"},
	{SDLK_LSUPER, "LSUPER"},
	{SDLK_RSUPER, "RSUPER"},
	{SDLK_MODE, "MODE"},
	{SDLK_COMPOSE, "COMPOSE"},
	{SDLK_HELP, "HELP"},
	{SDLK_PRINT, "PRINT"},
	{SDLK_SYSREQ, "SYSREQ"},
	{SDLK_BREAK, "BREAK"},
	{SDLK_MENU, "MENU"},
	{SDLK_POWER, "POWER"},
	{SDLK_EURO, "EURO"},
	{SDLK_UNDO, "UNDO"}
};

const char* ToStr(SDLKey key)
{
	for (unsigned int i=0 ; i<ARRAY_SIZE(KeyMapValues) ; i++)
	{
		if (key==KeyMapValues[i].key)
			return KeyMapValues[i].name;
	}
	return "UNKNOWN";
}

SDLKey ToSDLKey(const char* str)
{
	for (unsigned int i=0 ; i<ARRAY_SIZE(KeyMapValues) ; i++)
	{
		if (strcmp(str, KeyMapValues[i].name)==0)
			return KeyMapValues[i].key;
	}
	return SDLK_UNKNOWN;
}

ostream &operator<<(ostream &out, const SDLKey key)
{
	return out << ToStr(key);
}

istream &operator>>(istream &in, SDLKey &key)
{
	string tmp;
	getline(in,tmp,'\0');
	key = ToSDLKey(tmp.c_str());
	return in;
}


