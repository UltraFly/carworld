
#include "../H_Main.h"
#include "BeInternals.h"
#include <assert.h>
#include <iostream>

#define BOGUS_TIME_REFRESH_RATE 300

//CLASS HApplication
HApplication::HApplication() : m_data(NULL)
{}
HApplication::~HApplication()
{}

int HApplication::TimeRefreshRate()
{
	return BOGUS_TIME_REFRESH_RATE;
}

//CLASS HApplication
HglApplication::HglApplication() : m_window(new HBeWindow())
{}

HglApplication::~HglApplication()
{
	delete m_window;
}

unsigned int HglApplication::GetScreenMode()
{
	return 0;
}

bool HglApplication::SetScreenMode(unsigned int ScreenMode, ostream &out)
{
	return false;
}

void HglApplication::PrintScreenMode(unsigned int ScreenMode, ostream &out)
{}

void HglApplication::PrintAllScreenModes(ostream &out)
{}


//FUNCTIONS

#include <sys/utsname.h>
#include <stdio.h>
const char *SystemDescription()
{
	static char desc[sizeof(utsname)+128];
	utsname uts;
	uname(&uts);
	sprintf(desc,"system name: %s\nnode name: %s\nrelease: %s\nversion: %s\nmachine: %s\nsizeof(int): %d",
		uts.sysname,
		uts.nodename,
		uts.release,
		uts.version,
		uts. machine,
		(int)sizeof(int));
	return desc;
}

HglApplication *m_app = NULL;

void Add(HApplication *app)
{
	assert(m_app==NULL);
	m_app = dynamic_cast<HglApplication*>(app);
	assert(m_app!=NULL);
}

int EventLoop(int argc, char **argv)
{
	assert(m_app!=NULL);

	BApplication app("application/x-vnd.Be-carw");

	HBeWindow *w = dynamic_cast<HBeWindow*>(m_app->m_window);
	assert(w!=NULL);
	w->CreateWindow(m_app);
	
	app.Run();
	delete m_app;
	
	return 0;
}

