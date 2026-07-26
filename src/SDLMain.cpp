
#include "H_Standard.h"

#include "H_Keys.h"
#include "H_Main.h"

#include "CarWorldClient.h"
#if CARWORLD_ENABLE_NETWORKING
#include "CarWorldNet.h"
#include "CarWorldServer.h"
#endif
#include "SDLJoystick.h"

#include <ctype.h>
#include <cstdlib>
#include <iostream>
#include <map>

#include <stdio.h>

#include <sys/timeb.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#if CARWORLD_ENABLE_NETWORKING
#include <SDL3_net/SDL_net.h>
#endif

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
	SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
	if (full_screen)
		flags |= SDL_WINDOW_FULLSCREEN; //| SDL_WINDOW_BORDERLESS
	else
		flags |= SDL_WINDOW_RESIZABLE;
	displayWindow = SDL_CreateWindow(
		name,
		width,
		height,
		flags
	);
	if (displayWindow==NULL)
		throw HException(string("SDL could not create a window: ")+SDL_GetError());

	glContext = SDL_GL_CreateContext(displayWindow);
	if (glContext==NULL)
	{
		SDL_DestroyWindow(displayWindow);
		displayWindow = NULL;
		throw HException(string("SDL could not create an OpenGL context: ")+SDL_GetError());
	}
	if (!SDL_StartTextInput(displayWindow))
	{
		SDL_GL_DestroyContext(glContext);
		SDL_DestroyWindow(displayWindow);
		glContext = NULL;
		displayWindow = NULL;
		throw HException(string("SDL could not enable text input: ")+SDL_GetError());
	}
}

HSDLWindow::~HSDLWindow()
{
	if (displayWindow!=NULL)
		SDL_StopTextInput(displayWindow);
	if (glContext!=NULL)
		SDL_GL_DestroyContext(glContext);
	if (displayWindow!=NULL)
		SDL_DestroyWindow(displayWindow);
}

const char *HSDLWindow::GetKeyboardDescription()
{
	return "SDL keyboard";
}

bool HSDLWindow::IsPressed(SDL_Scancode k)
{
	const bool* keys = SDL_GetKeyboardState(NULL);
	if (keys==NULL)
		return false;
	return keys[k];
}

HJoystick* HSDLWindow::GetJoystick()
{
	SDLJoystick* joystick = new SDLJoystick(0);
	if (!joystick->IsValid())
	{
		delete joystick;
		return NULL;
	}
	return joystick;
}

void HSDLWindow::MakeCurrent()
{
	if (!SDL_GL_MakeCurrent(displayWindow, glContext))
		throw HException(string("SDL could not activate the OpenGL context: ")+SDL_GetError());
}

void HSDLWindow::SwapBuffers()
{
	SDL_GL_SwapWindow(displayWindow);
}

int HSDLWindow::getWidth()
{
	int w=0,h=0;
	SDL_GetWindowSize(displayWindow, &w, &h);
	return w;
}

