
#ifndef __WORLD_Block_H_
#define __WORLD_Block_H_

namespace std {}
using namespace std;

#include <fstream>

#include <vector>

#include "H_Geometry.h"
#include "H_Object.h"

class WorldBlock
{
public:	
	class MyTriangle
	{
	public:
		MyTriangle();
		MyTriangle(int Aa, int Ab, int Ac, int ASurfaceType);
		void Read(istream &in);
		void Write(ostream &out);
		void Init();
		//void reset(int Aa, int Ab, int Ac, int ASurfaceType);
		//void reset(WorldBlock *MyWorldBlock, int i, int j, int k, int ASurfaceType);
		void reset(WorldBlock *MyWorldBlock, int i, int j, int k, int ASurfaceType);
		bool IsInside(const Point2D &point);
		void UpdateValues();
		void FillIfEmpty();
		void Fill();
		void drawBorder();

		Contact GetValueAt(const Point2D &point);
		bool Hit(const FixedVector &ARay);
		bool IsInside(Point3D &X);
		void SetMaterial(int FillMode);
		OFFObject::Polygon toOFFPolygon() const;

	//private:
		Point3D *Corner1;
		Point3D *Corner2;
		Point3D *Corner3;
		int MyI, MyJ, MyK;
		REAL a, b, c;
		Point3D Normal;
		REAL MyOffset;
	public:
		int SurfaceType;
		WorldBlock *MyWorldBlock;
		Point3D Impact;
	};
public:
	WorldBlock();
	bool IsInitialized();

	MyTriangle *WhichTriangle(const Point2D &point);
	void InitTriangles();

	Contact GetValueAt(const Point2D &point);
	void Place(Ref &ARef);
	void InitOffObject();
	Contact GetContact(const Point3D &APoint3D);
	Contact GetContact(const FixedVector &WheelRay);
	//the draw for GL
	void draw();

	void ReadFile(const char *WorldBlockkName);
	void WriteFile(ofstream &outfile);

public:
//private:
	vector<OFFObject::Vertex> MyOFFVertexes;
	vector<MyTriangle> Triangles;

	Ref EndPosition;
	OFFObject MyOffObject;

	//to do with opt.
	MyTriangle *PreviousTriangle; //the previous triangle the cursor was in
	HRect EncapsulatingRect;
};

//define the different surface types:
#define GRASS_SURFACE 0
#define TARMAC_SURFACE 1

//operator overloads:
istream &operator >> (istream &infile, WorldBlock::MyTriangle &ATriangle);
ostream &operator << (ostream &outfile, WorldBlock::MyTriangle &ATriangle);

#endif //__WORLD_Block_H_
