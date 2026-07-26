
#include "H_Standard.h"
#include "CarWorldClient.h"
#if CARWORLD_ENABLE_NETWORKING
#include "CarWorldNet.h"
#endif
#include "CWVersion.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "H_Variable.h"
#include <SDL3/SDL_keyboard.h>

#if CARWORLD_ENABLE_NETWORKING
#define CLIENT_TIMEOUT 200
#endif


static const char * const help_text  =
	"available commands:\n"
	"help       : display this message\n"
	"version    : print version information\n"
	"set        : set a variable to a value\n"
	"exec       : execute configuration file\n"
	"bind       : bind a key to a command\n";

class ExecCFG : public HExecutable
{
public:
	ExecCFG(CarWorldClient *CWC) : CWC(CWC) {}
	void exec(const Command &c)
	{
		if (c.size()==2)
			CWC->execute_cfg(c[1].c_str());
		else
			cout << "usage: exec <file>\n";
	}
	virtual ~ExecCFG() {}
private:
	CarWorldClient *CWC;
};

class JoinServer : public HExecutable
{
public:
	JoinServer(CarWorldClient *CWC) : CWC(CWC) {}
	void exec(const Command &c)
	{
#if CARWORLD_ENABLE_NETWORKING
		if ((c.size()!=2) && (c.size()!=3))
			cout << "usage: join <server name> [<port>]\n";
		else
		{
			long port = DEFAULT_PORT;
			if (c.size()==3)
			{
				char *end = NULL;
				port = strtol(c[2].c_str(), &end, 10);
				if (end==c[2].c_str() || *end!='\0' || port<1 || port>65535)
				{
					cout << "port must be an integer from 1 to 65535.\n";
					return;
				}
			}
			CWC->join(c[1].c_str(), static_cast<Uint16>(port));
		}
#else
		(void)c;
		cout << "networking is disabled in this build.\n";
#endif
	}
	virtual ~JoinServer() {}
private:
	CarWorldClient *CWC;
};

template <class T> void writeToStream(ostream& out, T v)
{
	out << v;
}

class BindKey : public HExecutable
{
public:
	BindKey(CarWorldClient *CWC) : CWC(CWC) {}
	void exec(const Command &c)
	{
		if (c.size()==3)
			CWC->bind(SDL_GetScancodeFromName(c[1].c_str()), c[2].c_str());
		else
			cout << "usage: bind <key> \"<command line>\"\n";
	}
	void serialize(ostream &out)
	{
		map<SDL_Scancode,string>::iterator I;
		for (I = CWC->KeyBindings.begin() ; I!=CWC->KeyBindings.end(); I++)
		{
			out << "bind " << SDL_GetScancodeName((*I).first) << " \"";
			writeToStream(out, (*I).second);
			out << "\"\n";
		}
	}
	virtual ~BindKey() {}
private:
	CarWorldClient *CWC;
};

//CLASS CarWorldClient
CarWorldClient::CarWorldClient(bool full_screen) :
	HglApplication(640, 480, full_screen),
	m_Hgl(NULL),
	IsPromptMode(false),
	RealJoystick(NULL),
	FakeJoystick(NULL),
	CurrentJoystick(NULL),
#if CARWORLD_ENABLE_NETWORKING
	m_socket(NULL),
	m_serverAddress(NULL),
	m_serverPort(0),
	ID(-1),
#endif
	m_Vehicle(NULL),
	m_CarWorld(NULL)
{
//switch cout to display in the on screen prompt + log
	cout.rdbuf(&hbuf);
	cout << "starting " << name() << " ...\n\n";
	m_CarWorld = new CarWorld(TimeRefreshRate(),DEFAULT_LANDSCAPE);
	m_Vehicle = new CWVehicle(DEFAULT_VEHICLE);
	m_CarWorld->add(m_Vehicle);

	draw_init();

	resize(m_window->getWidth(), m_window->getHeight());
}

