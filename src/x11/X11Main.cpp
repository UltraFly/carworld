
#include <ctype.h>
#include "../H_Prompt.h"
#include "../H_Main.h"

#ifdef USE_LIN_JOY
#include "LinJoystick.h"
#endif //USE_LIN_JOY

#include <GL/glx.h>
#include <GL/gl.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/time.h>

#include <unistd.h> //for sleep()
#include <X11/keysym.h>

#include "X11Internals.h"
#include "../H_Keys.h"

void HErrorExit(const char *E)
{
	exit(1);
}

static list<HApplication*> AppList;

static pair<int,H_KEY> XKeyMapValues[] =
{
	pair<int,H_KEY>(-1,HK_INVALID),
	pair<int,H_KEY>(XK_Cancel,HK_CANCEL),
	pair<int,H_KEY>(XK_Clear,HK_CLEAR),
	pair<int,H_KEY>(XK_Shift_L,HK_SHIFT),
	pair<int,H_KEY>(XK_Shift_R,HK_SHIFT),
	pair<int,H_KEY>(XK_Control_L,HK_CONTROL),
	pair<int,H_KEY>(XK_Control_R,HK_CONTROL),
	pair<int,H_KEY>(XK_Menu,HK_MENU),
	pair<int,H_KEY>(XK_Pause,HK_PAUSE),
	pair<int,H_KEY>(XK_Escape,HK_ESCAPE),
	pair<int,H_KEY>(XK_Prior,HK_PRIOR),
	pair<int,H_KEY>(XK_Next,HK_NEXT),
	pair<int,H_KEY>(XK_End,HK_END),
	pair<int,H_KEY>(XK_Home,HK_HOME),
	pair<int,H_KEY>(XK_Select,HK_SELECT),
	pair<int,H_KEY>(XK_Print,HK_PRINT),
	pair<int,H_KEY>(XK_Execute,HK_EXECUTE),
	pair<int,H_KEY>(XK_Insert,HK_INSERT),
	pair<int,H_KEY>(XK_Delete,HK_DELETE),
	pair<int,H_KEY>(XK_Help,HK_HELP),
	pair<int,H_KEY>(XK_KP_0,HK_NUMPAD0),
	pair<int,H_KEY>(XK_KP_1,HK_NUMPAD1),
	pair<int,H_KEY>(XK_KP_2,HK_NUMPAD2),
	pair<int,H_KEY>(XK_KP_3,HK_NUMPAD3),
	pair<int,H_KEY>(XK_KP_4,HK_NUMPAD4),
	pair<int,H_KEY>(XK_KP_5,HK_NUMPAD5),
	pair<int,H_KEY>(XK_KP_6,HK_NUMPAD6),
	pair<int,H_KEY>(XK_KP_7,HK_NUMPAD7),
	pair<int,H_KEY>(XK_KP_8,HK_NUMPAD8),
	pair<int,H_KEY>(XK_KP_9,HK_NUMPAD9),
	pair<int,H_KEY>(XK_multiply,HK_MULTIPLY),
	pair<int,H_KEY>(XK_KP_Multiply,HK_MULTIPLY),
	pair<int,H_KEY>(XK_KP_Add,HK_ADD),
	pair<int,H_KEY>(XK_KP_Separator,HK_SEPARATOR),
	pair<int,H_KEY>(XK_KP_Subtract,HK_SUBTRACT),
	pair<int,H_KEY>(XK_KP_Decimal,HK_DECIMAL),
	pair<int,H_KEY>(XK_KP_Divide,HK_DIVIDE),
	pair<int,H_KEY>(XK_Num_Lock,HK_NUMLOCK),
	pair<int,H_KEY>(XK_Scroll_Lock,HK_SCROLL),
	pair<int,H_KEY>(XK_space,HK_SPACE),
	pair<int,H_KEY>(XK_Tab,HK_TAB),
	pair<int,H_KEY>(XK_Return,HK_RETURN),
	pair<int,H_KEY>(XK_BackSpace,HK_BACK),
	pair<int,H_KEY>(XK_Left,HK_LEFT),
	pair<int,H_KEY>(XK_Up,HK_UP),
	pair<int,H_KEY>(XK_Right,HK_RIGHT),
	pair<int,H_KEY>(XK_Down,HK_DOWN),
	pair<int,H_KEY>(XK_F1,HK_F1),
	pair<int,H_KEY>(XK_F2,HK_F2),
	pair<int,H_KEY>(XK_F3,HK_F3),
	pair<int,H_KEY>(XK_F4,HK_F4),
	pair<int,H_KEY>(XK_F5,HK_F5),
	pair<int,H_KEY>(XK_F6,HK_F6),
	pair<int,H_KEY>(XK_F7,HK_F7),
	pair<int,H_KEY>(XK_F8,HK_F8),
	pair<int,H_KEY>(XK_F9,HK_F9),
	pair<int,H_KEY>(XK_F10,HK_F10),
	pair<int,H_KEY>(XK_F11,HK_F11),
	pair<int,H_KEY>(XK_F12,HK_F12)
};

