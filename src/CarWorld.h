
#ifndef __CAR_WORLD_H_
#define __CAR_WORLD_H_

#include "H_Standard.h"
#include "CarWorldClasses.h"
#include "CWCamera.h"

//#define DEFAULT_VEHICLE "data/Vehicles/82porsch.vhc"
#define DEFAULT_VEHICLE "data/Vehicles/truck.vhc"
#define DEFAULT_LANDSCAPE "data/Landscape/landscape.txt"

#define EARTH_GRAVITY 9.8f

//World caracteristics:
#define INIT_TIME_INCREMENT 0.0001f
#define MIN_TIME_CLICKS_SEC 40.0

class CarWorld
{
public:
	CarWorld(int TimeRefreshRate, const char *LandscapeFile);
	~CarWorld();
	void add(CWFeature* AFeature);
	void add(CWVehicle* AVehicle);
	void next_camera();
	void DrawOnScreen();

	void reset();
	void update(int ElapsedTimeMs);
	void draw_init();
	void draw();

public:
	CWLandscape *m_Landscape;
	CWCamera *m_Camera; //the current camera
private:
	CWBackground *m_Background;

	list<CWFeature*> m_Features;

	//to do with time scaling:
	int RealTime; //in ms since start
	int Frames;
	int NbTimeClicksPerFrame;
	REAL fps;
	int MyTimeRefreshRate;
public:
	bool draw_console;
	bool draw_background;
	Point3D LightDirection;
};

#endif //__CAR_WORLD_H_
