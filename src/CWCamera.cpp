
#include "CWCamera.h"

//the camera classes
//CLASS CWCamera
CWCamera::~CWCamera() {}
void CWCamera::DrawOnScreen() {}
const Point3D CWCamera::X(1,0,0);
const Point3D CWCamera::Y(0,1,0);
const Point3D CWCamera::Z(0,0,1);
const Point3D CWCamera::Up(0,0,1);

//CLASS FixCam
FixCam::FixCam(CWVehicle *Vehicle, const Point3D &Position) :
	m_Position(Position),
	m_Vehicle(Vehicle)
{}

FixCam::~FixCam() {}

Ref FixCam::GetRef()
{
	if (m_Vehicle != NULL)
		return Ref(m_Position,m_Vehicle->MyRef.Position - m_Position,Up);
	else
		return Ref(m_Position,-Z,X);
}

//CLASS FollowCam
FollowCam::FollowCam(CWVehicle *Vehicle) :
	m_Vehicle(Vehicle)
{}

FollowCam::~FollowCam() {}

Ref FollowCam::GetRef()
{
	Point3D Direction(m_Vehicle->MyRef.Speed);
	Direction.z() = 0;
	if (Direction.normalize() == 0) Direction = m_Vehicle->MyRef.GetDirection();
	return Ref(Direction*(-8.f) + m_Vehicle->MyRef.Position + Z*3.f,Direction,Up);
}

//CLASS InCarCam
InCarCam::InCarCam(CWVehicle *Vehicle) :
	m_Vehicle(Vehicle)
{}

InCarCam::~InCarCam() {}

Ref InCarCam::GetRef()
{
	Point3D Location(0.f,-1.0f,.8f);
	return m_Vehicle->MyRef.GetRef(Location);
}

void InCarCam::DrawOnScreen()
{
	m_Vehicle->drawInfo();
}

//CLASS SateliteCam
SateliteCam::SateliteCam(CWVehicle *Vehicle, REAL height) :
	m_Vehicle(Vehicle),
	m_height(height)
{}

SateliteCam::~SateliteCam() {}

Ref SateliteCam::GetRef()
{
	Point3D Y(0,1,0);
	Point3D Z(0,0,1);
	return Ref(m_Vehicle->MyRef.Position+Z*m_height,Z*(-1.f)+Y*.001f,m_Vehicle->MyRef.GetDirection());
}
