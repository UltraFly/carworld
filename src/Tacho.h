
#ifndef __TACHO_H_
#define __TACHO_H_

namespace std {}
using namespace std;

#include "H_Graphics.h"

class Tacho
{
public:
	Tacho();
	Tacho(REAL RedLine);
	~Tacho();
	void draw(REAL Revs);
private:
	int CompiledList;
	REAL MyRedLine;
};

class DriveTrain
{
public:
	//EnginRevs(ex:Redline) and WheelRevs given in rev.s-1
	//gear ratio is given in EnginRevs/WheelRevs
	//Torque is given in N.m
	DriveTrain();
	DriveTrain(REAL Redline, REAL MaxTorque/*to be replaced by array*/);
	REAL GetTorque(REAL Gas, REAL WheelRevs);
	void drawInfo(REAL tmp);
private:
	REAL GetRevs(); //convert from rev/sec to rev/min
	int MyNbGears;
	REAL *MyGearRatios;
	REAL MyPower;
	REAL MyRedline;
	Tacho MyTacho;
	REAL MyMaxTorque;
};

#endif //__TACHO_H_
