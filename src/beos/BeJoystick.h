
#ifndef _BE_JOYSTICK_H_
#define _BE_JOYSTICK_H_

#include "../H_Main.h"

#include <Joystick.h>

///joistick implementation specific to the BeOS driver
class BeJoystick : public HJoystick
{
public:
	BeJoystick();
	virtual ~BeJoystick();
	virtual bool IsValid();
	virtual const char *GetDescription();
	virtual int GetNumAxis();
	virtual int GetNumButtons();
	virtual void UpdateState();
	virtual REAL GetAxisPos(int i);
	virtual bool GetButtonPos(int i);
private:
	BJoystick m_joy;
	string m_description;
	bool m_valid;
};

#endif //_BE_JOYSTICK_H_
