
#include "H_Geometry.h"
#include "H_Input.h"

//CLASS KeyJoystick
KeyJoystick::KeyJoystick(HWindow *w) :
	up_key(SDL_SCANCODE_UP),
	down_key(SDL_SCANCODE_DOWN),
	left_key(SDL_SCANCODE_LEFT),
	right_key(SDL_SCANCODE_RIGHT),
	button_key(SDL_SCANCODE_SPACE),
	speed(.1f),
	m_HWindow(w),
	Yaxis(0),
	Xaxis(0),
	button(false)
{}

KeyJoystick::~KeyJoystick()
{}

bool KeyJoystick::IsValid()
{
	return true;
}

const char *KeyJoystick::GetDescription()
{
	return m_HWindow->GetKeyboardDescription();
}

int KeyJoystick::GetNumAxis()
{
	return 2;
}

int KeyJoystick::GetNumButtons()
{
	return 1;
}

void KeyJoystick::UpdateState()
{
    button = m_HWindow->IsPressed(button_key.value);

    bool UpState = m_HWindow->IsPressed(up_key.value);
    bool DownState = m_HWindow->IsPressed(down_key.value);
    bool LeftState = m_HWindow->IsPressed(left_key.value);
    bool RightState = m_HWindow->IsPressed(right_key.value);
	//assert(!(UpState && DownState && LeftState && RightState));

    if (LeftState) Xaxis = H_MIN(Xaxis+2*speed,1);
	else if (RightState)
		Xaxis = H_MAX(Xaxis-2*speed,-1);
	else if (Xaxis>0)
		Xaxis = H_MAX(0,Xaxis-speed);
	else if (Xaxis<0)
		Xaxis = H_MIN(0,Xaxis+speed);

    if (UpState)
		Yaxis = H_MIN(Yaxis+2*speed,1);
	else if (DownState)
		Yaxis = H_MAX(Yaxis-2*speed,-1);
	else if (Yaxis>0)
		Yaxis = H_MAX(0,Yaxis-speed);
	else if (Yaxis<0)
		Yaxis = H_MIN(0,Yaxis+speed);
}

REAL KeyJoystick::GetAxisPos(int i)
{
	switch (i)
	{
	case 0: return Xaxis;
	case 1: return Yaxis;
	default: return 0;
	}
	return 0;
}

bool KeyJoystick::GetButtonPos(int i)
{
	return (i==0) ? button : 0;
}

bool KeyJoystick::SetForceFeedback(REAL x, REAL y)
{
	return false;
}
