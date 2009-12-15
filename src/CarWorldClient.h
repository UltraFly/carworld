
#ifndef _CAR_WORLD_CLIENT_H_
#define _CAR_WORLD_CLIENT_H_

namespace std {}
using namespace std;

#include "H_Main.h"

#include "H_Input.h"
#include "H_Prompt.h"
#include "H_Variable.h"
#include "H_Socket.h"
#include "CarWorld.h"

#include <queue>

class CarWorldClient : public HglApplication
{
public:
	CarWorldClient(bool full_screen);
	virtual ~CarWorldClient();
//overrided inherited methods
	const char* name();
	
	void draw_init();

	void key_down(SDLKey AHKey, char c);
	void resize(unsigned int width, unsigned int height);
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

	void bind(SDLKey key,const char *command);

	void print_help();
	void print_version();

//connect to a network server
	void join(const char *host, short port);

	void write_cfg(ostream &out);
public:
	map<SDLKey,string> KeyBindings;
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
//network
	HSocClient *m_socket;
	int ID;
	void SendState();
	bool RecieveState();
	map<int,CWVehicle*> m_Opponents;
//CarWorld
	CWVehicle *m_Vehicle;
	CarWorld *m_CarWorld;
};

#endif //_CAR_WORLD_CLIENT_H_
