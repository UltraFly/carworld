
/*======================================================================================*/
/*======================== Windows95 version of the main ===============================*/
/*======================================================================================*/

#include "../H_Standard.h"
#include "../H_Keys.h"
#include "../H_Main.h"
#include "DXJoystick.h"
#include "Win32Internals.h"

#include <ctype.h>
#include <iostream>
#include <strstream>
#include <map>

#include <stdio.h>

#include <sys/timeb.h>
#include <time.h>

#include <windows.h>
#include <windef.h>

HINSTANCE hCurrInst;
static list<HApplication*> AppList;

//Screen managment
static unsigned int NbScreenModes = 0;
static unsigned int CurrenScreenMode = 7; //curent display mode == devmodes[current]
static DEVMODE *devmodes = NULL;

H_KEY VKtoHK(int VK)
{
	return (H_KEY)VK;
}

int HKtoVK(H_KEY HK)
{
	return HK;
}

class HAppData
{
public:
	HAppData();
	int ElapsedTime();
public:
	LONGLONG PreviousTime;
};

//Timer related functions:
static LONGLONG GetCurrentMilliSec()
{
	static LARGE_INTEGER Frequency;
	static bool PerformanceTimer = (QueryPerformanceFrequency(&Frequency) != 0);

	if (PerformanceTimer)
	{
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		return (tmp.QuadPart/(Frequency.QuadPart/1000));
	}
	else
	{
		struct _timeb Current_timeb;
		_ftime( &Current_timeb);
		return Int32x32To64(1000,Current_timeb.time)+Current_timeb.millitm;
	}
}

HAppData::HAppData() : PreviousTime(GetCurrentMilliSec()) {}

//time since this function was last called
//0 if it is the first time
int HAppData::ElapsedTime()
{
	LONGLONG CurrentTime = GetCurrentMilliSec();
	int tmp = int(CurrentTime - PreviousTime);
	PreviousTime = CurrentTime;
	return tmp;
}

void HErrorExit(const char *E)
{
	cerr.flush();
    ChangeDisplaySettings(NULL, 0);
	MessageBox(NULL,E,"Application terminated",MB_OK);
	exit(1);
}

//CLASS HWinWindow
HWinWindow::HWinWindow() :
	m_DXJoystick(NULL)
{}

HWinWindow::~HWinWindow()
{
	delete m_DXJoystick;
}

const char *HWinWindow::GetKeyboardDescription()
{
	return "Generic Windows95/NT keyboard";
}

bool HWinWindow::IsPressed(H_KEY k)
{
	return GetAsyncKeyState( HKtoVK(k))!=0;
}

HJoystick *HWinWindow::GetJoystick()
{
	delete m_DXJoystick;
	m_DXJoystick = new DXJoystick(hInst, hWnd);
	return m_DXJoystick;
}

void HWinWindow::MakeCurrent()
{
	//doing this simple test is a _lot_ faster...
	static HWinWindow *current = NULL;
	if (current!=this)
		wglMakeCurrent(hDC, hRC);
	current = this;
}

void HWinWindow::SwapBuffers()
{
	::SwapBuffers(hDC);
}

void HWinWindow::SetAttrib(int width, int height, bool fullscreen)
{
	m_width = width;
	m_height = height;
	m_fullscreen = fullscreen;
}

//default implementations for the application functions:
//CLASS HApplication
HApplication::HApplication() : m_data(new HAppData) {}
HApplication::~HApplication() {delete m_data;}
const char *HApplication::name() {return "generic HApplication";}
void HApplication::on_idle(unsigned int elapsed_time) {/*sleep(1);*/} //change this...

int HApplication::TimeRefreshRate()
{
	//if the timer is the PerformanceTimer precision guaranteed to .3 sec
	//else, with the normal timer, 3s
	LARGE_INTEGER Frequency;
	return (QueryPerformanceFrequency(&Frequency) != 0) ? 300 : 3000; //mili seconds
}


//CLASS HglApplication
HglApplication::HglApplication() : m_window(new HWinWindow()) {}
HglApplication::~HglApplication() {delete m_window;}
void HglApplication::draw_init() {}
void HglApplication::draw_shutdown() {}
void HglApplication::key_down(H_KEY AHKey, char c) {}
void HglApplication::resize(unsigned int width, unsigned int height) {}
void HglApplication::draw() {}


