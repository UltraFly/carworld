
#ifdef WIN32
#pragma warning( disable : 4786 ) //disable "identifier name too long" warning
#endif

#include "CarWorldClient.h"
#include "CarWorldServer.h"
#include "CarWorldNet.h"

#define USAGE "bad usage!\n"

int find(int argc, char **argv, char *v)
{
	int i=0;
	while (i<argc && strcmp(argv[i],v)) i++;
	return i;
}

int main(int argc, char **argv)
{
	try
	{
		cerr = herr;
		if (find(argc,argv,"-server")!=argc)
		{
			short port = (argc==3) ? (short)atoi(argv[2]) : DEFAULT_PORT;
			Add(new CarWorldServer(port));
		}
		else
		{
			bool full_screen = find(argc,argv,"-f")!=argc;
			Add(new CarWorldClient(full_screen));
		}
		return EventLoop(argc, argv);
	}
	catch (const exception &E)
	{
		cerr << E.what() << endl;
		HErrorExit(E.what());
	}
	return 1;
}