void CarWorldClient::draw_init()
{
	cout.rdbuf(&hbuf);

	cout << "initiating graphics...\n";
	m_Hgl = new Hgl(m_window);
	m_Hgl->MakeCurrent();
	cout << endl;

	m_CarWorld->draw_init();

	Hgl::ThrowError(); //check to see if everything is OK

	cout << "initiating input...\n";
	FakeJoystick = new KeyJoystick(m_window);
	CurrentJoystick = FakeJoystick;

	cout << "\ninitiating command line parameters...\n";
	HExecutableSet *m_HExecutableSet = new HExecutableSet();
	m_HExecutableSet->add(new HVar<bool>("gl_finish", &Hgl::SetFinish,&Hgl::GetFinish));
	m_HExecutableSet->add(new HVar<bool>("gl_vertex_arrays", &Hgl::SetVertexArrays,&Hgl::GetVertexArrays));
	m_HExecutableSet->add(new HVar<bool>("gl_ext_compiled_vertex_array",&Hgl::SetExtCompiledVertexArrays,&Hgl::GetExtCompiledVertexArrays));
	m_HExecutableSet->add(new HVar<int>("gl_shadows", &Hgl::SetShadows, &Hgl::GetShadows));
	m_HExecutableSet->add(new HVar<Hgl::Enum>("gl_texturemode",&Hgl::SetTextureMode,&Hgl::GetTextureMode));
	m_HExecutableSet->add(new HVar<bool>("gl_use_opt",&OFFObject::UseOptimizedDraw));
	m_HExecutableSet->add(new HVar<bool>("draw_background",&(m_CarWorld->draw_background)));
	m_HExecutableSet->add(new HVarObj<CarWorldClient,int>("r_mode", this, &CarWorldClient::set_r_mode,&CarWorldClient::get_r_mode));
	m_HExecutableSet->add(new HVarObj<CarWorldClient,bool>("use_joystick",this, &CarWorldClient::set_joystick,&CarWorldClient::get_joystick));
	m_HExecutableSet->add(new HVar<HKey>("accel",&(FakeJoystick->up_key)));
	m_HExecutableSet->add(new HVar<HKey>("brake",&(FakeJoystick->down_key)));
	m_HExecutableSet->add(new HVar<HKey>("left",&(FakeJoystick->left_key)));
	m_HExecutableSet->add(new HVar<HKey>("right",&(FakeJoystick->right_key)));
	m_HExecutableSet->add(new HVar<HKey>("handbrake",&(FakeJoystick->button_key)));

	m_Executables["set"] = m_HExecutableSet;
	m_Executables["join"] = new JoinServer(this);
	m_Executables["bind"] = new BindKey(this);
	m_Executables["reset"] = new MethodCall<CarWorld>(m_CarWorld,&CarWorld::reset);
	m_Executables["next_camera"] = new MethodCall<CarWorld>(m_CarWorld,&CarWorld::next_camera);
	m_Executables["toggleconsole"] = new MethodCall<CarWorldClient>(this,&CarWorldClient::toggleconsole);
	m_Executables["help"] = new MethodCall<CarWorldClient>(this,&CarWorldClient::print_help);
	m_Executables["version"] = new MethodCall<CarWorldClient>(this,&CarWorldClient::print_version);
	m_Executables["exec"] = new ExecCFG(this);
	m_Executables["dump"] = new MethodCall<OFFObject>(&m_Vehicle->Model,&OFFObject::debug_dump);

	bind(SDL_SCANCODE_TAB,"toggleconsole");
	bind(SDL_SCANCODE_F2, "next_camera");
	bind(SDL_SCANCODE_F3, "reset");
	bind(SDL_SCANCODE_F4, "set use_joystick 1");
	bind(SDL_SCANCODE_F5, "set use_joystick 0");

	execute_cfg(ConfigurationFileName());
}

CarWorldClient::~CarWorldClient()
{
	//cout.rdbuf(&hbuf);
	//state must be saved while graphics variables are still valid...
	ofstream cfg_file(ConfigurationFileName(), ios::out);
	write_cfg(cfg_file);

	//cout.rdbuf(&hbuf);
#if CARWORLD_ENABLE_NETWORKING
	CloseNetworkConnection(true);
#endif
	delete RealJoystick;
	delete FakeJoystick;
	for (map<string,HExecutable *>::iterator I = m_Executables.begin(); I != m_Executables.end() ; I++)
		delete (*I).second;
	delete m_CarWorld;

	m_Hgl->MakeCurrent();
	delete m_Hgl;
	m_Hgl = NULL;

	cout << name() << " terminated.\n";
}