//START Screen managment functions
unsigned int HglApplication::GetScreenMode()
{
	return CurrenScreenMode;
}
static void InitScreenModes()
{
    /* get the total number of modes so we can allocate memory for all
       of 'em. */
    NbScreenModes = 0;
	DEVMODE devmode;
    while (EnumDisplaySettings(NULL, NbScreenModes, &devmode))
		NbScreenModes++;

    /* fill an array with all the devmodes so we don't have to keep
       grabbing 'em. */
	devmodes = new DEVMODE[NbScreenModes];
    NbScreenModes = 0;
    while (EnumDisplaySettings(NULL, NbScreenModes, &devmodes[NbScreenModes]))
		NbScreenModes++;
}

static void FreeScreenModes()
{
    delete [] devmodes;
}

bool HglApplication::SetScreenMode(unsigned int ScreenMode, ostream &out)
{
	HWinWindow *win = dynamic_cast<HWinWindow*>(m_window);
	//if (!IsFullScreen)
	{
		out << "option only available in Windows full screen mode.\n";
		return false;
	}
	if (ScreenMode >= NbScreenModes)
	{
		out << "invalid screen mode.\n";
		return false;
	}
	if (ChangeDisplaySettings(&devmodes[ScreenMode], CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
	{
		//destroy the current window
		/*DestroyWindow(hWnd);*/

		//change the screen settings
		ChangeDisplaySettings(&devmodes[ScreenMode], 0);
		//make sure the window has the right size
		MoveWindow(win->hWnd,0,0,devmodes[ScreenMode].dmPelsWidth,devmodes[ScreenMode].dmPelsHeight,TRUE);
		CurrenScreenMode = ScreenMode;
		out << "screen mode set to: ";
		PrintScreenMode(ScreenMode,cout);

		//create a new window
		/*hWnd = CreateWindow(BH_AppName, BH_AppName, WS_POPUP | WS_MAXIMIZE |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				DEFAULT_X, DEFAULT_Y, DEFAULT_WIDTH, DEFAULT_HEIGHT, NULL, NULL, hInst, NULL);
		if (hWnd == NULL)
				throw HException("CreateWindow() failed:  Cannot create a window.");
		//display window
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);*/

		return true;
	}
	out << "unable to change to mode:\n";
	PrintScreenMode(ScreenMode,out);
	return false;
}

void HglApplication::PrintScreenMode(unsigned int ScreenMode, ostream &out)
{
	if (ScreenMode < NbScreenModes)
	{
		out <<(int)(devmodes[ScreenMode].dmPelsWidth)<<"x"<<(int)(devmodes[ScreenMode].dmPelsHeight)
			<<" "<<(int)(devmodes[ScreenMode].dmBitsPerPel)<<" bpp\n";
	}
}

void HglApplication::PrintAllScreenModes(ostream &out)
{
	for (unsigned int i=0 ; i<NbScreenModes ; i++)
	{
		out << i << ": ";
		PrintScreenMode(i,out);
	}
}

static void setupPixelFormat(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd = {
		sizeof (PIXELFORMATDESCRIPTOR),             // Size of this structure
		1,                                          // Version number
		PFD_DRAW_TO_WINDOW |                        // Flags
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,                              // RGBA pixel values
		16,                                         // 16-bit color
		0, 0, 0, 0, 0, 0,                           // Don't care about these
		0, 0,                                       // No alpha buffer
		0, 0, 0, 0, 0,                              // No accumulation buffer
		16,                                         // 16-bit depth buffer
		8,                                          // 8 bit stencil buffer
		0,                                          // No auxiliary buffers
		PFD_MAIN_PLANE,                             // Layer type
		0,                                          // Reserved (must be 0)
		0, 0, 0                                     // No layer masks
	};
    int pixelFormat;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0)
			throw HException( "ChoosePixelFormat() failed:"
				"Cannot find a suitable pixel format.");

    if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE)
			throw HException( "SetPixelFormat() failed:"
				"Cannot set format specified.");

	//TracePixelFormat(hDC);
}

