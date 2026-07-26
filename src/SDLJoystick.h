
#ifndef _SDL_JOYSTICK_H_
#define _SDL_JOYSTICK_H_

#include "H_Main.h"

#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_joystick.h>

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
	void InitializeForceFeedback();
	void ShutdownForceFeedback();

	string name;
	SDL_Joystick* joystick;
	SDL_Haptic* haptic;
	SDL_HapticEffectID haptic_effect;
	bool haptic_steering_axis;
	Sint16 haptic_level;
	Sint32 haptic_direction_x;
	Sint32 haptic_direction_y;
	bool haptic_error_reported;
};

#endif //_SDL_JOYSTICK_H_
