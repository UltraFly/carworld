
#ifndef _LIN_JOYSTICK_H_
#define _LIN_JOYSTICK_H_

#include "../H_Main.h"

#include <linux/joystick.h>

///joistick implementation specific to the linux driver
class LinJoystick : public HJoystick
{
public:
	LinJoystick(const char *DeviceName = "/dev/js0");
	virtual ~LinJoystick();
	virtual bool IsValid();
	virtual const char *GetDescription();
	virtual int GetNumAxis();
	virtual int GetNumButtons();
	virtual void UpdateState();
	virtual REAL GetAxisPos(int i);
	virtual bool GetButtonPos(int i);
	virtual bool SetForceFeedback(REAL x, REAL y);
private:
	int fd;
	char name[128];
	JS_DATA_TYPE state;
};

#endif //_LIN_JOYSTICK_H_
