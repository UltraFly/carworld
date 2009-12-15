
#ifndef __CAR_WORLD_CLASSES_H_
#define __CAR_WORLD_CLASSES_H_

#include "H_Geometry.h"
#include "H_Input.h"
#include "H_Object.h"
#include "WorldBlock.h"
#include "Tacho.h"
//#include "CWVehiclePropeties.h"

//Set of objects
#define NB_OBJECTS	10
#define OFF_WHEEL	0
#define OFF_CAR		1
#define OFF_BACKGROUND 2

//to do with the roll cage:
//#define DRAW_R_CAGE
#define EPSILON 0.1 //m
#define NB_CAGE_POINTS 5 //m

#define HBRAKE_BUTTON 0

class CarWorld;

//virtual base class for all features (objects) that evolve in the "world"
class CWFeature
{
public:
	virtual ~CWFeature();
	virtual void reset();
//make the feature move by one time increment
	virtual void update();
	//virtual bbox get_bbox() const;
//drawing related
	virtual void draw_init();
	virtual void draw();
public:
	CarWorld *m_CarWorld;
};

class CWBackground : public CWFeature
{
public:
	CWBackground();
	virtual ~CWBackground();
	virtual void draw_init();
	virtual void draw();
private:
	OFFObject m_OFFObject;
};

class CWLandscape : public CWFeature
{
public:
	CWLandscape(const char *file_name);
	virtual ~CWLandscape();
	void draw_init();
	void draw();
	Contact GetContact(const FixedVector &AVector);
	Contact GetContact(const Point3D &APoint);
//private:
	list<WorldBlock> MyWorldBlocks;
};

#endif //__CAR_WORLD_CLASSES_H_
