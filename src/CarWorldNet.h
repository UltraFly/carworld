
#ifndef _CARWORLD_NET_H_
#define _CARWORLD_NET_H_

#include "CWVehicle.h"

#define DEFAULT_PORT 12000

//the client sends:
#define CLIENT_CONNECT 0
#define CLIENT_GAMESTATE 1
#define CLIENT_DISCONNECT 2

//the server sends:
#define SERVER_CONFIRM 3
#define SERVER_GAMESTATE 4
#define SERVER_DISCONNECT 5

//these structures should be writable in a platform independant way
//(endianness, int size...)
//to be sent to a different OS server

//the client number is taken care of by the HSocket classes
//the rest is "carworld" specific

//CLIENT:
struct ClientRequest
{
	int ClientNumber; //0

	int DatagramType; //CLIENT_CONNECT
	
	int VersionNumber; //CARWORLD_VERSION
//name of the player, type of car...
};

struct ClientGamestate
{
	int ClientNumber; //assigned by the server

	int DatagramType; //CLIENT_GAMESTATE

	CWVehicleState vehicle;
};

struct ClientDisconnect
{
	int ClientNumber; //assigned by the server

	int DatagramType; //CLIENT_DISCONNECT
};

//SERVER:
//must be made dynamic
#define MAX_VEHICLES 8

struct ServerConfirm
{
	int DatagramType; //SERVER_CONFIRM
	int ClientNumber; //number assigned to the client
	int VersionNumber; //CARWORLD_VERSION
	int N;
	int ClientNumbers[MAX_VEHICLES];
//names and types of cars of other players
};

struct ServerGamestate
{
	int DatagramType; //SERVER_GAMESTATE
	int N;
	int ClientNumbers[MAX_VEHICLES];
	CWVehicleState vehicle[MAX_VEHICLES];
};

struct ServerDisconnect
{
	int DatagramType; //SERVER_DISCONNECT
//will probably stay empty
};

#define MAX_DATAGRAM_SIZE sizeof(ServerGamestate)


#endif //_CARWORLD_NET_H_
