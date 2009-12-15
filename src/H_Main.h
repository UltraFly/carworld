
#ifndef _H_MAIN_H_
#define _H_MAIN_H_

namespace std {}
using namespace std;

#include <vector>
#include <string>
#include <iostream>
#include "H_Keys.h"
#include "H_Standard.h"

#define BH_DEFAULT_X 100
#define BH_DEFAULT_Y 100

//interface to joystick
class HJoystick
{
public:
	virtual ~HJoystick();
	virtual bool IsValid() = 0;
	virtual const char *GetDescription() = 0;
	virtual int GetNumAxis() = 0;
	virtual int GetNumButtons() = 0;
	virtual void UpdateState() = 0;
	virtual REAL GetAxisPos(int i) = 0;
	virtual bool GetButtonPos(int i) = 0;
	virtual bool SetForceFeedback(REAL x,REAL y) =0;
};

//this class is used internally to keep data about the (OS specific) window
//and display, meaning that 2 HWindows of the same app might use a different keyboard etc...
//it has to be virtual because we can't put the (OS specific) members of
//this object in this header
class HWindow
{
public:
	virtual ~HWindow();
	//to do with keyboard state: (asynchron)
	virtual const char* GetKeyboardDescription() = 0;
	virtual bool IsPressed(SDLKey k) = 0;
	virtual HJoystick* GetJoystick() = 0;
	//make this Window the current opengl rendering window
	virtual void MakeCurrent() = 0;
	virtual void SwapBuffers() = 0;
	//depending on implementation, SetAttrib might only work if called
	//before the window is opened
	virtual void SetAttrib(int width, int height, bool fullscreen) = 0;
public:
	int m_width, m_height;
	bool m_fullscreen;
};

class HAppData;

//derive from this class to create a new (non graphic) application
class HApplication
{
public:
	HApplication();
	virtual ~HApplication();

	static int TimeRefreshRate();
//for window title etc...
	virtual const char *name() = 0;
//elapsed time since last called
	virtual void on_idle(unsigned int elapsed_time) = 0;
public:
//OS specific application data, do not access
	HAppData *m_data;
};

//derive from this class to create a new graphic application
//it will render to a (single) opengl window (for now...)
class HglApplication : public HApplication
{
public:
	HglApplication();
	virtual ~HglApplication();

//screen mode manipulation:
	unsigned int GetScreenMode();
	bool SetScreenMode(unsigned int ScreenMode, ostream &out); //SetScreenMode returns true if successful
	void PrintScreenMode(unsigned int ScreenMode, ostream &out); //write screen mode information
	void PrintAllScreenModes(ostream &out);
//events
	virtual void draw_init() = 0; //init is called after the window is opened
	virtual void draw() = 0;
	virtual void key_down(SDLKey AHKey, char c) = 0;
	virtual void resize(unsigned int width, unsigned int height) = 0;
public:
//OS specific window
	HWindow *m_window;
};

const char *ConfigurationFileName();
void HErrorExit(const char *E);

void Add(HglApplication *app);

#endif //_H_MAIN_H_
