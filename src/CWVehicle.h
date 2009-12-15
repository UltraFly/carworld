
#ifndef __CW_VEHICLE_H_
#define __CW_VEHICLE_H_

#include "CarWorldClasses.h"


class CWVehicle;

//describes the state of the vehicle controls
struct CWCommand
{
	CWCommand();
	REAL GasBrake;
	REAL Steer;
	REAL SteerFeedBack;
	bool HandBrake;
};

class Wheel
{
public:
	Wheel(
		const Point3D &RelPos = Point3D(),
		REAL SpringStiffness = 30000,
		REAL MaxTravel = -0.5f,
		REAL MaxLoad = 5000,
		REAL WheelRadius = 0.4f,
		REAL DamperCompression = 2000,
		REAL DamperRebound = 2000,
		REAL Grip = 1.0f,
		REAL AccelFactor = 4000,
		REAL BrakeFactor = 4000,
		REAL SteerFactor = .5f,
		bool LockUp = true
	);
	void draw_init();
	void reset();
	void Set(
		const Ref &ARef,
		CWCommand &ACommand,
		CarWorld &CW //the world to interact with
	);
	//void Set(Ref &ARef);
	Point3D CalcForce(/*REAL &ForceFeedback*/);
	FixedVector &GetRay();
	void draw();

//constant for this wheel:
public:
	OFFObject Model;
	string ModelFile;
	Point3D ModelPos;
	Point3D ModelScale;

	Point3D RelPos;
	REAL SpringStiffness;
	REAL MaxTravel;
	REAL MaxLoad; //load beyond which the tyre is no longer efficient
	REAL WheelRadius;
	REAL DamperCompression;
	REAL DamperRebound;
	REAL Grip;
	REAL AccelFactor;
	REAL BrakeFactor;
	REAL SteerFactor;
	bool LockUp;

//variables used for computation:
public:
	REAL Thrust;
	REAL Angle;
	bool Lock;
	Point3D Acceleration;
	Ref MyRef;
	Point3D LandSpeed, LandDirection;
	FixedVector Ray; //the ray used to determine the impact with the ground
	REAL Gs;
	REAL Travel, DeltaTravel;
	Contact MyContact;
};

//structure to be sent over the network
struct CWVehicleState
{
	Ref m_Ref;
	CWCommand m_Command;
};

class CWVehicle : public CWFeature
{
public:
	CWVehicle(const char *name);
	void load(const char *name);
	virtual ~CWVehicle();
	void reset();
	void update();
	REAL GetSpeed() const; //returns speed in kph

	//update every thing attached to the car
	//ie: roll cage position, wheels, ...
	void UpdateCWVehicleParams();

	void draw_init();
	void draw();
	void drawShape();
	void ProjectShadow(const Point3D &LightDirection);
	void drawInfo();

	CWVehicleState GetState();
	void SetState(CWVehicleState &state);
public:
//read in the configuration file
	string ModelFile;
	Point3D ModelPos;
	Point3D ModelScale;

	Point3D InitPos;
//mass:
	REAL Mass; //kg
	REAL Weight; //N
	REAL MassDistrib; //kg.m
//to do with drive train:
	REAL RedLine; //REV.min-1
	REAL MaxTorque; //N.m
	REAL Friction; //N.s2.m-2

//the state of the commands of the vehicle
	CWCommand MyCommand;
	DriveTrain MyDriveTrain;
	InertRef MyRef;
	vector<Wheel> Wheels;
	OFFObject Model;
};

#endif //__CW_VEHICLE_H_