static void setupPalette(HWinWindow *w)
{
    int pixelFormat = GetPixelFormat(w->hDC);
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE* pPal;
    int paletteSize;

    DescribePixelFormat(w->hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        paletteSize = 1 << pfd.cColorBits;
    } else {
        return;
    }

    pPal = (LOGPALETTE*)
        malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = paletteSize;

    /* build a simple RGB color palette */
    {
        int redMask = (1 << pfd.cRedBits) - 1;
        int greenMask = (1 << pfd.cGreenBits) - 1;
        int blueMask = (1 << pfd.cBlueBits) - 1;
        int i;

        for (i=0; i<paletteSize; ++i) {
            pPal->palPalEntry[i].peRed =
                    (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
            pPal->palPalEntry[i].peGreen =
                    (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
            pPal->palPalEntry[i].peBlue =
                    (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
            pPal->palPalEntry[i].peFlags = 0;
        }
    }

    w->hPalette = CreatePalette(pPal);
    free(pPal);

    if (w->hPalette) {
        SelectPalette(w->hDC, w->hPalette, FALSE);
        RealizePalette(w->hDC);
    }
}


static LONG WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static map<HWND,HglApplication*> WindowMap;

	HglApplication *app = NULL;
	HWinWindow *w = NULL;

	if (msg==WM_CREATE)
	{
		app = (HglApplication *)(((LPCREATESTRUCT) lParam)->lpCreateParams);
		WindowMap[hWnd] = app;
	}

	if (WindowMap.find(hWnd)!=WindowMap.end())
	{
		app = (*(WindowMap.find(hWnd))).second;
		w = (HWinWindow*)(app->m_window);
	}

	try
	{
		switch(msg)
		{
		case WM_CREATE:
			//initialize OpenGL rendering
			w->hWnd = hWnd;
			w->hDC = GetDC(hWnd);
			setupPixelFormat(w->hDC);
			setupPalette(w);
			w->hRC = wglCreateContext(w->hDC);
			w->MakeCurrent();
			app->draw_init();
			return 0;

		case WM_CLOSE :
			DestroyWindow(w->hWnd);
			delete app->m_window;
			app->m_window = NULL;
			PostQuitMessage(0);
			return 0;

		case WM_DESTROY:
			w->MakeCurrent();
			app->draw_shutdown();
			if (w->hRC)
			{
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(w->hRC);
			}
			if (w->hPalette)
				DeleteObject(w->hPalette);
			ReleaseDC(w->hWnd, w->hDC);
			return 0;

        case WM_ACTIVATE:   // sent when window changes active state
			if (w->m_DXJoystick!=NULL)
				w->m_DXJoystick->SetAcquire(wParam != WA_INACTIVE);
            return TRUE;
            break;

		case WM_SIZE:
			w->MakeCurrent();
			app->resize((int)LOWORD(lParam), (int)HIWORD(lParam));
			return 0;

		case WM_PALETTECHANGED:
			w->MakeCurrent();
			//realize palette if this is *not* the current window */
			if (w->hRC && w->hPalette && (HWND) wParam != w->hWnd)
			{
				UnrealizeObject(w->hPalette);
				SelectPalette(w->hDC, w->hPalette, FALSE);
				RealizePalette(w->hDC);
				app->draw();
				break;
			}
			break;

		case WM_QUERYNEWPALETTE:
			w->MakeCurrent();
			//realize palette if this is the current window */
			if (w->hRC && w->hPalette)
			{
				UnrealizeObject(w->hPalette);
				SelectPalette(w->hDC, w->hPalette, FALSE);
				RealizePalette(w->hDC);
				app->draw();
				return TRUE;
			}
			break;

		case WM_PAINT:
			w->MakeCurrent();
			app->draw();
			ValidateRect(w->hWnd, NULL);
			return 0;

		case WM_KEYDOWN:
			w->MakeCurrent();
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);
			else
			{
				BYTE KeyboardState[256];
				GetKeyboardState(KeyboardState);
				unsigned short buffer[8];
				if (ToAscii(wParam, 0,KeyboardState,buffer,0)==1 && (255>buffer[0]) && isprint(buffer[0]))
					app->key_down(VKtoHK(wParam),(char)buffer[0]);
				else
					app->key_down(VKtoHK(wParam),'\0');
			}
			return 0;

		case WM_ACTIVATEAPP :
			w->MakeCurrent();
			//bActive = (wParam != 0);
			return 0;

		case WM_SETCURSOR:
			w->MakeCurrent();
			if (w->m_fullscreen)
			{
				SetCursor(NULL);
				return TRUE;
			}
			break;
		}
	}
	catch (const exception &E)
	{
		HErrorExit(E.what());
	}

	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

static void TracePixelFormat(HDC hdc, ostream &out)
{
	out << "TracePixelFormat\n";

	int nPixelFormat;
	int nbIndex;
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory((LPVOID)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;

	nPixelFormat = GetPixelFormat(hdc);
	nbIndex = DescribePixelFormat( 
		hdc, 
		nPixelFormat,
		sizeof(PIXELFORMATDESCRIPTOR),
		&pfd);
	char msg[1000];
	sprintf(msg, "nbIndex = %d\n", nbIndex);
	sprintf(msg, "iPixelType = %d\ncColorBits=%d\ncRedBits=%d\ncRedShift=%d\ncGreenBits=%d\ncGreenShift=%d\ncBlueBits=%d\ncBlueShift=%d\ncAlphaBits=%d\ncAlphaShift=%d\ncAccumBits=%d\ncAccumRedBits=%d\ncAccumGreenBits=%d\ncAccumBlueBits=%d\ncAccumAlphaBits=%d\ncDepthBits=%d\ncStencilBits=%d\ncAuxBuffers=%d\niLayerType=%d\nbReserved=%d\ndwLayerMask=%d\ndwVisibleMask=%d\ndwDamageMask=%d\n",
	pfd.iPixelType,       pfd.cColorBits,       pfd.cRedBits, 
		pfd.cRedShift,       pfd.cGreenBits,       pfd.cGreenShift, 
		pfd.cBlueBits,       pfd.cBlueShift,       pfd.cAlphaBits, 
		pfd.cAlphaShift,       pfd.cAccumBits,       pfd.cAccumRedBits, 
		pfd.cAccumGreenBits,       pfd.cAccumBlueBits,       pfd.cAccumAlphaBits, 
		pfd.cDepthBits,       pfd.cStencilBits,       pfd.cAuxBuffers, 
		pfd.iLayerType,       pfd.bReserved,      pfd.dwLayerMask, 
		pfd.dwVisibleMask,      pfd.dwDamageMask);
	out << msg;
}


void open_window(
	HglApplication *app,
	HINSTANCE hCurrentInst
)
{
	HWinWindow *w = dynamic_cast<HWinWindow*>(app->m_window);
	w->hInst = hCurrentInst;

	static bool IsRegistered = false;
	//register the class:
	if (!IsRegistered)
	{
		WNDCLASS wndClass;
		wndClass.style         = CS_OWNDC;
		wndClass.lpfnWndProc   = (WNDPROC)WindowProc;
		wndClass.cbClsExtra    = 0;
		wndClass.cbWndExtra    = 0;
		wndClass.hInstance     = hCurrentInst;
		wndClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
		wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = NULL;
		wndClass.lpszMenuName  = NULL;
		wndClass.lpszClassName = "HlgWindow";
		if (!RegisterClass(&wndClass))
			throw HException( "RegisterClass() failed:\nCannot register window class.");
		IsRegistered = true;
	}

	//cout << "create window\n";
	DWORD WindowStyle;
	if (w->m_fullscreen)
	{
		//ChangeDisplaySettings(&devmodes[1], 0);
		WindowStyle = WS_POPUP | WS_MAXIMIZE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		WindowStyle =	WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN |
						WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
	}
	w->hWnd = CreateWindow("HlgWindow", app->name(), WindowStyle, BH_DEFAULT_X, BH_DEFAULT_Y,
		w->m_width, w->m_height, NULL, NULL, hCurrentInst, app);
	if (w->hWnd == NULL)
			throw HException("CreateWindow() failed:  Cannot create a window.");

	//display window
	ShowWindow(w->hWnd, SW_SHOW);
	UpdateWindow(w->hWnd);
}

const char *SystemDescription()
{
	return "Microsoft Windows\n";
}

const char *ConfigurationFileName()
{
	return "carworld.cfg";
}

void Add(HApplication *app)
{
	AppList.push_back(app);
	HglApplication *hgl_app = dynamic_cast<HglApplication*>(app);
	if (hgl_app!=NULL)
	{
		open_window(hgl_app, hCurrInst);
	}
}


int EventLoop(int argc, char **argv)
{
	MSG msg;
	try
	{
		do
		{
			if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				for (list<HApplication*>::iterator I=AppList.begin(); I!=AppList.end() ; I++)
				{
					HglApplication *app = dynamic_cast<HglApplication *>(*I);
					if (app!=NULL && app->m_window != NULL)
						app->m_window->MakeCurrent();
					(*I)->on_idle((*I)->m_data->ElapsedTime());
				}
			}
		} while (msg.message != WM_QUIT);
		//delete all applications
		for (list<HApplication*>::iterator I=AppList.begin(); I!=AppList.end() ; I++)
			delete (*I);
		FreeScreenModes();
	}
	catch (const exception &E)
	{
		cerr.flush();
		HErrorExit(E.what());
	}

	//put the display settings back to default just in case...
    ChangeDisplaySettings(NULL, 0);
    return msg.wParam;
}

extern int main(int argc, char **argv);

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{
	InitScreenModes();
	hCurrInst=hCurrentInst;
	Command command_line(GetCommandLine());
	char** argv = new char*[command_line.size()];
	for (int i=0 ; i<command_line.size() ; i++)
		argv[i] = (char*)command_line[i].c_str();

	int ret = 0;
	try
	{
		main(command_line.size(),argv);
	}
	catch (const exception &E)
	{
		HErrorExit(E.what());
	}
	return ret;
}



