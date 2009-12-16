
#include <math.h>
#include <string.h>

#include <algorithm>

#include "H_Graphics.h"
#include "H_Object.h"
#include "H_Standard.h"

bool OFFObject::UseOptimizedDraw = false;

const Color OFFObject::Polygon::DefaultColor = Grey;

template<> list<HPointer<Texture>::Ref > HPointer<Texture>::RefList;

//CLASS OFFObject::Point:
OFFObject::Vertex::Vertex()
{}
OFFObject::Vertex::Vertex(const Point3D &APosition, const Point3D &ANormal, const Point2D &ATexCoord) :
		Position(APosition),
		Normal(ANormal),
		TexCoord(ATexCoord)
{}
OFFObject::Vertex::Vertex(const OFFObject::Vertex &AVertex) :
		Position(AVertex.Position),
		Normal(AVertex.Normal),
		TexCoord(AVertex.TexCoord)
{}
istream &OFFObject::Vertex::Read(istream &in)
{
	//do not read normal: it is calculated automatically
	in >> Position;
	in >> TexCoord;
	return in;
}
ostream &OFFObject::Vertex::Write(ostream &out) const
{
	out << Position;
	out << TexCoord;
	return out;
}
istream &operator >> (istream &in, OFFObject::Vertex &AVertex)
{
	return AVertex.Read(in);
}
ostream &operator << (ostream &out, const OFFObject::Vertex &AVertex)
{
	return AVertex.Write(out);
}

//CLASS OFFObject::Polygon:
OFFObject::Polygon::Polygon() {}

OFFObject::Polygon::~Polygon() {}

void OFFObject::Polygon::InitNormal(const vector<OFFObject::Vertex>& OFFVertexes)
{
	Normal = (OFFVertexes[IndexVertexes[2]].Position-OFFVertexes[IndexVertexes[0]].Position)
		^(OFFVertexes[IndexVertexes[1]].Position-OFFVertexes[IndexVertexes[0]].Position);
	Normal.normalize();
}

void OFFObject::Polygon::Invert()
{
	unsigned int swap;
	for (unsigned int k=0 ; k<IndexVertexes.size()/2 ; k++)
	{
		swap = IndexVertexes[k];
		IndexVertexes[k] = IndexVertexes[IndexVertexes.size()-k-1];
		IndexVertexes[IndexVertexes.size()-k-1] = swap;
	}
}

istream &OFFObject::Polygon::Read(istream &in)
{
	unsigned int size;
	in >> size;
	IndexVertexes.resize(size);
	for (unsigned int i=0 ; i<IndexVertexes.size() ; i++)
		in >> IndexVertexes[i];

	//if there is color data, read it
	EatWhite(in);
	if (in.peek() == '0' || in.peek() == '1') // there is a color
		in >> MyColor;
	else //no color found in file
		MyColor = DefaultColor;
	return in;
}
ostream &OFFObject::Polygon::Write(ostream &out) const
{
	out << (IndexVertexes.size());
	for (unsigned int i=0 ; i<IndexVertexes.size() ; i++)
		out << " " << IndexVertexes[i];
	out << "\t" << MyColor;
	return out;
}
istream &operator >> (istream &in, OFFObject::Polygon &APolygon)
{
	return APolygon.Read(in);
}
ostream &operator << (ostream &out, OFFObject::Polygon &APolygon)
{
	return APolygon.Write(out);
}

//CLASS OFFObject::Buffer:
void OFFObject::Buffer::AddVertex(const OFFObject::Vertex &AOFFVertex)
{
	Vertexes.push_back(AOFFVertex);
}

void OFFObject::Buffer::AddPolygon(const OFFObject::Polygon &ATriangle)
{
	Polygons.push_back(ATriangle);
}

void OFFObject::Buffer::SetTexture(const char *ATextureName)
{
	TextureName = ATextureName;
}

void OFFObject::Buffer::ReturnOffObject(OFFObject &tmp)
{
	tmp.MyTexture = (TextureName.empty()) ? NULL : new Texture(TextureName.c_str());
	tmp.Allocate(Vertexes.size(), Polygons.size());

	copy(Vertexes.begin(), Vertexes.end(), tmp.MyOFFVertexes.begin());
	copy(Polygons.begin(), Polygons.end(), tmp.MyOFFPolygons.begin());

	tmp.InitMyData();
}

