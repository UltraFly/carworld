

#include "H_Standard.h"
#include "CarWorld.h"
#include "H_Graphics.h"
#include <stdio.h>
#include <sstream>

//CLASS CarWorld:
CarWorld::CarWorld(int TimeRefreshRate, const char *LandscapeFile) :
						m_Landscape(new CWLandscape(LandscapeFile)),
						m_Camera(new FixCam()),
						m_Background(new CWBackground()),
						RealTime(0),
						Frames(0),
						NbTimeClicksPerFrame(0),
						fps(0.f),
						MyTimeRefreshRate(TimeRefreshRate),
						draw_console(false),
						draw_background(true),
						LightDirection(-.6f,-.4f,1.f)
{
	add(m_Landscape);
	add(m_Camera);
	add(m_Background);

	Ref::TimeIncrement = INIT_TIME_INCREMENT;
}
void CarWorld::draw_init()
{
	Hgl::ClearColor(Color(0.3f, 0.3f, 1.0f));
	for (list<CWFeature*>::iterator I=m_Features.begin() ; I!=m_Features.end() ; I++)
		(*I)->draw_init();
}
void CarWorld::draw_shutdown()
{
	for (list<CWFeature*>::iterator I=m_Features.begin() ; I!=m_Features.end() ; I++)
		(*I)->draw_shutdown();
}
CarWorld::~CarWorld()
{
	for (list<CWFeature*>::iterator I=m_Features.begin() ; I!=m_Features.end() ; I++)
		delete (*I);
}
void CarWorld::add(CWFeature* AFeature)
{
	AFeature->m_CarWorld = this;
	m_Features.push_back(AFeature);
	AFeature->reset();
}

void CarWorld::add(CWVehicle* AVehicle)
{
	add((CWFeature*)AVehicle);
	add(m_Camera = new InCarCam(AVehicle));
	add(new FixCam(AVehicle));
	add(new FollowCam(AVehicle));
	add(new SateliteCam(AVehicle));
}

//precondition: m_Camera is a pointer to a CWFeature in m_Features
void CarWorld::next_camera()
{
	list<CWFeature*>::iterator I = m_Features.begin();
	//find the current camera
	while (*I != m_Camera) I++;
	//find the next one
	I++;
	while (I!=m_Features.end() && dynamic_cast<CWCamera*>(*I)==NULL) I++;
	if (I==m_Features.end())
	{
		I = m_Features.begin();
		while (dynamic_cast<CWCamera*>(*I)==NULL) I++;
	}
	m_Camera = dynamic_cast<CWCamera*>(*I);

}

void CarWorld::reset()
{
	for (list<CWFeature*>::iterator I=m_Features.begin() ; I!=m_Features.end() ; I++)
		(*I)->reset();
}

void CarWorld::update(int ElapsedTimeMs)
{
	RealTime += ElapsedTimeMs;
	Frames += 1;
	//re-evaluare frame rate every 5 seconds
	if (RealTime > MyTimeRefreshRate)
	{
		fps = (Frames*1000.0f)/RealTime;
		//reevaluate Ref::NbTimeClicksPerSec
		//and NbTimeClicksPerFrame
		//so that time always pases at the same speed
		{
			NbTimeClicksPerFrame = 1;
			while (MIN_TIME_CLICKS_SEC>NbTimeClicksPerFrame*fps)
			{
				++NbTimeClicksPerFrame;
			}
			Ref::TimeIncrement = 1/(NbTimeClicksPerFrame*fps);
		}
		Frames = 0;
		RealTime = 0;
	}

	for (int i=0 ; i<NbTimeClicksPerFrame ; i++)
	{
		for (list<CWFeature*>::iterator I=m_Features.begin() ; I!=m_Features.end() ; I++)
			(*I)->update();
	}
}

void CarWorld::draw()
{
	//cout << "clearing screen...\n";
	int ToBeCleared = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
	if (Hgl::GetShadows()>0)
		ToBeCleared |= GL_STENCIL_BUFFER_BIT;
	glClear(ToBeCleared);

	//cout << "looking from the camera...\n";
	glLoadIdentity();
	Hgl::LookFrom(m_Camera->GetRef());

	//cout << "setting the light source...\n";
	Hgl::SetLightSource(LightDirection);

	//cout << "drawing the background...\n";
	if (draw_background)
		m_Background->draw();

	//cout << "drawing the landscape...\n";
	m_Landscape->draw();

	//cout << "draw all the other features...\n";
	for (list<CWFeature*>::iterator I = m_Features.begin() ; I!=m_Features.end() ; I++)
	{
		if (dynamic_cast<CWLandscape*>(*I)==NULL && dynamic_cast<CWBackground*>(*I)==NULL)
			(*I)->draw();
	}
}
void CarWorld::DrawOnScreen()
{
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1, -1.0, 1.0 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	ostringstream FPSCaption;
	FPSCaption << fps;
	Hgl::SetColor(White);
	Hgl::WriteText(FPSCaption.str().c_str(), Point2D(-.25,.75)); //write fps and speed*/

	//draw car info
	Hgl::Translate(Point3D(.75,-.75,0));
	Hgl::Scale(0.2f);
	m_Camera->DrawOnScreen();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