HMapper<int,H_KEY> XKeyMap(
	XKeyMapValues,
	XKeyMapValues+(sizeof(XKeyMapValues)/sizeof(XKeyMapValues[0]))
);

H_KEY XKtoHK(int XK)
{
	H_KEY HK = XKeyMap.find(XK);
	if (HK!=HK_INVALID)
		return HK;
	else
		return (H_KEY)toupper(XK);
}

int HKtoXK(H_KEY HK)
{
	int XK = XKeyMap.find(HK);
	if (XK!=-1)
		return XK;
	else
		return (int)HK;
}

class HAppData
{
public:
	HAppData();
	int ElapsedTime();
public:
	bool IsInitiated;
	timeval MyPreviousTime;
};

HAppData::HAppData() : IsInitiated(false) {}

static int Diff_timeval(struct timeval &A,struct timeval &B)
{
	int tmp = ((A.tv_sec-B.tv_sec)*1000)+(A.tv_usec-B.tv_usec)/1000;
	//cout << ((A.tv_sec-B.tv_sec)*1000000)+(A.tv_usec-B.tv_usec) << endl;
	return tmp;
}

int HAppData::ElapsedTime()
{
	timeval MyCurrentTime;
	timezone tz;

	if (!IsInitiated)
	{
		gettimeofday(&(MyPreviousTime),&tz);
		IsInitiated = true;
	}
	gettimeofday(&MyCurrentTime,&tz);
	int tmp = Diff_timeval(MyCurrentTime,MyPreviousTime);
	MyPreviousTime = MyCurrentTime;
	return tmp;
}


HX11Window::HX11Window() : dpy(NULL) {}

bool HX11Window::CheckEvent(XEvent *event)
{
	return XCheckWindowEvent(dpy, w, ~0, event);
}


static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
	if((e->type == MapNotify) && (e->xmap.window == (Window)arg))
		return GL_TRUE;
	else
		return GL_FALSE;
}

void HX11Window::open(const char *name)
{
	int errorBase,eventBase;
	XVisualInfo           *vi;
	Colormap              cmap;
	XSetWindowAttributes  swa;
	XEvent                event;
	static int attributes[] = {GLX_STENCIL_SIZE, 1, GLX_DEPTH_SIZE, 16, GLX_RGBA, GLX_DOUBLEBUFFER, None};

	dpy = XOpenDisplay(NULL);
	if(dpy == NULL)
		throw HException("could not open display");

	if(!glXQueryExtension(dpy, &errorBase, &eventBase))
		throw HException("could not query extention");

// find an OpenGL-capable Color Index visual with depth buffer
	vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributes);
	if(vi == NULL)
		throw HException("could not get visual");


// create an OpenGL rendering context in DIRECT mode
	cx = glXCreateContext(dpy, vi,  None, GL_TRUE);
	if(cx == NULL)
		throw HException("could not create rendering context");

// create an X colormap since probably not using default visual
	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
						 vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask /*| SubstructureNotifyMask
		| ButtonPressMask | ButtonReleaseMask| Button1MotionMask*/;

	w = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, m_width,
		m_height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel
		| CWColormap | CWEventMask, &swa);

	XSetStandardProperties(dpy, w, name, name, None, /*argv*/NULL, /*argc*/0, NULL);

	MakeCurrent();

	XMapWindow(dpy, w);
	XIfEvent(dpy, &event, WaitForMapNotify, (char*)w);
//set protocols:
	Atom DelWindowAtom = XInternAtom(dpy,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(dpy, w, &DelWindowAtom, 1);
}

HX11Window::~HX11Window()
{
	XDestroyWindow(dpy,w);
}

void HX11Window::MakeCurrent()
{
	glXMakeCurrent(dpy, w, cx);
}

const char *HX11Window::GetKeyboardDescription()
{
	return "Generic X Windows keyboard";
}

void HX11Window::SetAttrib(int width, int height, bool fullscreen)
{
	m_width = width;
	m_height = height;
	m_fullscreen = fullscreen;
}

static bool KeysymIsPressed(const char *keys, KeySym AKeySym, Display *dpy)
{
    KeyCode AKeyCode = XKeysymToKeycode(dpy,AKeySym);
    return (keys[AKeyCode/8] & (char)(1 << (AKeyCode%8))) != 0;
}

