
#include "H_Standard.h"
#include "CarWorldNet.h"
#include "CarWorldServer.h"

CarWorldServer::CarWorldServer(short port) :
	m_CarWorld(new CarWorld(TimeRefreshRate(),DEFAULT_LANDSCAPE)),
	m_Socket(SDLNet_UDP_Open(port))
{

}

CarWorldServer::~CarWorldServer()
{
	delete m_CarWorld;
	SDLNet_UDP_Close(m_Socket);
}

const char *CarWorldServer::name()
{
	return "Car World server";
}

bool CarWorldServer::AddClient(int id, ClientRequest *request)
{
	m_Clients[id] = new CWVehicle(DEFAULT_VEHICLE);
	m_CarWorld->add(m_Clients[id]);
	return true;
}

bool CarWorldServer::UpdateClient(int id, ClientGamestate *state)
{
	m_Clients[id]->SetState(state->vehicle);
	return true;
}

bool CarWorldServer::RemoveClient(int id, ClientDisconnect *discon)
{
//BUG: not implemented
	return true;
}

//extern "C" void usleep(int);

void CarWorldServer::on_idle(unsigned int elapsed_time)
{
	UDPpacket* packet = SDLNet_AllocPacket(MAX_DATAGRAM_SIZE);
	char* buffer = (char*)packet->data;
	if (SDLNet_UDP_Recv(m_Socket, packet)>0)
	{
		int ClientID = packet->channel;
		int RequestType = *(((int*)buffer)+1);
		switch (RequestType)
		{
		//should check the size of the datagram in each case
			case CLIENT_CONNECT:
			{
			//add client
				cout << "client(" << ClientID << ") connect request...\n";
				AddClient(ClientID,(ClientRequest*)buffer);
			//send confirmation
				SendConfirmation(ClientID);
				break;
			}
			case CLIENT_GAMESTATE:
			{
			//update this vehicle's state
				//cout << "client(" << ClientID << ") gamestate recieved...\n";
				UpdateClient(ClientID, (ClientGamestate*)buffer);
			//send this client the state of the server
				SendGamestate(ClientID);
				break;
			}
			case CLIENT_DISCONNECT:
			{
			//remove this client from the list
				cout << "client(" << ClientID << ") disconnect request...\n";
				SendDisconnect(ClientID);
				RemoveClient(ClientID, (ClientDisconnect*)buffer);
				cout << "exiting...\n";
				exit(0);
				break;
			}
			default:
				cout << "recieved unknown request.\n";
		}
	}
	m_CarWorld->update(elapsed_time);
	//usleep(1000);
}

void CarWorldServer::GetClientList(int ClientNumbers[MAX_VEHICLES])
{
	int i=0;
	for (map<int,CWVehicle*>::iterator I=m_Clients.begin() ; I!=m_Clients.end() ; I++)
	{
		ClientNumbers[i] = I->first;
		i++;
	}
}

void CarWorldServer::GetState(CWVehicleState vehicle[MAX_VEHICLES])
{
	int i=0;
	for (map<int,CWVehicle*>::iterator I=m_Clients.begin() ; I!=m_Clients.end() ; I++)
	{
		vehicle[i] = I->second->GetState();
		i++;
	}
}

void CarWorldServer::SendConfirmation(int id)
{
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(ServerConfirm));
	ServerConfirm* confirm = (ServerConfirm*)packet->data;
	confirm->DatagramType = SERVER_CONFIRM;
	confirm->ClientNumber = id;
	//confirm.VersionNumber = CARWORLD_VERSION;
	confirm->N = int(m_Clients.size());
	GetClientList(confirm->ClientNumbers);
	SDLNet_UDP_Send(m_Socket, id, packet);
	SDLNet_FreePacket(packet);
}


void CarWorldServer::SendGamestate(int id)
{
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(ServerGamestate));
	ServerGamestate* state = (ServerGamestate*)packet->data;
	state->DatagramType = SERVER_GAMESTATE;
	state->N = int(m_Clients.size());
	GetClientList(state->ClientNumbers);
	GetState(state->vehicle);
	SDLNet_UDP_Send(m_Socket, id, packet);
	SDLNet_FreePacket(packet);
}


void CarWorldServer::SendDisconnect(int id)
{
//BUG: not implemented
}