//CLASS OFFObject:
OFFObject::OFFObject() :
	IsSmooth(true),
	AllSameColor(true), //BUG this var is not updated
	MyTexture(NULL),
	CompiledList(0),
	CompiledList_NoMaterial(0)
{}

OFFObject::~OFFObject()
{
	Release();
}

void OFFObject::Allocate(unsigned int NbVertexs, unsigned int NbPolygons)
{
	MyOFFPolygons.resize(NbPolygons);
	MyOFFVertexes.resize(NbVertexs);
}

void OFFObject::Release()
{
	delete MyTexture;
	MyTexture = NULL;
	HGL_DELETE_LIST(CompiledList);
	HGL_DELETE_LIST(CompiledList_NoMaterial);
}


bool OFFObject::IsInit()
{
	return MyOFFVertexes.size() != 0;
}

void OFFObject::readfile(const char* FileName)
{
	try
	{
		Release();
		//cout << "    " << FileName << "\n";
		ifstream infile;
		OpenDataFile(infile, FileName);

		bool IsTextured = false;
		IsSmooth = true;
		if (infile.peek() == 'o' || infile.peek() == 'O')
		{
			char TrashText[150];
			infile.getline(TrashText,150);
			IsTextured = (strcmp(TrashText,"OFF2")==0);
		}
		if (IsTextured)
		{
			char TextureName[150];
			infile.getline(TextureName,150);
			string TextureFile = PathOf(FileName)+'/'+TextureName;
			//H_NEW_ONE(MyTexture, Texture((char*)TextureFile));
			MyTexture = new Texture(TextureFile.c_str());
		}
		unsigned int NVERTICES, NFACES, NEDGES;
		infile >> NVERTICES >> NFACES >> NEDGES;
		Allocate(NVERTICES, NFACES);
		//cout << read all the points from the file
		for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
		{
			infile >> MyOFFVertexes[i].Position;
			if (IsTextured)
				infile >> MyOFFVertexes[i].TexCoord;
		}

		//cout << read all the faces from the file
		for (unsigned int j=0 ; j<MyOFFPolygons.size() ; j++)
			infile >> MyOFFPolygons[j];

		//cout << "initializing OFF data...\n";
		InitMyData();
	}
	catch (const HException &E)
	{
		Release();
		throw HException(string("problem with OFF object \"")+TitleOf(FileName)+"\":\n"+E.what());
	}
}

void OFFObject::writefile(const char* FileName)
{
	ofstream outfile(FileName, ios::out);
	if (MyTexture != 0)
		outfile << "OFF2\n" << (*MyTexture).Title() << "\n";
	else
		outfile << "OFF\n";
	outfile << MyOFFVertexes.size() << " ";
	outfile << MyOFFPolygons.size() << " 0\n";
	//0 is the number of edges, for compatibility with OFF format
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
	{
		outfile << MyOFFVertexes[i].Position;
		if (MyTexture != 0)
			outfile << "\t" << MyOFFVertexes[i].TexCoord;
		outfile << endl;
	}
	
	outfile << endl;

	//write all the faces from the file
	for (unsigned int j=0 ; j<MyOFFPolygons.size() ; j++)
		outfile << MyOFFPolygons[j] << endl;
}


void OFFObject::debug_dump()
{
	ofstream outfile("dump.txt", ios::out);
	if (MyTexture != 0)
		outfile << "OFF2\n" << (*MyTexture).Title() << "\n";
	else
		outfile << "OFF\n";
	outfile << MyOFFVertexes.size() << " ";
	outfile << MyOFFPolygons.size() << " 0\n";
	//0 is the number of edges, for compatibility with OFF format
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
	{
		outfile << "pos:" << MyOFFVertexes[i].Position;
		if (MyTexture != 0)
			outfile << "\ttex:" << MyOFFVertexes[i].TexCoord;
		outfile << "normal:" << MyOFFVertexes[i].Normal;
		outfile << endl;
	}
	
	outfile << endl;

	//write all the faces from the file
	for (unsigned int j=0 ; j<MyOFFPolygons.size() ; j++)
		outfile << MyOFFPolygons[j] << endl;
}