const char *CarWorldClient::name()
{
	/*int majorv = CARWORLD_VERSION/100000;
	int minorv = (CARWORLD_VERSION%100000)/100;
	int minorminorv = CARWORLD_VERSION%100;
	static char CarWorldName[1024];
	sprintf_s(CarWorldName,ARRAY_SIZE(CarWorldName),"Car World v%d.%.3d.%.2d",majorv,minorv,minorminorv);
	return CarWorldName;*/
	return CW_PRODUCT_NAME " " CW_VERSION_STR;
}

void CarWorldClient::toggleconsole()
{
	IsPromptMode = !IsPromptMode;
}

void CarWorldClient::execute_cfg(const char *FileName)
{
	cout << "executing " << FileName << "...\n";
	string buffer;
	ifstream in(FileName, ios::in);
	if (!in)
	{
		cout << "could not open \"" << FileName << "\"\n";
	}
	else while (ReadTextLine(in, buffer))
	{
		pars_command(buffer.c_str());
	}
}

void CarWorldClient::pars_command(const char *value)
{
	Command ACommand(value);
	try
	{
		//if nothing do nothing
		if (ACommand.size() <= 0) return;
		if (m_Executables.find(ACommand[0].c_str())==m_Executables.end())
			cout << "\"" << ACommand[0].c_str() << "\" : is not a valid executable\n";
		else
		{
			m_Executables[ACommand[0].c_str()]->exec(ACommand);
		}
	}
	catch (const exception &E)
	{
		cout << E.what() << endl;
	}
}

void CarWorldClient::bind(SDL_Scancode key,const char *command)
{
	KeyBindings[key] = command;
}

void CarWorldClient::print_help()
{
	cout << help_text;
}

void CarWorldClient::print_version()
{
	cout << name()  << endl;
	Hgl::PrintVersion(cout);
	Hgl::PrintExtentions(cout);
	PrintAllScreenModes(cout);
}

#if CARWORLD_ENABLE_NETWORKING
void CarWorldClient::CloseNetworkConnection(bool notify_server)
{
	if (notify_server && m_socket!=NULL && m_serverAddress!=NULL && ID>=0)
	{
		ClientDisconnect disconnect = {};
		disconnect.ClientNumber = ID;
		disconnect.DatagramType = CLIENT_DISCONNECT;
		NET_SendDatagram(
			m_socket,
			m_serverAddress,
			m_serverPort,
			&disconnect,
			static_cast<int>(sizeof(disconnect))
		);
	}

	if (m_socket!=NULL)
	{
		NET_DestroyDatagramSocket(m_socket);
		m_socket = NULL;
	}
	if (m_serverAddress!=NULL)
	{
		NET_UnrefAddress(m_serverAddress);
		m_serverAddress = NULL;
	}
	m_serverPort = 0;
	ID = -1;
}

bool CarWorldClient::SendPacket(const void *data, int size)
{
	if (m_socket==NULL || m_serverAddress==NULL)
		return false;
	if (!NET_SendDatagram(m_socket, m_serverAddress, m_serverPort, data, size))
	{
		cout << "network send failed: " << SDL_GetError() << "\n";
		CloseNetworkConnection(false);
		return false;
	}
	return true;
}

