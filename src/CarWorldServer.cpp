#include "H_Standard.h"
#include "CarWorldNet.h"
#include "CarWorldServer.h"

#include "CWVersion.h"

#include <cstring>

CarWorldServer::CarWorldServer(Uint16 port) :
	m_CarWorld(new CarWorld(TimeRefreshRate(),DEFAULT_LANDSCAPE)),
	m_Socket(NULL),
	m_Port(port)
{
	if (!ReopenSocket())
	{
		delete m_CarWorld;
		m_CarWorld = NULL;
		throw HException(string("could not create server socket: ")+SDL_GetError());
	}
	cout << "server listening on UDP port " << port << ".\n";
}

CarWorldServer::~CarWorldServer()
{
	for (map<int,ClientConnection>::iterator I=m_Clients.begin();
		I!=m_Clients.end(); I++)
	{
		NET_UnrefAddress(I->second.address);
	}
	m_Clients.clear();
	if (m_Socket!=NULL)
		NET_DestroyDatagramSocket(m_Socket);
	delete m_CarWorld;
}

const char *CarWorldServer::name()
{
	return "Car World server";
}

bool CarWorldServer::ReopenSocket()
{
	if (m_Socket!=NULL)
		NET_DestroyDatagramSocket(m_Socket);
	m_Socket = NET_CreateDatagramSocket(NULL, m_Port, 0);
	return m_Socket!=NULL;
}

int CarWorldServer::AllocateClientID() const
{
	for (int id=0; id<MAX_VEHICLES; id++)
	{
		if (m_Clients.find(id)==m_Clients.end())
			return id;
	}
	return -1;
}

int CarWorldServer::FindClient(NET_Address *address, Uint16 port) const
{
	for (map<int,ClientConnection>::const_iterator I=m_Clients.begin();
		I!=m_Clients.end(); I++)
	{
		if (I->second.port==port &&
			NET_CompareAddresses(I->second.address, address)==0)
			return I->first;
	}
	return -1;
}

bool CarWorldServer::IsClientEndpoint(
	int id,
	NET_Address *address,
	Uint16 port
) const
{
	map<int,ClientConnection>::const_iterator I=m_Clients.find(id);
	return I!=m_Clients.end() &&
		I->second.port==port &&
		NET_CompareAddresses(I->second.address, address)==0;
}

bool CarWorldServer::AddClient(
	int id,
	const ClientRequest &request,
	NET_Address *address,
	Uint16 port
)
{
	if (id<0 || id>=MAX_VEHICLES || request.VersionNumber!=CW_VERSION)
		return false;

	ClientConnection connection = {};
	connection.vehicle = new CWVehicle(DEFAULT_VEHICLE);
	connection.address = NET_RefAddress(address);
	connection.port = port;
	m_Clients[id] = connection;
	m_CarWorld->add(static_cast<CWFeature*>(connection.vehicle));
	return true;
}

bool CarWorldServer::UpdateClient(int id, ClientGamestate &state)
{
	map<int,ClientConnection>::iterator I=m_Clients.find(id);
	if (I==m_Clients.end())
		return false;
	I->second.vehicle->SetState(state.vehicle);
	return true;
}

bool CarWorldServer::RemoveClient(
	int id,
	const ClientDisconnect &disconnect
)
{
	(void)disconnect;
	map<int,ClientConnection>::iterator I=m_Clients.find(id);
	if (I==m_Clients.end())
		return false;

	CWVehicle *vehicle = I->second.vehicle;
	NET_UnrefAddress(I->second.address);
	m_Clients.erase(I);
	m_CarWorld->remove(vehicle);
	return true;
}