void OFFObject::InitMyData()
{
	unsigned int i,j;

	HGL_DELETE_LIST(CompiledList);
	HGL_DELETE_LIST(CompiledList_NoMaterial);

	//init the polygon normals
	for (i=0 ; i<MyOFFPolygons.size() ; i++)
		MyOFFPolygons[i].InitNormal(MyOFFVertexes);

	//initialize the vertex normals
	for (i=0 ; i<MyOFFVertexes.size() ; i++)
		MyOFFVertexes[i].Normal = Point3D(0,0,0);
	
	for (i=0 ; i<MyOFFPolygons.size() ; i++)
		for (j=0 ; j<MyOFFPolygons[i].IndexVertexes.size() ; j++)
			MyOFFVertexes[MyOFFPolygons[i].IndexVertexes[j]].Normal += MyOFFPolygons[i].Normal;

	for (i=0 ; i<MyOFFVertexes.size() ; i++)
		MyOFFVertexes[i].Normal.normalize();
	
	//figure out the number of triangles in the object
	int NumTriangles = 0;
	for (i=0 ; i<MyOFFPolygons.size() ; i++)
		NumTriangles += MyOFFPolygons[i].IndexVertexes.size() - 2;
	//now fill in NumTriangles with the correct indexes
	TriIndexes.resize(3*NumTriangles);
	int current = 0;
	for (i=0 ; i<MyOFFPolygons.size() ; i++)
	{
		for (j=1 ; j<MyOFFPolygons[i].IndexVertexes.size()-1 ; j++)
		{
			TriIndexes[current++] = MyOFFPolygons[i].IndexVertexes[0];
			TriIndexes[current++] = MyOFFPolygons[i].IndexVertexes[j];
			TriIndexes[current++] = MyOFFPolygons[i].IndexVertexes[j+1];
		}
	}
}

void OFFObject::Center()
{
	unsigned int i;
	//calculate the barycenter
	Point3D BaryC(0,0,0);
	for (i=0 ; i<MyOFFVertexes.size() ; i++)
		BaryC += MyOFFVertexes[i].Position;
	BaryC /= (REAL)MyOFFVertexes.size();

	//move the object to the origine
	for (i=0 ; i<MyOFFVertexes.size() ; i++)
		MyOFFVertexes[i].Position -= BaryC;
		
	InitMyData();
}
void OFFObject::Scale(const Point3D &scale)
{
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
	{
		MyOFFVertexes[i].Position.x() = MyOFFVertexes[i].Position.x()*scale.x();
		MyOFFVertexes[i].Position.y() = MyOFFVertexes[i].Position.y()*scale.y();
		MyOFFVertexes[i].Position.z() = MyOFFVertexes[i].Position.z()*scale.z();
	}
	InitMyData();
}

void OFFObject::draw(const Ref &Position)
{
	//is the object initialized?
	if (!IsInit()) return;

	//do we have to use the texture?
	if
	(
		(! Hgl::IsEnabled(Hgl::SHADOW))
		&& (MyTexture != 0)
		&& (Hgl::GetTextureMode() != Hgl::NONE)
	)
		MyTexture->Use();
	else
		glDisable(GL_TEXTURE_2D);

	//draw at the correct location
	glPushMatrix();
	Hgl::Relocate(Position);
	if (UseOptimizedDraw)
		GLdraw(!Hgl::IsEnabled(Hgl::SHADOW));
	else
		GLdraw_NoOpt(!Hgl::IsEnabled(Hgl::SHADOW));
	glPopMatrix();
}

void OFFObject::GLdraw(bool UseMaterial)
{
	//do we have to build the lists?
	if (!CompiledList) BuildCompiledLists();

	//call the right list
	if (UseMaterial)
		glCallList(CompiledList);
	else
		glCallList(CompiledList_NoMaterial);
}