void CarWorldClient::join(const char *host, Uint16 port)
{
	const Sint32 ConnectionTimeout = 5000;

	CloseNetworkConnection(true);
	cout << "trying to connect to: " << host << ":" << port << "...\n";

	m_serverAddress = NET_ResolveHostname(host);
	if (m_serverAddress==NULL)
	{
		cout << "could not resolve server: " << SDL_GetError() << "\n";
		return;
	}
	NET_Status resolution = NET_WaitUntilResolved(
		m_serverAddress,
		ConnectionTimeout
	);
	if (resolution!=NET_SUCCESS)
	{
		cout << (resolution==NET_WAITING
			? "server name resolution timed out.\n"
			: string("could not resolve server: ")+SDL_GetError()+"\n");
		CloseNetworkConnection(false);
		return;
	}

	m_socket = NET_CreateDatagramSocket(NULL, 0, 0);
	if (m_socket==NULL)
	{
		cout << "could not create client socket: " << SDL_GetError() << "\n";
		CloseNetworkConnection(false);
		return;
	}
	m_serverPort = port;

	ClientRequest request = {};
	request.ClientNumber = 0;
	request.DatagramType = CLIENT_CONNECT;
	request.VersionNumber = CW_VERSION;
	if (!SendPacket(&request, static_cast<int>(sizeof(request))))
	{
		CloseNetworkConnection(false);
		return;
	}

	void *sockets[] = {m_socket};
	int ready = NET_WaitUntilInputAvailable(sockets, 1, ConnectionTimeout);
	if (ready<=0)
	{
		cout << (ready==0
			? "connection timed out waiting for the server.\n"
			: string("network receive failed: ")+SDL_GetError()+"\n");
		CloseNetworkConnection(false);
		return;
	}

	NET_Datagram *packet = NULL;
	if (!NET_ReceiveDatagram(m_socket, &packet) || packet==NULL)
	{
		cout << "could not receive server confirmation: "
			<< SDL_GetError() << "\n";
		CloseNetworkConnection(false);
		return;
	}

	ServerConfirm confirm = {};
	bool valid =
		packet->buflen==static_cast<int>(sizeof(confirm)) &&
		packet->port==m_serverPort &&
		NET_CompareAddresses(packet->addr, m_serverAddress)==0;
	if (valid)
	{
		memcpy(&confirm, packet->buf, sizeof(confirm));
		valid =
			confirm.DatagramType==SERVER_CONFIRM &&
			confirm.ClientNumber>=0 &&
			confirm.N>=1 &&
			confirm.N<=MAX_VEHICLES;
	}
	NET_DestroyDatagram(packet);

	if (!valid)
	{
		cout << "server returned an invalid confirmation packet.\n";
		CloseNetworkConnection(false);
		return;
	}

	ID = confirm.ClientNumber;
	m_Opponents.clear();
	m_Opponents[ID] = m_Vehicle;
	for (int i = 0; i<confirm.N; i++)
	{
		int new_id = confirm.ClientNumbers[i];
		if (new_id != ID)
		{
			m_Opponents[new_id] = new CWVehicle(DEFAULT_VEHICLE);
			m_CarWorld->add(m_Opponents[new_id]);
			m_Opponents[new_id]->draw_init();
		}
	}
	cout << "connected as client " << ID << ".\n";
}
#endif

void CarWorldClient::write_cfg(ostream &out)
{
	for (map<string,HExecutable*>::iterator I = m_Executables.begin() ; I!=m_Executables.end() ; I++)
		(*I).second->serialize(out);
}

void CarWorldClient::set_r_mode(int mode)
{
	SetScreenMode(mode,cout);
}

int CarWorldClient::get_r_mode()
{
	//echo not supported yet...
	return 0;
}

void CarWorldClient::set_joystick(bool use_joystick)
{
	if (use_joystick)
	{
		CurrentJoystick = FakeJoystick;
		delete RealJoystick;
		RealJoystick = m_window->GetJoystick();
		if ((RealJoystick!=NULL) && RealJoystick->IsValid())
			CurrentJoystick = RealJoystick;
	}
	else
	{
		if (RealJoystick!=NULL)
			RealJoystick->SetForceFeedback(0,0);
		CurrentJoystick = FakeJoystick;
	}
	cout << "now using: \"" << CurrentJoystick->GetDescription() << "\" for input\n";
}

bool CarWorldClient::get_joystick()
{
	return (CurrentJoystick==RealJoystick);
}

void CarWorldClient::set_active(bool active)
{
	if (RealJoystick!=NULL)
		RealJoystick->SetAcquire(active);
}

void CarWorldClient::key_down(SDL_Scancode AHKey, SDL_Keycode c)
{
	//cout.rdbuf(&hbuf);
	if (IsPromptMode)
	{
		string ReturnedCommand(hbuf.HitKey(AHKey,c));
		if (!ReturnedCommand.empty())
			pars_command(ReturnedCommand.c_str());
	}
	map<SDL_Scancode,string>::iterator I = KeyBindings.find(AHKey);
	if (I != KeyBindings.end())
		pars_command((*I).second.c_str());
	//else
	//	cout << "\"" << KeyMap.find(AHKey) << "\" key unbound\n";
}

