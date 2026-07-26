
#include "H_Keys.h"
#include "H_Standard.h"
#include <string.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_keyboard.h>

ostream &operator<<(ostream &out, const HKey key)
{
	return out << SDL_GetScancodeName(key.value);
}

istream &operator>>(istream &in, HKey &key)
{
	string tmp;
	getline(in,tmp,'\0');
	key.value = SDL_GetScancodeFromName(tmp.c_str());
	return in;
}