void OFFObject::GLdraw_NoOpt(bool UseMaterial)
{
	if (UseMaterial)
	{
		for (vector<Polygon>::iterator I=MyOFFPolygons.begin() ; I!=MyOFFPolygons.end() ; I++)
		{
			if (!IsSmooth)
				Hgl::Normal(I->Normal);
			if (MyTexture == 0)
				Hgl::SetColor(I->MyColor);
			Hgl::Begin(GL_POLYGON);
			for (unsigned int i=0 ; i<I->IndexVertexes.size() ; i++)
			{
				if (IsSmooth)
					Hgl::Normal(MyOFFVertexes[I->IndexVertexes[i]].Normal);
				if (MyTexture != 0)
					Hgl::TexCoord(MyOFFVertexes[I->IndexVertexes[i]].TexCoord);
				Hgl::Vertex(MyOFFVertexes[I->IndexVertexes[i]].Position);
			}
			Hgl::End();
		}
	}
	else
	{
		for (vector<Polygon>::iterator I=MyOFFPolygons.begin() ; I!=MyOFFPolygons.end() ; I++)
		{
			Hgl::Begin(GL_POLYGON);
			for (unsigned int i=0 ; i<I->IndexVertexes.size() ; i++)
				Hgl::Vertex(MyOFFVertexes[I->IndexVertexes[i]].Position);
			Hgl::End();
		}
	}
}

//BUG this function needs reworking:
//initialize TriIndexes by using tesselation and use that instead
//(1 DrawElement call per object)
void OFFObject::BuildCompiledLists()
{
	unsigned int i;
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(
		3,
		GL_FLOAT,
		sizeof(OFFObject::Vertex),
		&(MyOFFVertexes[0].Position)
	);
	Hgl::LockArrays(0, MyOFFVertexes.size());

	//Build the CompiledList_NoMaterial for the shadow
	HGL_NEW_LIST(CompiledList_NoMaterial);
		glDrawElements(
			GL_TRIANGLES,           
			TriIndexes.size(),         
			GL_UNSIGNED_INT,           
			(const void*)&TriIndexes[0]
		);
	glEndList();

	//Build the CompiledList to be called when the object is drawn
	if (IsSmooth)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(OFFObject::Vertex), &(MyOFFVertexes[0].Normal));
	}
	if (MyTexture != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(OFFObject::Vertex), &(MyOFFVertexes[0].TexCoord));
	}
	HGL_NEW_LIST(CompiledList);
	if (MyTexture != 0)
		Hgl::SetColor(Color(.8f,.8f,.8f));
	Color PreviousColor(-1,-1,-1);
	if (IsSmooth && AllSameColor)
	{
		Hgl::SetColor(MyOFFPolygons[0].MyColor);
		glDrawElements(
			GL_TRIANGLES,           
			TriIndexes.size(),         
			GL_UNSIGNED_INT,           
			(const void*)&TriIndexes[0]
		);
	}
	else //les optimized path
	{
		for (i=0 ; i<MyOFFPolygons.size() ; i++)
		{
			if (!IsSmooth) Hgl::Normal(MyOFFPolygons[i].Normal);
			if ((MyTexture == 0) && (MyOFFPolygons[i].MyColor!=PreviousColor))
			{
				PreviousColor=MyOFFPolygons[i].MyColor;
				Hgl::SetColor(MyOFFPolygons[i].MyColor);
			}
			glDrawElements(
				GL_POLYGON,           
				MyOFFPolygons[i].IndexVertexes.size(),         
				GL_UNSIGNED_INT,           
				(const void*)&MyOFFPolygons[i].IndexVertexes[0]
			);
		}
	}
	glEndList();

	Hgl::UnlockArrays();

	glPopClientAttrib();
}

REAL OFFObject::GetMaxDist()
{
	REAL MaxDist = 0;
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
		MaxDist = max(MaxDist,MyOFFVertexes[i].Position.norm());
	return MaxDist;
}
void OFFObject::InvertNormals()
{
	for (unsigned int i=0 ; i<MyOFFPolygons.size() ; i++)
		MyOFFPolygons[i].Invert();
	InitMyData();
}

void OFFObject::Rotate(const Point3D &K)
{
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
		RotateAround(MyOFFVertexes[i].Position,K);
	InitMyData();
}
void OFFObject::Translate(const Point3D &K)
{
	for (unsigned int i=0 ; i<MyOFFVertexes.size() ; i++)
		MyOFFVertexes[i].Position += K;
	InitMyData();
}

void OFFObject::SetShading(bool AIsSmooth)
{
	IsSmooth = AIsSmooth;
	InitMyData();
}

bool OFFObject::GetShading()
{
	return IsSmooth;
}
