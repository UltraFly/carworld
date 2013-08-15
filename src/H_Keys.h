
#ifndef _H_KEYS_H_
#define _H_KEYS_H_

#include <iostream>
using namespace std;

#include <SDL_scancode.h>
#include <SDL_keycode.h>

class HKey
{
public:
	HKey(SDL_Scancode value = SDL_SCANCODE_UNKNOWN) : value(value) {}
	SDL_Scancode value;
};

//const char* ToStr(SDL_Scancode key);
//SDL_Scancode ToSDL_Scancode(const char* str);

//BUG this function sould be used to serialise keys...

ostream& operator<<(ostream& out, const HKey key);
istream& operator>>(istream& in, HKey& key);

#endif //_H_KEYS_H_
