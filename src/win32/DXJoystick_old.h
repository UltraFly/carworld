
#ifndef _DX_JOYSTICK_H_
#define _DX_JOYSTICK_H_

//using directX (DirectInput)
#include <dinput.h>

#include "../H_Main.h"
#include "Win32Internals.h"

class DXJoystick : public HJoystick
{
public:
	DXJoystick(HINSTANCE hInst, HWND hWnd);
	virtual ~DXJoystick();
	virtual bool IsValid();
	virtual const char *GetDescription();
	virtual int GetNumAxis();
	virtual int GetNumButtons();
	virtual void UpdateState();
	virtual REAL GetAxisPos(int i);
	virtual bool GetButtonPos(int i);
	virtual void SetForceFeedback(int i, REAL amplitude);

	bool valid;
	bool ForceFeedback;
	REAL axis[8];
	bool button[32];
//Windows specific DirectX members and methods
private:
	bool inputAcquireDevices();
	bool inputPrepareDevice();
	bool inputInitForceFeedback();
private:
	IDirectInput* gpdi;    // base DirectInput object
	IDirectInputDevice2* gpdiJoystick;    // DirectInputDevice2 objects
	IDirectInputEffect* gpdiEffect;
	string JoysticProductName;
};

#endif //_DX_JOYSTICK_H_
