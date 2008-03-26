
#include "H_Standard.h"
#include "WorldBlock.h"
#include "H_Graphics.h"

#define NB_BITMAP_PICS 1
#define ROAD_BITMAP 0


static Color TarmacColor(255,255,255);
static Color GrassColor(0,255,0);

//CLASS MyTriangle
WorldBlock::MyTriangle::MyTriangle() :
	SurfaceType(GRASS_SURFACE)
{}

void WorldBlock::MyTriangle::Read(istream &in)
{
	in >> MyI >> MyJ >> MyK >> SurfaceType;
}
void WorldBlock::MyTriangle::Write(ostream &out)
{
	out << MyI << " " << MyJ << " " << MyK << " " << SurfaceType << "\n";
}

void WorldBlock::MyTriangle::reset(WorldBlock *AWorldBlock, int i, int j, int k, int ASurfaceType)
{
	MyWorldBlock = AWorldBlock;
	MyI = i;
	MyJ = j;
	MyK = k;
	SurfaceType = ASurfaceType;
	Init();
}

void WorldBlock::MyTriangle::Init()
{
	Corner1 = &MyWorldBlock->MyOFFVertexes[MyI].Position;
	Corner2 = &MyWorldBlock->MyOFFVertexes[MyJ].Position;
	Corner3 = &MyWorldBlock->MyOFFVertexes[MyK].Position;
	UpdateValues();
}

static bool AngleAigu(const Point2D &A, const Point2D &B)
{
	return (0<=(A.x()*B.y() - A.y()*B.x() + H_EPSILON)); //the H_EPSILON is for tolerance
}


bool WorldBlock::MyTriangle::IsInside(const Point2D &point)
{
	Point2D v1 = Point2D(Corner1->x(),Corner1->y()) - point;
	Point2D v2 = Point2D(Corner2->x(),Corner2->y()) - point;
	Point2D v3 = Point2D(Corner3->x(),Corner3->y()) - point;

	return (AngleAigu(v1,v3) && AngleAigu(v3,v2) && AngleAigu(v2,v1));
}
void WorldBlock::MyTriangle::UpdateValues()
{
	Point3D pt1 = *Corner1;
	Point3D pt2 = *Corner2;
	Point3D pt3 = *Corner3;

	REAL coeff1 = (pt2.x() - pt3.x());
	REAL coeff2 = (pt3.x() - pt1.x());
	REAL coeff3 = (pt1.x() - pt2.x());

	if (pt1.z()==0 && pt2.z()==0 && pt3.z()==0) //the triangle is in the boundarie
	{
		a = 0.0;
		b = 0.0;
		c = 0.0;
	}

	else
	{	// compute a,b,c
		if ((pt1.y()*coeff1 + pt2.y()*coeff2 + pt3.y()*coeff3) != 0)
		{
			c = (pt1.z()*coeff1 + pt2.z()*coeff2 + pt3.z()*coeff3)
			/(pt1.y()*coeff1 + pt2.y()*coeff2 + pt3.y()*coeff3 );
		}
		else
		{
			c=0.0;
		}

		if ((pt1.x() - pt2.x())!=0)
		{
			b = ((pt1.z()-pt2.z()) + c*(pt2.y()-pt1.y())) / (pt1.x() - pt2.x());
		}
		else
		{
			//b=0.0;
			b = ((pt2.z()-pt3.z()) + c*(pt3.y()-pt2.y())) / (pt2.x() - pt3.x());
		}

		a = pt1.z() - b*pt1.x() - c*pt1.y();
	}

	Normal = (pt3-pt1)^(pt2-pt1);
	Normal.normalize();
	MyOffset = -1*((*Corner1)*Normal);
}

void WorldBlock::MyTriangle::FillIfEmpty()
{
	if (Corner1->z()==0 && Corner2->z()==0 && Corner3->z()==0)
		SurfaceType = TARMAC_SURFACE;
}

void WorldBlock::MyTriangle::Fill()
{
	switch (SurfaceType)
	{
	case TARMAC_SURFACE :
		/*BitmapPic[ROAD_BITMAP].Use();
		Hgl::Begin(GL_TRIANGLES);
			glColor3f(1,1,1);
			Hgl::TexCoord(TexCoord1) ; Hgl::Vertex(Corner1.LessZAxe());
			Hgl::TexCoord(TexCoord3) ; Hgl::Vertex(Corner3.LessZAxe());
			Hgl::TexCoord(TexCoord2) ; Hgl::Vertex(Corner2.LessZAxe());
		Hgl::End();
		glDisable(GL_TEXTURE_2D) ;*/
		break;
	case GRASS_SURFACE :
	/*	Hgl::Color(GrassColor);
		Hgl::Begin(GL_TRIANGLES);
			Hgl::Vertex(Corner1.LessZAxe());
			Hgl::Vertex(Corner3.LessZAxe());
			Hgl::Vertex(Corner2.LessZAxe());
		Hgl::End();*/
		break;
	default:
		break;
	}
	drawBorder();
}
void WorldBlock::MyTriangle::drawBorder()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	Hgl::SetColor(Color(0,0,0));
	Hgl::Begin(GL_LINE_STRIP);
		Hgl::Vertex(Point2D(Corner1->x(),Corner1->y()));
		Hgl::Vertex(Point2D(Corner3->x(),Corner3->y()));
		Hgl::Vertex(Point2D(Corner2->x(),Corner2->y()));
		Hgl::Vertex(Point2D(Corner1->x(),Corner1->y()));
	Hgl::End();
	glPopAttrib();
}

