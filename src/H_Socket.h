
#ifndef _H_SOCKET_H_
#define _H_SOCKET_H_

void random_wait();

//an unique id is assigned to each client  
class HSocServer
{
public:
	HSocServer(short portno);
	~HSocServer();
	int Recieve(void* buffer, int size, int *id);
	bool Send(void* buffer, int size, int id);
	bool Broadcast(void* buffer, int size);
private:
	int s;
};

class HSocClient
{
public:
	HSocClient(const char *servername, short portno);
	~HSocClient();
	void SetBlocking(bool IsBlocking);
	int Recieve(void* buffer, int size);
	bool Send(void* buffer, int size);
private:
	int s;
};

#endif //_H_SOCKET_H_
