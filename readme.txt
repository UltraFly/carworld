
AUTHOR(S)
Marcus Hewat	hewat@users.sourceforge.net

DESCRIPTION
carworld is a 3d networked driving simulator

HISTORY
*version 0.245:
	fixed "truck is dark" bug
	fixed "textures are white on first launch" bug
	fixed "crash on exit" bug
*version 0.244:
	use SDL_image for loading images
	use PNG instead of PPM for image format
	use Visual Studio 2008 (9.0)
	use SDL opengl extention querying and remove most #ifdef WIN32
*version v0.243: (28/06/06)
	compiles with gcc 4.1
	no more warnings compiling with Visual Studio .Net 2005 (8.0)
*version v0.241: (09/06/06)
	compiles with gcc 4.0.4
	compiles with Visual Studio .Net 2005 (8.0)
	fixed: config file contains nul characters in Windows version
*version v0.239: (07/04/04)
	ported to SDL, removed most platform specific code (except network code & Direct Input)
	compiles with gcc 3.3.1
	compiles with Microsoft Visual C++ 6.0
	known bug: cfg file does not work under Linux
	known bug: server mode does not work
*version v0.223: (20/09/00) Networked mode now works under windows
*version v0.222: (19/07/00) truck straightend out
	rpm package target added to makefile
*version v0.219: (15/06/00) DirectX force feedback support, removed dependency to GLU
	auto restart added
	fixed: "start out of balance"
*version v0.215: (19/04/00) slightly better networking, first version to compile under beos
	applied patches sent in.
*version v0.215: slightly better network support, first BeOS port, applied patches sent in 
*version v0.205: preliminary support for networked gaming (under UNIX/Linux only for now) 
*version v0.198: bug fixes, support for linux joystick etc... 
*version v0.190: first release

WEB
find carworld on the web at:
http://carworld.sourceforge.net

LICENCE
This software is under the GPL (General Public Licence), that is, you can use and modify if for
free but must make the source of your modifications available under the same
licence.

CONTRIBUTIONS
Advice, 3DS files for car/landscape models, new developpement, pathces,
binaries for exotic hardware, functionality/performance reports etc... will be
greatly appreciated. send them to me (hewat@users.sourceforge.net)

REQUIERMENTS
the SDL librairy http://www.libsdl.org
the SDL_image librairy

RUN

(if you used winzip to extract, you will need to make shure the "smart CR/LF conversion"
is turned off or the data will be corrupt)

IMPORTANT NOTE: you must in the root directory of the carworld distribution to run carworld

COMPILE
compiles with various CC g++ and cl.exe (Microsoft's VC++ compiler),
type 'make' or 'gmake' if this does not work to compile, this is assuming you also have 'make'
installed on your system (on windows I use make for cygwin)
if this does not work, send me a patch so that it does.


USING
once you have launched the game try using keys F2 to F5 to change options
arrow keys control the vehicle, space bar is the hand-break
TAB gives you access to a Quake-like console.
it is recomended to edit the "data/config.cfg" file to suit your computer


NETWORKED GAME
start a separat carworld with the "-server" option to start a server
type "join <servername>" at the prompt (TAB key) of a running game to connect to a server
(attention: disconnection from the server kills it right now)
note: you might have to press F2 a few times to have a view point from inside _YOUR_OWN_ vehicle...

BUGS
*untested networked Windows version (probably does not work/crashes)
*some unhandled error returns
*problem with BeOS version


TODO
-don't hardcode path to road texture
-use SDL for networked game
-sound (with openal)
-clean up file formats (use 3ds?)+make a better terrain editor (with Qt)...
