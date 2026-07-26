
#ifndef _CAR_WORLD_CLIENT_H_
#define _CAR_WORLD_CLIENT_H_

namespace std {}
using namespace std;

#include "H_Main.h"

#include "H_Input.h"
#include "H_Prompt.h"
#include "H_Variable.h"
#include "CarWorld.h"

#if CARWORLD_ENABLE_NETWORKING
#include <SDL3_net/SDL_net.h>
#endif

#include <queue>

class CarWorldClient : public HglApplication
{
public:
	CarWorldClient(bool full_screen);
	virtual ~CarWorldClient();
//overrided inherited methods
	const char* name();
	
	void draw_init();
	
	void key_down(SDL_Scancode AHKey, SDL_Keycode c);
	void text_input(const char* text);
	void resize(unsigned int width, unsigned int height);
	void set_active(bool active);
	void on_idle(unsigned int elapsed_time);

	void draw();

//methods particular to CarW, executable from the command line
	void toggleconsole();

	void set_r_mode(int mode); //screen mode manipulation (not 100% yet...)
	int get_r_mode();

	void set_joystick(bool use_joystick);
	bool get_joystick();

	void execute_cfg(const char *FileName);
	void pars_command(const char *value);

	void bind(SDL_Scancode key,const char *command);

	void print_help();
	void print_version();

#if CARWORLD_ENABLE_NETWORKING
//connect to a network server
	void join(const char *host, Uint16 port);
#endif

	void write_cfg(ostream &out);
public:
	map<SDL_Scancode,string> KeyBindings;
private:
//graphics
	Hgl_streambuf hbuf;
	Hgl *m_Hgl;
//command line
	bool IsPromptMode;
	map<string,HExecutable*> m_Executables;
//input
	HJoystick *RealJoystick;
	KeyJoystick *FakeJoystick;
	HJoystick *CurrentJoystick;
#if CARWORLD_ENABLE_NETWORKING
//network
	NET_DatagramSocket *m_socket;
	NET_Address *m_serverAddress;
	Uint16 m_serverPort;
	int ID;
	void CloseNetworkConnection(bool notify_server);
	bool SendPacket(const void *data, int size);
	void SendState();
	bool ReceiveState();
	map<int,CWVehicle*> m_Opponents;
#endif
//CarWorld
	CWVehicle *m_Vehicle;
	CarWorld *m_CarWorld;
};

#endif //_CAR_WORLD_CLIENT_H_
