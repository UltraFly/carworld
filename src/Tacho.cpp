
#include "Tacho.h"

#define R_INT .8f
#define R_EXT 1.f
#define NEEDLE_LEN_F 1.1f
#define NEEDLE_LEN_B .2f
#define NEEDLE_WIDTH .05f

Tacho::Tacho() {};
Tacho::Tacho(REAL RedLine) : CompiledList(0), MyRedLine(RedLine) {}

Tacho::~Tacho()
{
	if (CompiledList != 0)
		glDeleteLists(CompiledList,1); 
}

static void ZRotate(Point3D &A,REAL angle)
{
	RotateAround(A,Point3D(0,0,angle));
}

void Tacho::draw(REAL Revs)
{
	static REAL Step = -PI/6;
	//draw the frame:
	if (CompiledList != 0)
		glCallList(CompiledList);
	else
	{
		//CompiledList = glGenLists(1);
		//glNewList(CompiledList, GL_COMPILE);

		int i;
		Point3D IntPt(0,-R_INT,0), ExtPt(0,-R_EXT,0);
		Hgl::SetColor(Color(250,130,0));
		ZRotate(IntPt,Step*(1+(MyRedLine/1000)));
		ZRotate(ExtPt,Step*(1+(MyRedLine/1000)));
		glBegin(GL_QUADS);
		for (i=0 ; i<=4 ; i++)
		{
			Hgl::Vertex(IntPt);
			Hgl::Vertex(ExtPt);
			ZRotate(IntPt,Step/3);
			ZRotate(ExtPt,Step/3);
			Hgl::Vertex(ExtPt);
			Hgl::Vertex(IntPt);
			if (i==1) Hgl::SetColor(Color(255,0,0));
		}
		glEnd();

		Hgl::SetColor(Color(255,255,255));
		glLineWidth(2);
		IntPt = Point3D(0,-R_INT,0);
		ExtPt = Point3D(0,-R_EXT,0);
		ZRotate(IntPt,Step);
		ZRotate(ExtPt,Step);
		glBegin(GL_LINES);
		for (i=0 ; i<=(MyRedLine/1000)+2 ; i++)
		{
			Hgl::Vertex(ExtPt);
			Hgl::Vertex(IntPt);
			ZRotate(IntPt,Step);
			ZRotate(ExtPt,Step);
		}
		glEnd();

		//glEndList();
	}
	//draw the needle:
	Point3D TipPt(0,-NEEDLE_LEN_F,0);
	Point3D BackPtR(-NEEDLE_WIDTH,NEEDLE_LEN_B,0);
	Point3D BackPtL(NEEDLE_WIDTH,NEEDLE_LEN_B,0);
	REAL Angle = Step*(1+min(Revs,(MyRedLine+1000))/1000);
	ZRotate(TipPt,Angle);
	ZRotate(BackPtL,Angle);
	ZRotate(BackPtR,Angle);

	Hgl::SetColor(Color(255,0,0));
	glBegin(GL_TRIANGLES);
	Hgl::Vertex(TipPt);
	Hgl::Vertex(BackPtR);
	Hgl::Vertex(BackPtL);
	glEnd();
	Hgl::SetColor(Color(255,255,255));
}

//clas DriveTrain:

DriveTrain::DriveTrain()
{}
DriveTrain::DriveTrain(REAL Redline, REAL MaxTorque/*to be replaced by array*/) :
	MyRedline(Redline),
	MyTacho(Redline),
	MyMaxTorque(MaxTorque)
{

}
REAL DriveTrain::GetTorque(REAL Gas, REAL WheelRevs)
{
	return MyMaxTorque;
}
void DriveTrain::drawInfo(REAL tmp)
{
	MyTacho.draw(tmp*300);
}
