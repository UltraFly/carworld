
#ifdef USE_LIN_JOY

#include "LinJoystick.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

LinJoystick::LinJoystick(const char *DeviceName) :
	fd(open(DeviceName, O_RDONLY))
{}

LinJoystick::~LinJoystick()
{}

bool LinJoystick::IsValid()
{
	return fd >= 0;
}

const char *LinJoystick::GetDescription()
{
	if (IsValid())
	{
		ioctl(fd, JSIOCGNAME(sizeof(name)), name);
		return name;
	}
	else
		return "no joystick available";
}

int LinJoystick::GetNumAxis()
{
	if (!IsValid())
		return 0;

	int NumAxis;
	ioctl(fd, JSIOCGAXES, &NumAxis);
	return NumAxis;
}

int LinJoystick::GetNumButtons()
{
	if (!IsValid())
		return 0;

	int NumButtons;
	ioctl(fd, JSIOCGBUTTONS, &NumButtons);
	return NumButtons;
}

void LinJoystick::UpdateState()
{
	if (!IsValid())
		return;

	if (read(fd, &state, JS_RETURN) != JS_RETURN)
		cerr << "could not read form joystick!\n";
}

REAL LinJoystick::GetAxisPos(int i)
{
	if (!IsValid())
		return 0;

	if (i==0)
		return (state.x-128)/-128.f;
	else if (i==1)
		return (state.y-128)/-128.f;
	return 0.f;
}

bool LinJoystick::GetButtonPos(int i)
{
	if (!IsValid())
		return false;

	return (state.buttons & (1<<i))!=0;
		
}

bool LinJoystick::SetForceFeedback(REAL x, REAL y)
{
	return false;
}

#endif //USE_LIN_JOY
