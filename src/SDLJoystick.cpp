
#include "SDLJoystick.h"
#include <SDL3/SDL_joystick.h>

SDLJoystick::SDLJoystick(int index) :
	name("ERROR"),
	joystick(NULL)
{
	int NumJoy = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&NumJoy);
	if (index<0 || index>=NumJoy)
	{
		SDL_free(joysticks);
		return;
	}
	joystick = SDL_OpenJoystick(joysticks[index]);
	SDL_free(joysticks);
	if (joystick!=NULL)
	{
		const char* joystick_name = SDL_GetJoystickName(joystick);
		name = joystick_name!=NULL ? joystick_name : "SDL joystick";
	}
}


//Initialize the DirectInput variables.
SDLJoystick::~SDLJoystick()
{
	if (joystick!=NULL)
		SDL_CloseJoystick(joystick);
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
	return SDL_GetNumJoystickAxes(joystick);
}

int SDLJoystick::GetNumButtons()
{
	if (!IsValid())
		return 0;
	return SDL_GetNumJoystickButtons(joystick);
}

void SDLJoystick::UpdateState()
{
	if (!IsValid())
		return;
	SDL_UpdateJoysticks();
}

REAL SDLJoystick::GetAxisPos(int i)
{
	if (!IsValid())
		return 0;
	UpdateState();
	Sint16 SIntPos = SDL_GetJoystickAxis(joystick, i);
	return -REAL(SIntPos)/32768.0f;
}

bool SDLJoystick::GetButtonPos(int i)
{
	if (!IsValid())
		return 0;
	return SDL_GetJoystickButton(joystick, i);
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