void CarWorldClient::text_input(const char* text)
{
	if (IsPromptMode)
	{
		hbuf.textInput(text);
	}
}

void CarWorldClient::resize(unsigned int width, unsigned int weight)
{
	//cout.rdbuf(&hbuf);
	m_Hgl->MakeCurrent();
	HRect ClientRect = HRect(0,0,(REAL)width,(REAL)weight);
	Hgl::ResizeWindow(ClientRect);
}

#if CARWORLD_ENABLE_NETWORKING
void CarWorldClient::SendState()
{
	ClientGamestate state = {};
	state.ClientNumber = ID;
	state.DatagramType = CLIENT_GAMESTATE;
	state.vehicle = m_Vehicle->GetState();
	SendPacket(&state, static_cast<int>(sizeof(state)));
}

bool CarWorldClient::ReceiveState()
{
	NET_Datagram *packet = NULL;
	if (!NET_ReceiveDatagram(m_socket, &packet))
	{
		cout << "network receive failed: " << SDL_GetError() << "\n";
		CloseNetworkConnection(false);
		return false;
	}
	if (packet==NULL)
		return false;

	ServerGamestate state = {};
	bool valid =
		packet->buflen==static_cast<int>(sizeof(state)) &&
		packet->port==m_serverPort &&
		NET_CompareAddresses(packet->addr, m_serverAddress)==0;
	if (valid)
	{
		memcpy(&state, packet->buf, sizeof(state));
		valid =
			state.DatagramType==SERVER_GAMESTATE &&
			state.N>=0 &&
			state.N<=MAX_VEHICLES;
	}
	NET_DestroyDatagram(packet);
	if (!valid)
		return false;

//update the states of the opponents
	for (int i=0 ; i<state.N ; i++)
	{
		int new_id = state.ClientNumbers[i];
		if (new_id!=ID)
		{
			map<int,CWVehicle*>::iterator I = m_Opponents.find(new_id);
			if (I==m_Opponents.end()) //the client is not in the list yet
			{
			//add the new vehicle
				m_Opponents[new_id] = new CWVehicle(DEFAULT_VEHICLE);
				m_CarWorld->add(m_Opponents[new_id]);
				m_Opponents[new_id]->draw_init();
			}
			m_Opponents[new_id]->SetState(state.vehicle[i]);
		}
	}
	return true;
}
#endif

//update the controles of a vehicle with the current position of the joystick
static void UpdateCommand(CWCommand *Command, HJoystick *Joystick)
{
	Joystick->UpdateState();
	Command->Steer = Joystick->GetAxisPos(0);
	Command->GasBrake = Joystick->GetAxisPos(1);
	Command->HandBrake = Joystick->GetButtonPos(0);
	Joystick->SetForceFeedback(Command->SteerFeedBack,-Command->GasBrake);
}

void CarWorldClient::on_idle(unsigned int elapsed_time)
{
	if (m_window != NULL)
	{
		//cout.rdbuf(&hbuf);
#if CARWORLD_ENABLE_NETWORKING
		if (m_socket!=NULL && ID>=0)
		{
			static unsigned int time_since_send = 0;
			if (ReceiveState())
			{
				SendState();
				time_since_send = 0;
			}
			else
				time_since_send += elapsed_time;
			if (time_since_send>CLIENT_TIMEOUT)
			{
				SendState();
				time_since_send = 0;
			}
		}
#endif
		UpdateCommand(&m_Vehicle->MyCommand, CurrentJoystick);
		m_CarWorld->update(elapsed_time);
		draw();
	}
}

void CarWorldClient::draw()
{
	if (m_window != NULL)
	{
		//cout.rdbuf(&hbuf);
		m_Hgl->MakeCurrent();
		m_CarWorld->draw();
		if (IsPromptMode)
			hbuf.draw();
		else
			m_CarWorld->DrawOnScreen();
		Hgl::Finish();
		Hgl::ThrowError();
		Hgl::SwapBuffers();
	}
}
