
#include "BeJoystick.h"
#include "String.h"

BeJoystick::BeJoystick() : m_valid(false)
{
	if (m_joy.CountDevices()<=0)
	{
		m_description = "*** No game ports detected.";
		return;
	}

	char devName[B_OS_NAME_LENGTH];
	if (m_joy.GetDeviceName(0, devName)!=B_OK)
	{
		m_description = "*** Error while reading controller name.";
		return;
	}
	
	if (m_joy.Open(devName, true)<B_OK)
	{
		m_description = string("1. No controller on ") + devName;
		return;
	} 

	BString name;
	if (m_joy.GetControllerName(&name) != B_OK)
	{
		m_description = string("*** Can't get name of controller ") + devName;
		return;
	}

	m_valid = true;
	m_description = name.String();
}

BeJoystick::~BeJoystick()
{}

bool BeJoystick::IsValid()
{
	return m_valid;
}

const char *BeJoystick::GetDescription()
{
	return m_description.c_str();
}

int BeJoystick::GetNumAxis()
{
	if (!IsValid())
		return 0;

	return m_joy.CountAxes();
}

int BeJoystick::GetNumButtons()
{
	if (!IsValid())
		return 0;

	return m_joy.CountButtons();
}

void BeJoystick::UpdateState()
{}

REAL BeJoystick::GetAxisPos(int i)
{
	if (!IsValid())
		return 0;

	int16 *axes = 0;
	m_joy.GetAxisValues(axes);
	//cout << axes[i] << endl;
	return axes[i]/32768.f;
}

bool BeJoystick::GetButtonPos(int i)
{
	if (!IsValid())
		return false;

	return (m_joy.ButtonValues() & (1<<i))!=0;
		
}
