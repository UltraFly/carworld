
#include "SDLJoystick.h"

#include <algorithm>
#include <cmath>

#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_joystick.h>

SDLJoystick::SDLJoystick(int index) :
	name("ERROR"),
	joystick(NULL),
	haptic(NULL),
	haptic_effect(-1),
	haptic_steering_axis(true),
	haptic_level(0),
	haptic_direction_x(0),
	haptic_direction_y(0),
	haptic_error_reported(false)
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
		InitializeForceFeedback();
	}
}

SDLJoystick::~SDLJoystick()
{
	ShutdownForceFeedback();
	if (joystick!=NULL)
		SDL_CloseJoystick(joystick);
}

void SDLJoystick::InitializeForceFeedback()
{
	if (!SDL_IsJoystickHaptic(joystick))
	{
		cout << "force feedback unavailable on this joystick.\n";
		return;
	}

	haptic = SDL_OpenHapticFromJoystick(joystick);
	if (haptic==NULL)
	{
		cout << "could not open force feedback: " << SDL_GetError() << "\n";
		return;
	}

	Uint32 features = SDL_GetHapticFeatures(haptic);
	if ((features & SDL_HAPTIC_CONSTANT)==0)
	{
		cout << "force feedback device does not support constant force.\n";
		SDL_CloseHaptic(haptic);
		haptic = NULL;
		return;
	}

	if ((features & SDL_HAPTIC_AUTOCENTER)!=0 && !SDL_SetHapticAutocenter(haptic, 0))
		cout << "could not disable force feedback autocenter: " << SDL_GetError() << "\n";

	int axes = SDL_GetNumHapticAxes(haptic);
	haptic_steering_axis =
		SDL_GetJoystickType(joystick)==SDL_JOYSTICK_TYPE_WHEEL || axes<2;

	SDL_HapticEffect effect = {};
	effect.type = SDL_HAPTIC_CONSTANT;
	effect.constant.direction.type =
		haptic_steering_axis ? SDL_HAPTIC_STEERING_AXIS : SDL_HAPTIC_CARTESIAN;
	effect.constant.direction.dir[0] = haptic_steering_axis ? 0 : 1;
	effect.constant.length = SDL_HAPTIC_INFINITY;
	effect.constant.level = 0;

	haptic_effect = SDL_CreateHapticEffect(haptic, &effect);
	if (haptic_effect<0)
	{
		cout << "could not create constant force effect: " << SDL_GetError() << "\n";
		SDL_CloseHaptic(haptic);
		haptic = NULL;
		return;
	}

	if (!SDL_RunHapticEffect(haptic, haptic_effect, 1))
	{
		cout << "could not start constant force effect: " << SDL_GetError() << "\n";
		SDL_DestroyHapticEffect(haptic, haptic_effect);
		haptic_effect = -1;
		SDL_CloseHaptic(haptic);
		haptic = NULL;
		return;
	}

	haptic_direction_x = effect.constant.direction.dir[0];
	haptic_direction_y = effect.constant.direction.dir[1];
	cout << "force feedback enabled using "
		<< (haptic_steering_axis ? "the steering axis" : "two axes")
		<< ".\n";
}

void SDLJoystick::ShutdownForceFeedback()
{
	if (haptic==NULL)
		return;

	if (haptic_effect>=0)
	{
		SDL_StopHapticEffect(haptic, haptic_effect);
		SDL_DestroyHapticEffect(haptic, haptic_effect);
		haptic_effect = -1;
	}
	SDL_CloseHaptic(haptic);
	haptic = NULL;
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

//Stop force output while the application is inactive and resume it on return.
bool SDLJoystick::SetAcquire(bool Active)
{
	if (haptic==NULL || haptic_effect<0)
		return false;
	return Active
		? SDL_RunHapticEffect(haptic, haptic_effect, 1)
		: SDL_StopHapticEffect(haptic, haptic_effect);
}


//Apply the X and Y forces to the effect we prepared.
bool SDLJoystick::SetForceFeedback(REAL x, REAL y)
{
	if (haptic==NULL || haptic_effect<0)
		return false;

	double force_x = std::max(-1.0, std::min(1.0, static_cast<double>(x)));
	double force_y = std::max(-1.0, std::min(1.0, static_cast<double>(y)));

	SDL_HapticEffect effect = {};
	effect.type = SDL_HAPTIC_CONSTANT;
	effect.constant.direction.type =
		haptic_steering_axis ? SDL_HAPTIC_STEERING_AXIS : SDL_HAPTIC_CARTESIAN;
	effect.constant.length = SDL_HAPTIC_INFINITY;

	if (haptic_steering_axis)
	{
		effect.constant.level =
			static_cast<Sint16>(std::lround(force_x * 32767.0));
	}
	else
	{
		double magnitude = std::sqrt(force_x*force_x + force_y*force_y);
		if (magnitude>0.0)
		{
			effect.constant.direction.dir[0] =
				static_cast<Sint32>(std::lround(force_x / magnitude * 32767.0));
			effect.constant.direction.dir[1] =
				static_cast<Sint32>(std::lround(force_y / magnitude * 32767.0));
		}
		else
		{
			effect.constant.direction.dir[0] = haptic_direction_x;
			effect.constant.direction.dir[1] = haptic_direction_y;
		}
		effect.constant.level = static_cast<Sint16>(
			std::lround(std::min(1.0, magnitude) * 32767.0)
		);
	}

	if (effect.constant.level==haptic_level &&
		effect.constant.direction.dir[0]==haptic_direction_x &&
		effect.constant.direction.dir[1]==haptic_direction_y)
		return true;

	if (!SDL_UpdateHapticEffect(haptic, haptic_effect, &effect) ||
		!SDL_RunHapticEffect(haptic, haptic_effect, 1))
	{
		if (!haptic_error_reported)
		{
			cout << "could not update force feedback: " << SDL_GetError() << "\n";
			haptic_error_reported = true;
		}
		return false;
	}

	haptic_level = effect.constant.level;
	haptic_direction_x = effect.constant.direction.dir[0];
	haptic_direction_y = effect.constant.direction.dir[1];
	return true;
}
