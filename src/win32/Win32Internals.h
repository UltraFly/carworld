
#ifndef _H_INTERNALS_H_
#define _H_INTERNALS_H_

#include <windows.h>
#include <windef.h>

class HJoystick;
class DXJoystick;

class HWinWindow : public HWindow
{
public:
	HWinWindow();
	virtual ~HWinWindow();
	virtual void SetAttrib(int width, int height, bool fullscreen);
	bool IsPressed(H_KEY k);
	const char *GetKeyboardDescription();
	HJoystick *GetJoystick();
	virtual void MakeCurrent();
	virtual void SwapBuffers();
public:
	DXJoystick *m_DXJoystick;

	HWND hWnd;				//the window
	HINSTANCE hInst;		//handle to the instance
	HGLRC hRC;				//opengl context
	HDC hDC;				//device context
	HPALETTE hPalette;		//handle to the palette
};

#endif //_H_INTERNALS_H_
