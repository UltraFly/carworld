
#ifndef _X11_INTERNALS_H_
#define _X11_INTERNALS_H_

#include <X11/Xlib.h>
#include <GL/glx.h>


class HX11Window : public HWindow
{
//overloades
public:
	virtual bool IsPressed(H_KEY k);
	virtual const char *GetKeyboardDescription();
	virtual HJoystick *GetJoystick();
	virtual void MakeCurrent();
	virtual void SwapBuffers();
	virtual void SetAttrib(int width, int height, bool fullscreen);
public:
	HX11Window();
	virtual ~HX11Window();
	bool CheckEvent(XEvent *event);
	void open(const char *name);
public:
	Display *dpy;
	Window w;
	GLXContext cx;
};

#endif //_X11_INTERNALS_H_