Contact WorldBlock::MyTriangle::GetValueAt(const Point2D &point)
{
	Contact tmp;
	tmp.Found = true;
	tmp.Position = Point3D(point.x(),point.y(),a+(b*point.x())+(c*point.y()));
	tmp.Normal = Normal;
	tmp.SurfaceType = SurfaceType;
	return tmp;
}
bool WorldBlock::MyTriangle::Hit(const FixedVector &ARay)
{
	REAL t = -((MyOffset+(Normal*ARay.Position))/(Normal*ARay.Value));
	if ((Normal*ARay.Value == 0) || t<=0) //Ray is || to plane or behind orig of ray
		//discard: set Normal to be the null FixedVector
		return false;
	else
	{
		Impact = ARay.Position + ARay.Value*t;
		if (IsInside(Impact))
		{
			return true;
		}
		else
			return false;
	}
}
bool WorldBlock::MyTriangle::IsInside(Point3D &X)
{
	//to be optimized
	bool IsIn = true;
	Point3D V1 = (*Corner1)-X;
	Point3D V2 = (*Corner2)-X;
	Point3D V3 = (*Corner3)-X;
	Point3D Reference = V1^V2;
	Point3D ThisOne = V2^V3;
	if (Reference*ThisOne<0) IsIn = false;
	ThisOne = V3^V1;
	if (Reference*ThisOne<0) IsIn = false;
	return IsIn;
}
void WorldBlock::MyTriangle::SetMaterial(int FillMode)
{
	SurfaceType = FillMode;
}
OFFObject::Polygon WorldBlock::MyTriangle::toOFFPolygon() const
{
	OFFObject::Polygon tmp;
	tmp.IndexVertexes.resize(3);
	tmp.IndexVertexes[0] = MyI;
	tmp.IndexVertexes[1] = MyJ;
	tmp.IndexVertexes[2] = MyK;
	if (SurfaceType==0)
		tmp.MyColor = Color(0,100,0);
	else //it's tarmac
		tmp.MyColor = Color(255,255,255);
	return tmp;
}
istream &operator >> (istream &infile, WorldBlock::MyTriangle &ATriangle)
{
	ATriangle.Read(infile);
	return infile;
}
ostream &operator << (ostream &outfile, WorldBlock::MyTriangle &ATriangle)
{
	ATriangle.Write(outfile);
	return outfile;
}

//CLASS WorldBlock

WorldBlock::WorldBlock() :
	PreviousTriangle(NULL)
{
}
void WorldBlock::InitTriangles()
{
	//init triangles:
	for (vector<MyTriangle>::iterator I = Triangles.begin() ; I!=Triangles.end() ; I++)
		(*I).Init();

	//init encapsulating rect:
	Point3D tmp = MyOFFVertexes[0].Position;
	EncapsulatingRect = HRect(tmp.x(), tmp.y(), tmp.x(), tmp.y());
	for (vector<OFFObject::Vertex>::iterator J = MyOFFVertexes.begin() ; J!=MyOFFVertexes.end() ; J++)
	{
		tmp = (*J).Position;
		EncapsulatingRect.left = H_MIN(EncapsulatingRect.left,tmp.x());
		EncapsulatingRect.right = H_MAX(EncapsulatingRect.right,tmp.x());
		EncapsulatingRect.top = H_MAX(EncapsulatingRect.top,tmp.y());
		EncapsulatingRect.bottom = H_MIN(EncapsulatingRect.bottom,tmp.y());
	}
	//3 extra meters margin
	EncapsulatingRect.left -= 3;
	EncapsulatingRect.right += 3;
	EncapsulatingRect.top += 3;
	EncapsulatingRect.bottom -= 3;

}

bool WorldBlock::IsInitialized()
{
	return MyOFFVertexes.size() > 0;
}
WorldBlock::MyTriangle *WorldBlock::WhichTriangle(const Point2D &point)
{
	//find out which triangle this point is in

	//first test for the same triangle as last time:
	if (PreviousTriangle)
		if(PreviousTriangle->IsInside(point))
			return PreviousTriangle;

	//the point is in a different triangle as last time:
	for (vector<MyTriangle>::iterator I = Triangles.begin() ; I!=Triangles.end() ; I++)
	{
		if ((*I).IsInside(point))
		{
			PreviousTriangle = &(*I);
			return PreviousTriangle;
		}
	}
	PreviousTriangle=0;
	return PreviousTriangle; //not found
}

