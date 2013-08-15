
#include "H_Standard.h"

#include "H_Keys.h"
#include "H_Main.h"

#include "CarWorldClient.h"
#include "SDLJoystick.h"

#include <ctype.h>
#include <iostream>
#include <map>

#include <stdio.h>

#include <sys/timeb.h>
#include <time.h>

#include <SDL.h>

HWindow::~HWindow() {}
HJoystick::~HJoystick(){}

class HSDLWindow : public HWindow
{
public:
	HSDLWindow(const char* name, int width, int height, bool full_screen);
	virtual ~HSDLWindow();
	bool IsPressed(SDL_Scancode k);
	const char* GetKeyboardDescription();
	HJoystick* GetJoystick();
	virtual void MakeCurrent();
	virtual void SwapBuffers();
	virtual int getWidth();
	virtual int getHeight();
private:
	SDL_Window* displayWindow;
	SDL_GLContext glContext;
};


void HErrorExit(const char *E)
{
	cout << E << endl << flush;
	exit(1);
}

//CLASS HSDLWindow
HSDLWindow::HSDLWindow(const char* name, int width, int height, bool full_screen)
{
	Uint32 flags = SDL_WINDOW_OPENGL;
	if (full_screen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; //| SDL_WINDOW_BORDERLESS
	else
		flags |= SDL_WINDOW_RESIZABLE;
	displayWindow = SDL_CreateWindow(
		name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		flags
	);

	glContext = SDL_GL_CreateContext(displayWindow);
}

HSDLWindow::~HSDLWindow()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(displayWindow);
}

const char *HSDLWindow::GetKeyboardDescription()
{
	return "SDL keyboard";
}

bool HSDLWindow::IsPressed(SDL_Scancode k)
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys==NULL)
		return false;
	return keys[k]!=0;
}

HJoystick* HSDLWindow::GetJoystick()
{
	return new SDLJoystick(0);
}

void HSDLWindow::MakeCurrent()
{
	SDL_GL_MakeCurrent(displayWindow, glContext);
}

void HSDLWindow::SwapBuffers()
{
	SDL_GL_SwapWindow(displayWindow);
}

int HSDLWindow::getWidth()
{
	int w,h;
	SDL_GetWindowSize(displayWindow, &w, &h);
	return w;
}

int HSDLWindow::getHeight()
{
	int w,h;
	SDL_GetWindowSize(displayWindow, &w, &h);
	return h;
}

//default implementations for the application functions:
//CLASS HApplication
HApplication::HApplication() {}
HApplication::~HApplication() {}
const char* HApplication::name() {return "generic HApplication";}
void HApplication::on_idle(unsigned int elapsed_time) {}

int HApplication::TimeRefreshRate()
{
	return 1000;
}

//CLASS HglApplication
HglApplication::HglApplication(int width, int height, bool full_screen) : m_window(new HSDLWindow("cool", width,height,full_screen)) {}
HglApplication::~HglApplication() {delete m_window;}
void HglApplication::draw_init() {}
void HglApplication::key_down(SDL_Scancode AHKey, SDL_Keycode c) {}
void HglApplication::resize(unsigned int width, unsigned int height) {}
void HglApplication::draw() {}


//START Screen managment functions
unsigned int HglApplication::GetScreenMode()
{
//BUG implement this function
	cout << "WARNING: this function is not yet implemented." << endl;
	return 0;
}

bool HglApplication::SetScreenMode(unsigned int ScreenMode, ostream &out)
{
//BUG implement this function
	cout << "WARNING: this function is not yet implemented." << endl;
	return false;
}

void HglApplication::PrintScreenMode(unsigned int ScreenMode, ostream &out)
{
//BUG implement this function
	cout << "WARNING: this function is not yet implemented." << endl;
}

void HglApplication::PrintAllScreenModes(ostream &out)
{
//BUG implement this function
	cout << "WARNING: this function is not yet implemented." << endl;
}

const char* ConfigurationFileName()
{
	return "carworld.cfg";
}

int find(int argc, char **argv, const char *v)
{
	int i=0;
	while (i<argc && strcmp(argv[i],v)) i++;
	return i;
}

extern ofstream herr;

int main(int argc, char *argv[])
{
	cout << "Hello" << endl;
	streambuf* cout_streambuf = cout.rdbuf();
	HglApplication* app = NULL;
	try
	{

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

		{
			SDL_version CompileVer;
			SDL_VERSION(&CompileVer);
			cout << "SDL compile time version: " << (int)CompileVer.major << "." << (int)CompileVer.minor << "." << (int)CompileVer.patch << endl;
		}

		{
			SDL_version DynamicVer;
			SDL_GetVersion(&DynamicVer);
			cout << "SDL run time version: " << (int)DynamicVer.major << "." << (int)DynamicVer.minor << "." << (int)DynamicVer.patch << endl;
		}

		{
			cout << ((SDL_BYTEORDER==SDL_LIL_ENDIAN) ? "little endian" : "big endian") << endl;
		}
		
		bool full_screen = find(argc,argv,"-f")!=argc;
		HglApplication* app = new CarWorldClient(full_screen);

		bool done = false;
		Uint32 CurrentTime = SDL_GetTicks();
		while (!done)
		{
			//SDL_PumpEvents();
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch(event.type)
				{
				case SDL_KEYDOWN:
					{
						if (event.key.keysym.sym==SDLK_ESCAPE)
							done = true;
						else
						{
							app->key_down(event.key.keysym.scancode, event.key.keysym.sym);
						}
					}
					break;
				case SDL_TEXTINPUT:
					{
						app->text_input(event.text.text);
					}
					break;
			//BUG support window resizing here...
				case SDL_WINDOWEVENT:
					{
						switch (event.window.event)
						{
						case SDL_WINDOWEVENT_RESIZED:
//BUG if i call SDL_SetVideoMode() on win32 i lose all textures and lists...
#ifndef WIN32
							//SDL_SetVideoMode(event.window.data1, event.window.data2, bpp, flags);
#endif //WIN32
							app->resize(event.window.data1, event.window.data2);
							break;
						default:
							break;
						}
					}
					break;
					
				case SDL_QUIT:
					done = true;
					break;
				}
			}
			Uint32 NewTime = SDL_GetTicks();
			app->on_idle(NewTime-CurrentTime);
			CurrentTime = NewTime;
			app->draw();
		}
		delete app;
		app = NULL;
		cout.rdbuf(cout_streambuf);
		SDL_Quit();
		return 0;
	}
	catch (const HException &E)
	{
	//resor cout to normal
		cout.rdbuf(cout_streambuf);

	//write the error message in log file and standard out
		herr << E.what() << endl << flush;
		cout << E.what() << endl << flush;

	//free resources
	//we do this last in case it crashes...
		delete app;

		return 1;
	}
}