void CarWorldServer::on_idle(unsigned int elapsed_time)
{
	while (m_Socket!=NULL)
	{
		NET_Datagram *packet = NULL;
		if (!NET_ReceiveDatagram(m_Socket, &packet))
		{
			cout << "server receive failed: " << SDL_GetError() << "\n";
			if (!ReopenSocket())
				cout << "could not reopen server socket: "
					<< SDL_GetError() << "\n";
			break;
		}
		if (packet==NULL)
			break;

		int header[2] = {};
		if (packet->buflen>=static_cast<int>(sizeof(header)))
			memcpy(header, packet->buf, sizeof(header));
		int client_id = header[0];
		int request_type = header[1];

		switch (request_type)
		{
			case CLIENT_CONNECT:
			{
				if (packet->buflen!=static_cast<int>(sizeof(ClientRequest)))
					break;
				ClientRequest request = {};
				memcpy(&request, packet->buf, sizeof(request));
				if (request.VersionNumber!=CW_VERSION)
				{
					cout << "rejected client with incompatible version.\n";
					break;
				}

				client_id = FindClient(packet->addr, packet->port);
				if (client_id<0)
				{
					client_id = AllocateClientID();
					if (!AddClient(
						client_id,
						request,
						packet->addr,
						packet->port
					))
					{
						cout << "rejected client connection request.\n";
						break;
					}
					cout << "client(" << client_id << ") connected.\n";
				}
				SendConfirmation(client_id);
				break;
			}
			case CLIENT_GAMESTATE:
			{
				if (packet->buflen!=static_cast<int>(sizeof(ClientGamestate)) ||
					!IsClientEndpoint(client_id, packet->addr, packet->port))
					break;
				ClientGamestate state = {};
				memcpy(&state, packet->buf, sizeof(state));
				if (UpdateClient(client_id, state))
					SendGamestate(client_id);
				break;
			}
			case CLIENT_DISCONNECT:
			{
				if (packet->buflen!=static_cast<int>(sizeof(ClientDisconnect)) ||
					!IsClientEndpoint(client_id, packet->addr, packet->port))
					break;
				ClientDisconnect disconnect = {};
				memcpy(&disconnect, packet->buf, sizeof(disconnect));
				cout << "client(" << client_id << ") disconnected.\n";
				RemoveClient(client_id, disconnect);
				break;
			}
			default:
				cout << "received unknown request.\n";
		}

		NET_DestroyDatagram(packet);
	}
	m_CarWorld->update(elapsed_time);
}

void CarWorldServer::GetClientList(int ClientNumbers[MAX_VEHICLES])
{
	int i=0;
	for (map<int,ClientConnection>::iterator I=m_Clients.begin();
		I!=m_Clients.end() && i<MAX_VEHICLES; I++)
	{
		ClientNumbers[i] = I->first;
		i++;
	}
}

void CarWorldServer::GetState(CWVehicleState vehicle[MAX_VEHICLES])
{
	int i=0;
	for (map<int,ClientConnection>::iterator I=m_Clients.begin();
		I!=m_Clients.end() && i<MAX_VEHICLES; I++)
	{
		vehicle[i] = I->second.vehicle->GetState();
		i++;
	}
}

bool CarWorldServer::SendPacket(int id, const void *data, int size)
{
	map<int,ClientConnection>::iterator I=m_Clients.find(id);
	if (m_Socket==NULL || I==m_Clients.end())
		return false;
	if (!NET_SendDatagram(
		m_Socket,
		I->second.address,
		I->second.port,
		data,
		size
	))
	{
		cout << "server send failed: " << SDL_GetError() << "\n";
		if (!ReopenSocket())
			cout << "could not reopen server socket: "
				<< SDL_GetError() << "\n";
		return false;
	}
	return true;
}

void CarWorldServer::SendConfirmation(int id)
{
	ServerConfirm confirm = {};
	confirm.DatagramType = SERVER_CONFIRM;
	confirm.ClientNumber = id;
	confirm.N = static_cast<int>(m_Clients.size());
	GetClientList(confirm.ClientNumbers);
	SendPacket(id, &confirm, static_cast<int>(sizeof(confirm)));
}

void CarWorldServer::SendGamestate(int id)
{
	ServerGamestate state = {};
	state.DatagramType = SERVER_GAMESTATE;
	state.N = static_cast<int>(m_Clients.size());
	GetClientList(state.ClientNumbers);
	GetState(state.vehicle);
	SendPacket(id, &state, static_cast<int>(sizeof(state)));
}