//CLASS WorldBlock
void WorldBlock::Place(Ref &ARef)
{
	for (vector<OFFObject::Vertex>::iterator J = MyOFFVertexes.begin() ; J!=MyOFFVertexes.end() ; J++)
		(*J).Position = ARef.GetAbsCoord((*J).Position);

	EndPosition = ARef.GetRef(EndPosition);

	InitTriangles();
}
void WorldBlock::InitOffObject()
{
	OFFObject::Buffer tmp;

	for (vector<OFFObject::Vertex>::iterator J = MyOFFVertexes.begin() ; J!=MyOFFVertexes.end() ; J++)
		tmp.AddVertex(*J);

	for (vector<MyTriangle>::iterator I = Triangles.begin() ; I!=Triangles.end() ; I++)
		tmp.AddPolygon((*I).toOFFPolygon());

	tmp.SetTexture("data/Landscape/CWRoad.png");

	tmp.ReturnOffObject(MyOffObject);
	MyOffObject.SetShading(false);
}
Contact WorldBlock::GetContact(const Point3D &APoint3D)
{
	return GetValueAt(Point2D(APoint3D.x(),APoint3D.y()));
}
Contact WorldBlock::GetContact(const FixedVector &WheelRay)
{
	Contact tmp;
	//first see if the wheel is anywhere near the WorldBlock
	if (EncapsulatingRect.IsContained(Point2D(WheelRay.Position.x(),WheelRay.Position.y())))
	{
		//First test if the triangle hit this time is the same as last.
		if (PreviousTriangle!=NULL && PreviousTriangle->Hit(WheelRay))
		{
			tmp.Found = true;
			tmp.Position = PreviousTriangle->Impact;
			tmp.Normal = PreviousTriangle->Normal;
			tmp.SurfaceType = PreviousTriangle->SurfaceType;
			return tmp;
		}
		else
		//find the right triangle:
		for (vector<MyTriangle>::iterator I = Triangles.begin() ; I!=Triangles.end() ; I++)
		{
			if ((*I).Hit(WheelRay))
			{
				tmp.Found = true;
				tmp.Position = (*I).Impact;
				tmp.Normal = (*I).Normal;
				tmp.SurfaceType = (*I).SurfaceType;
				PreviousTriangle = &(*I);
				return tmp;
			}
		}
	}

	return tmp;
}
void WorldBlock::draw()
{
	MyOffObject.draw();
}
Contact WorldBlock::GetValueAt(const Point2D &point)
{
	Contact tmp;
//first see if the wheel is anywhere near the WorldBlock
	if (EncapsulatingRect.IsContained(point))
	{
		MyTriangle *ATriangle = WhichTriangle(point);
		if (ATriangle)
			tmp = ATriangle->GetValueAt(point);
	}
	return tmp;
}

void WorldBlock::ReadFile(const char *WorldBlockName)
{
	try
	{
		ifstream infile;
		//cout << "opening:" << WorldBlockName << endl;
		OpenDataFile(infile, WorldBlockName);

		unsigned int i;

		unsigned int NbPoints, NbTriangles;
		infile >> NbPoints;
		infile >> NbTriangles;

		//read the positions of the controle points
		OFFObject::Vertex CurrentVertex;
		for (i=0 ; i<NbPoints ; i++)
		{
			infile >> CurrentVertex;
			MyOFFVertexes.push_back(CurrentVertex);
		}

		//read the propeties of the triangles
		MyTriangle CurrentTriangle;
		for (i=0 ; i<NbTriangles ; i++)
		{
			infile >> CurrentTriangle;
			CurrentTriangle.MyWorldBlock = this;
			Triangles.push_back(CurrentTriangle);
		}
		infile >> EndPosition;
		InitTriangles();
	}
	catch (const HException &E)
	{
		Triangles.clear();
		MyOFFVertexes.clear();
		throw HException(string("problem with WorldBlock object \"")+TitleOf(WorldBlockName)+("\":\n")+E.what());
	}
}

void WorldBlock::WriteFile(ofstream &outfile)
{
	outfile << MyOFFVertexes.size() << " " << Triangles.size() << "\n";

	for (vector<OFFObject::Vertex>::iterator J = MyOFFVertexes.begin() ; J!=MyOFFVertexes.end() ; J++)
		outfile << (*J);

	for (vector<MyTriangle>::iterator I = Triangles.begin() ; I!=Triangles.end() ; I++)
		outfile << (*I);

	outfile << EndPosition;
}