int HSDLWindow::getHeight()
{
	int w=0,h=0;
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
HglApplication::HglApplication(int width, int height, bool full_screen) : m_window(new HSDLWindow("CarWorld", width,height,full_screen)) {}
HglApplication::~HglApplication() {delete m_window;}
void HglApplication::draw_init() {}
void HglApplication::key_down(SDL_Scancode AHKey, SDL_Keycode c) {}
void HglApplication::resize(unsigned int width, unsigned int height) {}
void HglApplication::set_active(bool active) {(void)active;}
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
	HApplication* app = NULL;
	HglApplication* graphical_app = NULL;
	bool sdl_initialized = false;
#if CARWORLD_ENABLE_NETWORKING
	bool network_initialized = false;
#endif
	try
	{
#if CARWORLD_ENABLE_NETWORKING
		int server_argument = find(argc,argv,"-server");
		bool server_mode = server_argument!=argc;
		Uint16 server_port = DEFAULT_PORT;
		if (server_mode && server_argument+1<argc)
		{
			char *end = NULL;
			long port = strtol(argv[server_argument+1], &end, 10);
			if (end==argv[server_argument+1] || *end!='\0' ||
				port<1 || port>65535)
				throw HException("server port must be an integer from 1 to 65535.");
			server_port = static_cast<Uint16>(port);
		}
#else
		bool server_mode = false;
#endif

		SDL_InitFlags subsystems = server_mode
			? 0
			: SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC;
		if (!SDL_Init(subsystems))
			throw HException(string("SDL could not initialize: ")+SDL_GetError());
		sdl_initialized = true;

#if CARWORLD_ENABLE_NETWORKING
		if (!NET_Init())
			throw HException(string("SDL_net could not initialize: ")+SDL_GetError());
		network_initialized = true;
#endif

		{
			int CompileVer = SDL_VERSION;
			cout << "SDL compile time version: "
				<< SDL_VERSIONNUM_MAJOR(CompileVer) << "."
				<< SDL_VERSIONNUM_MINOR(CompileVer) << "."
				<< SDL_VERSIONNUM_MICRO(CompileVer) << endl;
		}

		{
			int DynamicVer = SDL_GetVersion();
			cout << "SDL run time version: "
				<< SDL_VERSIONNUM_MAJOR(DynamicVer) << "."
				<< SDL_VERSIONNUM_MINOR(DynamicVer) << "."
				<< SDL_VERSIONNUM_MICRO(DynamicVer) << endl;
		}

		{
			cout << ((SDL_BYTEORDER==SDL_LIL_ENDIAN) ? "little endian" : "big endian") << endl;
		}

#if CARWORLD_ENABLE_NETWORKING
		if (server_mode)
			app = new CarWorldServer(server_port);
		else
#endif
		{
			bool full_screen = find(argc,argv,"-f")!=argc;
			graphical_app = new CarWorldClient(full_screen);
			app = graphical_app;
		}

		bool done = false;
		Uint64 CurrentTime = SDL_GetTicks();
		while (!done)
		{
			if (graphical_app!=NULL)
			{
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					switch(event.type)
					{
					case SDL_EVENT_KEY_DOWN:
						if (event.key.key==SDLK_ESCAPE)
							done = true;
						else
							graphical_app->key_down(
								event.key.scancode,
								event.key.key
							);
						break;
					case SDL_EVENT_TEXT_INPUT:
						graphical_app->text_input(event.text.text);
						break;
					case SDL_EVENT_WINDOW_RESIZED:
//BUG if i call SDL_SetVideoMode() on win32 i lose all textures and lists...
#ifndef WIN32
						//SDL_SetVideoMode(event.window.data1, event.window.data2, bpp, flags);
#endif //WIN32
						graphical_app->resize(
							event.window.data1,
							event.window.data2
						);
						break;
					case SDL_EVENT_WINDOW_FOCUS_GAINED:
						graphical_app->set_active(true);
						break;
					case SDL_EVENT_WINDOW_FOCUS_LOST:
						graphical_app->set_active(false);
						break;
					case SDL_EVENT_QUIT:
						done = true;
						break;
					}
				}
			}
			Uint64 NewTime = SDL_GetTicks();
			app->on_idle(static_cast<unsigned int>(NewTime-CurrentTime));
			CurrentTime = NewTime;
			if (graphical_app!=NULL)
				graphical_app->draw();
			else
				SDL_Delay(1);
		}
		delete app;
		app = NULL;
		graphical_app = NULL;
		cout.rdbuf(cout_streambuf);
#if CARWORLD_ENABLE_NETWORKING
		NET_Quit();
		network_initialized = false;
#endif
		SDL_Quit();
		sdl_initialized = false;
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
#if CARWORLD_ENABLE_NETWORKING
		if (network_initialized)
			NET_Quit();
#endif
		if (sdl_initialized)
			SDL_Quit();

		return 1;
	}
}