bool HX11Window::IsPressed(H_KEY k)
{
    char keys_return[32];
    XQueryKeymap(dpy,keys_return);
	return KeysymIsPressed(keys_return, HKtoXK(k),dpy);
}

HJoystick *HX11Window::GetJoystick()
{
#ifdef USE_LIN_JOY
	return new LinJoystick();
#else
	return NULL;
#endif //USE_LIN_JOY
}

//swap the screen (or window) buffer
void HX11Window::SwapBuffers()
{
  	glXSwapBuffers(dpy, w);
}


char toASCII(KeySym key)
{
	if (key>=0 && key<=255)
		return char(key);
	else
		return '\0';
}

//default implementations for the application functions:
//CLASS HApplication
HApplication::HApplication() : m_data(new HAppData) {}
HApplication::~HApplication() {delete m_data;}
const char *HApplication::name() {return "generic HApplication";}
void HApplication::on_idle(unsigned int elapsed_time) {/*sleep(1);*/} //change this...

//CLASS HglApplication
HglApplication::HglApplication() : m_window(new HX11Window()) {}
HglApplication::~HglApplication()
{
	delete m_window;
}
void HglApplication::draw_init() {}
void HglApplication::draw_shutdown() {}
void HglApplication::key_down(H_KEY AHKey, char c) {}
void HglApplication::resize(unsigned int width, unsigned int height) {}
void HglApplication::draw() {}


//precision of the return value of ElapsedTime();
int HApplication::TimeRefreshRate()
{
	return 500;
}


//screen mode manipulation:
unsigned int HglApplication::GetScreenMode()
{
	return 0;
}

bool HglApplication::SetScreenMode(unsigned int ScreenMode, ostream &out)
{
	return  false;
}

void HglApplication::PrintScreenMode(unsigned int ScreenMode, ostream &out) {}

void HglApplication::PrintAllScreenModes(ostream &out) {}

//FUNCTIONS

#include <sys/utsname.h>
#include <stdio.h>
const char *SystemDescription()
{
	static char desc[sizeof(utsname)+128];
	utsname uts;
	uname(&uts);
	sprintf(desc,"system name: %s\nrelease: %s\nversion: %s\nmachine: %s\nsizeof(int): %d",
		uts.sysname,
		uts.release,
		uts.version,
		uts. machine,
		(int)sizeof(int));
	return desc;
}

#include <stdlib.h>
const char *ConfigurationFileName()
{
	static char CfgFileName[FILENAME_MAX];
	sprintf(CfgFileName,"%s/.carworldrc",getenv("HOME"));
	return CfgFileName;
}

void Add(HApplication *app)
{
	AppList.push_back(app);
	HglApplication *hgl_app = dynamic_cast<HglApplication*>(app);
	if (hgl_app!=NULL)
	{
		dynamic_cast<HX11Window*>(hgl_app->m_window)->open(hgl_app->name());
		hgl_app->draw_init();
	}
}

int EventLoop(int argc, char **argv)
{
	while(!AppList.empty())
	{
		//for all apps, process window event or do on_idle
		list<HApplication*>::iterator I=AppList.begin();
		do
		{
			HglApplication *app = dynamic_cast<HglApplication *>(*I);
			if (app==NULL)
				(*I)->on_idle((*I)->m_data->ElapsedTime());
			else
			{
				XEvent event;
				HX11Window * w = dynamic_cast<HX11Window*>(app->m_window);
	 			if (!w->CheckEvent(&event))
	 			{
	 				w->MakeCurrent();
	 				app->on_idle((*I)->m_data->ElapsedTime());
	 			}
	 			else
	 			{
	 				switch(event.type)
	 				{
	 				case ConfigureNotify:
	 					w->MakeCurrent();
	 					app->resize((unsigned int)event.xconfigure.width,(unsigned int)event.xconfigure.height);
	 					break;
	 				case KeyPress:
	 					w->MakeCurrent();
	 					KeySym key;
	 					XLookupString((XKeyEvent *)&event, NULL, 0, &key, NULL);
	 					if (XKtoHK(key) != HK_ESCAPE)
						{
	 						app->key_down(XKtoHK(key),toASCII(key));
	 						break;
						}
	 				case DestroyNotify :
	 				case ClientMessage :
	 					w->MakeCurrent();
	 					app->draw_shutdown();
	 					delete (*I);
	 					(*I) = NULL;
	 					break;
	 				}
	 			}
			}
			if ((*I)==NULL)
			{
				I=AppList.erase(I);
			}
			else
				I++;
		} while (I != AppList.end());
	}
	return 0;
}

