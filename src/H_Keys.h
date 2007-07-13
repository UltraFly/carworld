
#ifndef _H_KEYS_H_
#define _H_KEYS_H_

#include <iostream>
using namespace std;

#include <SDL_keysym.h>

const char* ToStr(SDLKey key);
SDLKey ToSDLKey(const char* str);

ostream& operator<<(ostream &out, const SDLKey key);
istream& operator>>(istream &in, SDLKey &key);

#endif //_H_KEYS_H_
