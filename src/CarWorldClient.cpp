
#include "H_Standard.h"
#include "CarWorldClient.h"
#include "CarWorldNet.h"
#include "CWVersion.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "H_Variable.h"

#define CLIENT_TIMEOUT 200


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
		if ((c.size()!=2) && (c.size()!=3))
			cout << "usage: join <server name> [<port>]\n";
		else
		{
			short port = (c.size()==3) ? (short)atoi(c[2].c_str()) : DEFAULT_PORT;
			CWC->join(c[1].c_str(),port);
		}
	}
	virtual ~JoinServer() {}
private:
	CarWorldClient *CWC;
};

class BindKey : public HExecutable
{
public:
	BindKey(CarWorldClient *CWC) : CWC(CWC) {}
	void exec(const Command &c)
	{
		if (c.size()==3)
			CWC->bind(ToSDLKey(c[1].c_str()), c[2].c_str());
		else
			cout << "usage: bind <key> \"<command line>\"\n";
	}
	void serialize(ostream &out)
	{
		map<SDLKey,string>::iterator I;
		for (I = CWC->KeyBindings.begin() ; I!=CWC->KeyBindings.end(); I++)
		{
			out << "bind " << (*I).first << " \"" << (*I).second << "\"\n";
		}
	}
	virtual ~BindKey() {}
private:
	CarWorldClient *CWC;
};

//CLASS CarWorldClient
CarWorldClient::CarWorldClient(bool full_screen) :
	m_Hgl(NULL),
	IsPromptMode(false),
	RealJoystick(NULL),
	FakeJoystick(NULL),
	CurrentJoystick(NULL),
	m_socket(NULL),
	ID(0),
	m_Vehicle(NULL),
	m_CarWorld(NULL)
{
	m_window->SetAttrib(640,480,full_screen);
//switch cout to display in the on screen prompt + log
	cout.rdbuf(&hbuf);
	cout << "starting " << name() << " ...\n\n";
	m_CarWorld = new CarWorld(TimeRefreshRate(),DEFAULT_LANDSCAPE);
	m_Vehicle = new CWVehicle(DEFAULT_VEHICLE);
	m_CarWorld->add(m_Vehicle);
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
	RealJoystick = m_window->GetJoystick();
	if (RealJoystick!=NULL)
		cout << RealJoystick->GetDescription() << endl;
	else
		cout << "no joystick support.\n";
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
	m_HExecutableSet->add(new HVar<SDLKey>("accel",&(FakeJoystick->up_key)));
	m_HExecutableSet->add(new HVar<SDLKey>("break",&(FakeJoystick->down_key)));
	m_HExecutableSet->add(new HVar<SDLKey>("left",&(FakeJoystick->left_key)));
	m_HExecutableSet->add(new HVar<SDLKey>("right",&(FakeJoystick->right_key)));
	m_HExecutableSet->add(new HVar<SDLKey>("handbreak",&(FakeJoystick->button_key)));

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

	bind(SDLK_TAB,"toggleconsole");
	bind(SDLK_F2, "next_camera");
	bind(SDLK_F3, "reset");
	bind(SDLK_F4, "set use_joystick 1");
	bind(SDLK_F5, "set use_joystick 0");

	execute_cfg(ConfigurationFileName());
}

CarWorldClient::~CarWorldClient()
{
	//cout.rdbuf(&hbuf);
	//state must be saved while graphics variables are still valid...
	ofstream cfg_file(ConfigurationFileName(), ios::out);
	write_cfg(cfg_file);

	//cout.rdbuf(&hbuf);
	delete FakeJoystick;
	for (map<string,HExecutable *>::iterator I = m_Executables.begin(); I != m_Executables.end() ; I++)
		delete (*I).second;
	delete m_CarWorld;

	m_Hgl->MakeCurrent();
	delete m_Hgl;
	m_Hgl = NULL;

	if (m_socket!=NULL)
	{
	//disconnect from the server
		ClientDisconnect disc;
		disc.ClientNumber = ID;
		disc.DatagramType = CLIENT_DISCONNECT;
		m_socket->Send((void*)&disc,sizeof(disc));
		delete m_socket;
	}
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
	char buffer[1024];
	ifstream in(FileName, ios::in);
	if (!in)
	{
		cout << "could not open \"" << FileName << "\"\n";
	}
	else while (!in.eof())
	{
		in.getline(buffer,1024);
		pars_command(buffer);
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

void CarWorldClient::bind(SDLKey key,const char *command)
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

void CarWorldClient::join(const char *host, short port)
{
//close previous socket
	delete m_socket;
//open the socket
	cout << "trying to connect to: " << host << ":" << port << "...\n";
	m_socket = new HSocClient(host,port);
//request to join the game
	ClientRequest request;
	request.ClientNumber = 0;
	request.DatagramType = CLIENT_CONNECT;
	request.VersionNumber = CW_VERSION;
	m_socket->Send((void*)&request,sizeof(request));
//get the confirmation
	ServerConfirm confirm;
	m_socket->Recieve((void*)&confirm,sizeof(confirm));
	ID = confirm.ClientNumber;
//initiate the opponents:
	m_Opponents.clear();
	m_Opponents[ID] = m_Vehicle;
	for (int i=0 ; i<confirm.N ; i++)
	{
		int new_id = confirm.ClientNumbers[i];
		if (new_id!=ID)
		{
			m_Opponents[new_id] = new CWVehicle(DEFAULT_VEHICLE);
			m_CarWorld->add(m_Opponents[new_id]);
			m_Opponents[new_id]->draw_init();
		}
	}
//set non blocking socket
	m_socket->SetBlocking(false);
}

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
	if (use_joystick && (RealJoystick!=NULL) && RealJoystick->IsValid())
		CurrentJoystick = RealJoystick;
	else
		CurrentJoystick = FakeJoystick;
	cout << "now using: \"" << CurrentJoystick->GetDescription() << "\" for input\n";
}

bool CarWorldClient::get_joystick()
{
	return (CurrentJoystick==RealJoystick);
}

void CarWorldClient::key_down(SDLKey AHKey, char c)
{
	//cout.rdbuf(&hbuf);
	if (IsPromptMode)
	{
		string ReturnedCommand(hbuf.HitKey(AHKey,c));
		if (!ReturnedCommand.empty())
			pars_command(ReturnedCommand.c_str());
	}
	map<SDLKey,string>::iterator I = KeyBindings.find(AHKey);
	if (I != KeyBindings.end())
		pars_command((*I).second.c_str());
	//else
	//	cout << "\"" << KeyMap.find(AHKey) << "\" key unbound\n";
}

void CarWorldClient::resize(unsigned int width, unsigned int weight)
{
	//cout.rdbuf(&hbuf);
	m_Hgl->MakeCurrent();
	HRect ClientRect = HRect(0,0,(REAL)width,(REAL)weight);
	Hgl::ResizeWindow(ClientRect);
}

void CarWorldClient::SendState()
{
	ClientGamestate state;
	state.ClientNumber = ID;
	state.DatagramType = CLIENT_GAMESTATE;
	state.vehicle = m_Vehicle->GetState();
	m_socket->Send((void*)&state,sizeof(state));
}

bool CarWorldClient::RecieveState()
{
	ServerGamestate state;
	if (m_socket->Recieve((void*)&state,sizeof(state))==sizeof(state))
	{
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
	return false;
}

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
		if (m_socket!=NULL)
		{
			static unsigned int time_since_send = 0;
			if (RecieveState())
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
