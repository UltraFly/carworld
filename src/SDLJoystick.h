
#ifndef _SDL_JOYSTICK_H_
#define _SDL_JOYSTICK_H_

#include "H_Main.h"

#include <SDL_joystick.h>

class SDLJoystick : public HJoystick
{
public:
	SDLJoystick(int index);
	virtual ~SDLJoystick();
	virtual bool IsValid();
	virtual const char *GetDescription();
	virtual int GetNumAxis();
	virtual int GetNumButtons();
	virtual void UpdateState();
	virtual REAL GetAxisPos(int i);
	virtual bool GetButtonPos(int i);
	virtual bool SetAcquire(bool Active);
	virtual bool SetForceFeedback(REAL x, REAL y);
private:
	int device_index;
	string name;
	SDL_Joystick* joystick;
};

#endif //_SDL_JOYSTICK_H_
