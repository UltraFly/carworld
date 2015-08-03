
#ifndef _CAR_WORLD_SERVER_H_
#define _CAR_WORLD_SERVER_H_

#include "H_Main.h"
#include "SDL_net.h"
#include "CarWorldNet.h"
#include "CarWorld.h"

class CarWorldServer : public HApplication
{
public:
	CarWorldServer(short port);
	virtual ~CarWorldServer();
//overrided inherited methods
	const char *name();

	void on_idle(unsigned int elapsed_time);

//helper methods:
	void GetClientList(int ClientNumbers[MAX_VEHICLES]);
	void GetState(CWVehicleState vehicle[MAX_VEHICLES]);

//actions on the server
	bool AddClient(int id, ClientRequest *request);
	bool UpdateClient(int id, ClientGamestate *state);
	bool RemoveClient(int id, ClientDisconnect *discon);

//datagram sendin methods:
	void SendConfirmation(int id);
	void SendGamestate(int id);
	void SendDisconnect(int id);
private:
	CarWorld *m_CarWorld;
	map<int,CWVehicle*> m_Clients;
	UDPsocket m_Socket;
};

#endif //_CAR_WORLD_SERVER_H_
