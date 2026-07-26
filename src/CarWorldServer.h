
#ifndef _CAR_WORLD_SERVER_H_
#define _CAR_WORLD_SERVER_H_

#include "H_Main.h"
#include <SDL3_net/SDL_net.h>
#include "CarWorldNet.h"
#include "CarWorld.h"

class CarWorldServer : public HApplication
{
public:
	CarWorldServer(Uint16 port);
	virtual ~CarWorldServer();
//overrided inherited methods
	const char *name();

	void on_idle(unsigned int elapsed_time);

//helper methods:
	void GetClientList(int ClientNumbers[MAX_VEHICLES]);
	void GetState(CWVehicleState vehicle[MAX_VEHICLES]);

//actions on the server
	bool AddClient(
		int id,
		const ClientRequest &request,
		NET_Address *address,
		Uint16 port
	);
	bool UpdateClient(int id, ClientGamestate &state);
	bool RemoveClient(int id, const ClientDisconnect &disconnect);

	//datagram sendin methods:
	void SendConfirmation(int id);
	void SendGamestate(int id);
private:
	struct ClientConnection
	{
		CWVehicle *vehicle;
		NET_Address *address;
		Uint16 port;
	};

	int AllocateClientID() const;
	int FindClient(NET_Address *address, Uint16 port) const;
	bool IsClientEndpoint(int id, NET_Address *address, Uint16 port) const;
	bool ReopenSocket();
	bool SendPacket(int id, const void *data, int size);

	CarWorld *m_CarWorld;
	map<int,ClientConnection> m_Clients;
	NET_DatagramSocket *m_Socket;
	Uint16 m_Port;
};

#endif //_CAR_WORLD_SERVER_H_
