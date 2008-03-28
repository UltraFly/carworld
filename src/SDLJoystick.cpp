
#include "SDLJoystick.h"
#include <SDL_joystick.h>

SDLJoystick::SDLJoystick(int index) :
	device_index(index),
	name("ERROR"),
	joystick(NULL)
{
	int NumJoy = SDL_NumJoysticks();
	if (index<0 || index>=NumJoy)
		return;
	joystick = SDL_JoystickOpen(device_index);
	name = SDL_JoystickName(device_index);
	//SDL_JoystickEventState(SDL_QUERY, SDL_ENABLE or SDL_IGNORE);
}


//Initialize the DirectInput variables.
SDLJoystick::~SDLJoystick()
{
}

bool SDLJoystick::IsValid()
{
	return joystick!=NULL;
}

const char* SDLJoystick::GetDescription()
{
	return name.c_str();
}

int SDLJoystick::GetNumAxis()
{
	if (!IsValid())
		return 0;
	return SDL_JoystickNumAxes(joystick);
}

int SDLJoystick::GetNumButtons()
{
	if (!IsValid())
		return 0;
	return SDL_JoystickNumButtons(joystick);
}

void SDLJoystick::UpdateState()
{
	if (!IsValid())
		return;
	//SDL_JoystickUpdate();
}

REAL SDLJoystick::GetAxisPos(int i)
{
	if (!IsValid())
		return 0;
	UpdateState();
	Sint16 SIntPos = SDL_JoystickGetAxis(joystick, i);
	return -REAL(SIntPos)/32768.0f;
}

bool SDLJoystick::GetButtonPos(int i)
{
	if (!IsValid())
		return 0;
	return SDL_JoystickGetButton(joystick, i)!=0;
}

//Acquire or unacquire the mouse, depending on if the app is active
//Input device must be acquired before the GetDeviceState is called
bool SDLJoystick::SetAcquire(bool Active)
{
    return true;
}


//Apply the X and Y forces to the effect we prepared.
bool SDLJoystick::SetForceFeedback(REAL x, REAL y)
{
	return true;
}
