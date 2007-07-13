
#ifndef __H_OBJECT_H_
#define __H_OBJECT_H_

namespace std {}
using namespace std;

#include <fstream>

#include <list>

#include "H_Graphics.h"

class OFFObject
{
public:
	class Vertex
	{
	public:
		Vertex();
		Vertex(const Point3D &APosition, const Point3D &ANormal, const Point2D &ATexCoord);
		Vertex(const Vertex &AVertex);
		istream &Read(istream &in);
		ostream &Write(ostream &out) const;
	public:
		Point3D Position;
		Point3D Normal;
		Point2D TexCoord;
	};

	class Polygon
	{
	public:
		Polygon();
		~Polygon();
		void InitNormal(const vector<Vertex> &OFFVertexes);
		void Invert();
		istream &Read(istream &in);
		ostream &Write(ostream &out) const;
	public:
		vector<unsigned int> IndexVertexes;
		Point3D Normal;
		Color MyColor;
		static const Color DefaultColor;
	};

	class Buffer
	{
	public:
		void AddVertex(const Vertex &AOFFVertex);
		void AddPolygon(const Polygon &ATriangle);
		void SetTexture(const char *ATextureName);
		void ReturnOffObject(OFFObject &tmp);
	private:
		list<Vertex> Vertexes;
		list<Polygon> Polygons;
		string TextureName;
	};

public:
    OFFObject();
    ~OFFObject();
	void readfile(const char* FileName);
	void writefile(const char* FileName);
	void debug_dump();
//set the normals to the vertices
	void InitMyData();
	void draw(const Ref &Position = Ref());
//optionnal manipulating functions
	bool IsInit();
	REAL GetMaxDist();
	void InvertNormals();
	void Center();
	void Scale(const Point3D &scale);
	void Rotate(const Point3D &K);
	void Translate(const Point3D &K);
	void SetShading(bool AIsSmooth);
	bool GetShading();
	static bool UseOptimizedDraw;
private:
	void Allocate(unsigned int NbVertex, unsigned int NbPolygons);
	void Release();
	//optimized GLdraw using vertex arrays and lists
	void BuildCompiledLists();
	void GLdraw(bool UseMaterial = true);
	//unoptimized using glVertex
	void GLdraw_NoOpt(bool UseMaterial = true);

private:
//variables
	bool IsSmooth;
//all polygons are the same color
	bool AllSameColor;
	Texture *MyTexture;
	int CompiledList;
	int CompiledList_NoMaterial;
	vector<Vertex> MyOFFVertexes;
	vector<Polygon> MyOFFPolygons;
//TriIndexes is an array of indexes which represent the polygons
//as triangles
	vector<unsigned int> TriIndexes;

	friend class Buffer;
	friend istream &operator >> (istream &in, OFFObject::Vertex &AVertex);
	friend ostream &operator << (ostream &out, const OFFObject::Vertex &AVertex);

	friend istream &operator >> (istream &in, OFFObject::Polygon &APolygon);
	friend ostream &operator << (ostream &out, const OFFObject::Polygon &APolygon);
};

istream &operator >> (istream &in, OFFObject::Vertex &APoint);
ostream &operator << (ostream &out, const OFFObject::Vertex &APoint);

istream &operator >> (istream &in, OFFObject::Polygon &APolygon);
ostream &operator << (ostream &out, const OFFObject::Polygon &APolygon);

#endif //__OFF_OBJECT_H_
