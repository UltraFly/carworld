
/*#ifndef JOY_FFEED_H
#define JOY_FFEED_H

#define DIRECTINPUT_VERSION 0x0500
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <dinput.h>

#include "../H_Main.h"

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
	virtual bool SetAcquire(bool Active);
	virtual bool SetForceFeedback(REAL x, REAL y);
private:
	static BOOL CALLBACK EnumFFJoysticksCallback(const DIDEVICEINSTANCE* pInst, VOID* lpvContext);
private:
	bool valid;
	string JoyName;

//range fr the axes
	LONG lMin,lMax;

//current joystick state
    DIJOYSTATE dijs;
	IDirectInput*           g_pDI;         
	IDirectInputDevice2*    g_pJoystick;
	IDirectInputEffect*     g_pEffect;
	HINSTANCE               g_hInst;
    DIDEVCAPS				didc;

};

#endif // !defined(JOY_FFEED_H)*/
