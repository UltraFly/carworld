
#ifndef USE_POSIX_SOCKETS

#pragma warning( disable : 4786 ) //disable "identifier name too long" warning

extern "C" int errno;
#include <winsock.h>
#include <io.h>
#include <iostream>

using namespace std;

#define DATA_T char
#define SOCKLEN_T int

#include "H_Socket.h"
#include "H_Standard.h"

//UTILS
void write_error(const char *Where, bool bad=true)
{
	if (bad)
	{
		char ErrorMessage[256];
		errno_t err = strerror_s(ErrorMessage, ARRAY_SIZE(ErrorMessage), errno);

		cout << "ERROR(" << __FILE__ << "," << Where << "): " << ErrorMessage << endl;
	}
}

bool InitWinSock()
{
	static bool IsInit = false;
	if (!IsInit)
	{
		WSADATA WSAData;
		int ret = WSAStartup(MAKEWORD(1,1), &WSAData);
		if (ret==0)
		{
			IsInit = true;
			return true;
		}
	}
	write_error("InitWinSock() failed.", true);
	return false;
}

//CLASS HSocServer
HSocServer::HSocServer(short portno)
{
	InitWinSock();

//open socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	write_error("socket creation",s==INVALID_SOCKET);

//set non blocking
	unsigned long nonzero = 1;
	int ret = ioctlsocket(s, FIONBIO, (unsigned long*)&nonzero);
	write_error("set nonblocking",ret!=0);

//bind
	sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(portno);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(s, (sockaddr*)&name, sizeof(name));
	write_error("bind",ret!=0);
	cout << "waiting on port: " << portno << endl;
}

HSocServer::~HSocServer()
{
	closesocket(s);
	WSACleanup();
}

#include <map>
using std::map;

static int NextClientID = 1;
static map<int,sockaddr> ClientAdresses;
bool operator==(const sockaddr &A, const sockaddr &B)
{
	return memcmp((void*)&A,(void*)&B,sizeof(sockaddr))==0;
}

int HSocServer::Recieve(void* buffer, int size, int *id)
{
//read incomming
	sockaddr from;
	SOCKLEN_T fromlen = sizeof(from);
	int recieved_bytes = recvfrom(s,  (DATA_T*)buffer, size, 0, &from, (SOCKLEN_T*)&fromlen);
	//write_error("recieveing bytes",recieved_bytes!=-1);
	if (recieved_bytes>0)
	{
		*id = *((int*)buffer);
		if (*id==0)
		{
		//the client does not have an ID yet, give him one
			*id = NextClientID++;
			ClientAdresses[*id] = from;
		}
		else
		{
		//we know about this client, check his ID
			/*map<int,sockaddr>::iterator I = ClientAdresses.find(*id);
			if ((I==ClientAdresses.end()) || !(I->second==from))
			{
				cout << "bad ID/adress pair: ID=" << (*id) << endl;
				return -1;
			}*/
		}
		return recieved_bytes;
	}
	return -1;
}

bool HSocServer::Send(void* buffer, int size, int id)
{
	map<int,sockaddr>::iterator I = ClientAdresses.find(id);
	if (I==ClientAdresses.end())
	{
		cout << "bad ID: ID=" << id << endl;
		return false;
	}
	int sent_bytes = sendto(s,  (DATA_T*)buffer, size, 0, &I->second, sizeof(I->second));
	return (sent_bytes==size);
}

bool HSocServer::Broadcast(void* buffer, int size)
{
	int sent_bytes = 0;
	map<int,sockaddr>::iterator I;
	for (I = ClientAdresses.begin() ; I!=ClientAdresses.end() ; I++)
	{
		sent_bytes += sendto(s, (DATA_T*)buffer, size, 0, &I->second, sizeof(I->second));
	}
	return sent_bytes==ClientAdresses.size()*size;
}

//CALSS HSocClient
HSocClient::HSocClient(const char *servername, short portno)
{
	InitWinSock();

	int ret;

//open socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	write_error("socket creation",s==INVALID_SOCKET);

//get the server adress
	hostent *mmserv = gethostbyname(servername);
	write_error("gethostbyname",mmserv==0);
	sockaddr_in name;
	memcpy((char*)(&(name.sin_addr.s_addr)), mmserv->h_addr, mmserv->h_length);
	name.sin_family = AF_INET;
	name.sin_port = htons(portno);

//connect
	ret = connect(s, (sockaddr*)&name, sizeof(name));
	write_error("connect",ret!=0);
}

HSocClient::~HSocClient()
{
	closesocket(s);
	WSACleanup();
}

void HSocClient::SetBlocking(bool IsBlocking)
{
	if (IsBlocking)
	{
		int ret = ioctlsocket(s, FIONBIO, 0);
		write_error("set blocking",ret!=0);
	}
	else
	{
		unsigned long nonzero = 1;
		int ret = ioctlsocket(s, FIONBIO, (unsigned long*)&nonzero);
		write_error("set nonblocking",ret!=0);
	}
}

bool HSocClient::Send(void* buffer, int size)
{
	int sent_bytes = send(s, (DATA_T*)buffer, size, 0);
	write_error("sending bytes",sent_bytes!=size);
	return sent_bytes==size;
}


int HSocClient::Recieve(void* buffer, int size)
{
	int recv_bytes = recv(s, (DATA_T*)buffer, size, 0);
	if (recv_bytes>0)
	{
	}
	else
	{
		write_error("recieving bytes");
		return false;
	}
	return recv_bytes;
}

#endif //USE_POSIX_SOCKETS

