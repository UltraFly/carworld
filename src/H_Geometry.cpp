
//#include "StdAfx.h"

#include <math.h>

#include "H_Geometry.h"

//Point3D class

/*Point3D::Point3D(){}
Point3D::Point3D(REAL ax, REAL ay, REAL az) {x()=ax;y()=ay;z()=az;}
Point3D::Point3D(const Point3D &A) {x()=A.x();y()=A.y();z()=A.z();}
void Point3D::reset(){x()=0;y()=0;z()=0;}
bool Point3D::IsNull() const {return x()==0 && y()==0 && z()==0;}
bool Point3D::operator== (const Point3D &A) const
{
	return	ABS(x()-A.x())<H_EPSILON &&
			ABS(y()-A.y())<H_EPSILON &&
			ABS(z()-A.z())<H_EPSILON;
}
REAL Point3D::norm() const
{
	return (REAL)sqrt((x()*x())+(y()*y())+(z()*z()));
}

REAL Point3D::normalize()
{
	REAL m_norm = norm();
	if (m_norm>0)
	{
		x() = x()/m_norm;
		y() = y()/m_norm;
		z() = z()/m_norm;
	}
	return m_norm;
}
REAL Point3D::normalize(REAL Value)
{
	REAL m_norm = norm();
	if (m_norm>0)
	{
		x() = x()*Value/m_norm;
		y() = y()*Value/m_norm;
		z() = z()*Value/m_norm;
	}
	return m_norm;
}

REAL Point3D::Angle (Point3D A)
{
	Point3D B = *this;
	B.normalize();
	A.normalize();
	return (REAL)acos(LIMIT(-.999999f,A*B,.999999f));
}*/

/*void Point3D::XRotate (REAL teta)
{
	REAL CosO = (REAL)cos(teta);
	REAL SinO = (REAL)sin(teta);
	*this = Point3D(x, CosO*y - SinO*z, SinO*y + CosO*z);
}


void Point3D::YRotate (REAL teta)
{
	REAL CosO = (REAL)cos(teta);
	REAL SinO = (REAL)sin(teta);
	*this = Point3D(CosO*x + SinO*z, y, SinO*x - CosO*z);
}


void Point3D::ZRotate (REAL teta)
{
	REAL CosO = (REAL)cos(teta);
	REAL SinO = (REAL)sin(teta);
	*this = Point3D(CosO*x - SinO*y, SinO*x + CosO*y, z);
}*/

//this function is derived from
//void gl_rotation_matrix( GLfloat angle, GLfloat x, GLfloat y, GLfloat z,
//                        GLfloat m[] )
//found in MesaGL
/*void RotateAround(Point3D &A,Point3D K) //do not pass K by reference!
//void Point3D::Rotate(Point3D K)
{
	Point3D tmp(A);
	REAL angle = K.normalize();

	REAL s, c;
	REAL xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	s = (REAL)sin(angle);
	c = (REAL)cos(angle);

	if (angle>0)
	{
		xx = K.x() * K.x();
		yy = K.y() * K.y();
		zz = K.z() * K.z();
		xy = K.x() * K.y();
		yz = K.y() * K.z();
		zx = K.z() * K.x();
		xs = K.x() * s;
		ys = K.y() * s;
		zs = K.z() * s;
		one_c = 1 - c;

		A.x() = ((one_c * xx) + c)*tmp.x() + ((one_c * xy) - zs)*tmp.y() + ((one_c * zx) + ys)*tmp.z();
		A.y() = ((one_c * xy) + zs)*tmp.x() + ((one_c * yy) + c)*tmp.y() + ((one_c * yz) - xs)*tmp.z();
		A.z() = ((one_c * zx) - ys)*tmp.x() + ((one_c * yz) + xs)*tmp.y() + ((one_c * zz) + c)*tmp.z();
	}
}

Point3D Point3D::operator/ (REAL factor) const
{
	Point3D tmp(x()/factor, y()/factor, z()/factor);
	return tmp;
}

REAL Point3D::operator* (const Point3D &A) const
{
	return (x()*A.x() + y()*A.y() + z()*A.z());
}

Point3D Point3D::operator* (REAL factor) const
{
	return Point3D(x()*factor,y()*factor,z()*factor);
}

Point3D Point3D::operator- (const Point3D &A) const
{
	return Point3D(x()-A.x(),y()-A.y(),z()-A.z());
}

Point3D Point3D::operator+ (const Point3D &A) const
{
	return Point3D(x()+A.x(),y()+A.y(),z()+A.z());
}

Point3D Point3D::operator^ (const Point3D &A) const
{
	Point3D tmp(
		(y()*A.z()-z()*A.y()),
		(z()*A.x()-x()*A.z()),
		(x()*A.y()-y()*A.x())
	);
	return tmp;
}

void Point3D::operator+= (const Point3D &A)
{
	x() += A.x();
	y() += A.y();
	z() += A.z();
}
void Point3D::operator+= (const Point2D &A)
{
	x() += A.x();
	y() += A.y();
}
void Point3D::operator*= (REAL factor)
{
	x() *= factor;
	y() *= factor;
	z() *= factor;
}
void Point3D::operator-= (const Point3D &A)
{
	x() -= A.x();
	y() -= A.y();
	z() -= A.z();
}
void Point3D::operator/= (REAL denom)
{
	x() /= denom;
	y() /= denom;
	z() /= denom;
}
Point3D Point3D::operator - () const
{
	return Point3D(-x(),-y(),-z());
}
REAL sin(Point3D A, Point3D B)
{
	A.normalize();
	B.normalize();
	return (A^B).z();
}
REAL cos(Point3D A, Point3D B)
{
	A.normalize();
	B.normalize();
	return A*B;
}

istream & operator>> (istream &infile, Point3D &APoint)
{
	infile >> APoint.x();
	infile >> APoint.y();
	infile >> APoint.z();
	return infile;
}
ostream & operator<< (ostream &outfile, const Point3D &APoint)
{
	outfile << APoint.x() << " ";
	outfile << APoint.y() << " ";
	outfile << APoint.z() << "\n";
	return outfile;
}*/

void StraightenRef(Point3D &Direction, Point3D &Up, Point3D &W)
{
	W = Direction^Up;
	Up = W^Direction;
	Direction.normalize();
	Up.normalize();
	W.normalize();
}

//CLASS Ref:
REAL Ref::TimeIncrement;

Ref::Ref()
{
	reset();
}
Ref::Ref(const Point3D &APosition, const Point3D &AY, const Point3D &AZ)
{
	reset();
	Position = APosition;
	Y = AY;
	Z = AZ;
	Straighten();
}
void Ref::reset()
{
	Position.reset();
	X = Point3D(1,0,0);
	Y = Point3D(0,1,0);
	Z = Point3D(0,0,1);
	Speed.reset();
	Momentum.reset();
}
void Ref::Straighten()
{
	//Y is the only one guaranteed only to change in length
	//might have to add extra checks here
	X = Y^Z;
	Z = X^Y;
	X.normalize();
	Y.normalize();
	Z.normalize();
}
void Ref::Move(const Point3D &point)
{
	Position += point;
}
void Ref::Rotate(const Point3D &Axe)
{
	RotateAround(Y,Axe);
	RotateAround(Z,Axe);
	Straighten();
}
Point3D Ref::GetAbsCoord(const Point3D &RelPos) const
{
	return (X*RelPos.x() + Y*RelPos.y() + Z*RelPos.z() + Position);
}
Ref Ref::GetRef(const Point3D &RelPos) const
{
	Ref tmp(GetAbsCoord(RelPos),Y,Z);
	tmp.Speed = Speed + (Momentum^(tmp.Position-Position));
	return tmp;
}
Ref Ref::GetRef(const Ref &ARef) const
{
	return Ref(GetAbsCoord(ARef.Position),GetAbsCoord(ARef.Y)-Position,GetAbsCoord(ARef.Z)-Position);
}
Point3D Ref::GetDirection() const
{
	return Y;
}
Point3D Ref::GetUp() const
{
	return Z;
}
Point3D Ref::GetW() const
{
	return X;
}
Point3D Ref::GetX() const
{
	return X;
}
Point3D Ref::GetY() const
{
	return Y;
}
Point3D Ref::GetZ() const
{
	return Z;
}
istream &operator >> (istream &infile, Ref &ARef)
{
	ARef.reset();
	infile >> ARef.Position;
	infile >> ARef.Y;
	infile >> ARef.Z;
	ARef.Straighten();
	return infile;
}
ostream &operator << (ostream &outfile, const Ref &ARef)
{
	outfile << ARef.Position;
	outfile << ARef.Y;
	outfile << ARef.Z;
	return outfile;
}
HRect::HRect()
{}
HRect::HRect(REAL l, REAL t, REAL r, REAL b )
	: left(l), top(t), right(r), bottom(b)
{}
HRect::HRect(const Point2D &A, const Point2D &B) :
		left(H_MIN(A.x(),B.x())),
		top(H_MAX(A.y(),B.y())),
		right(H_MAX(A.x(),B.x())),
		bottom(H_MIN(A.y(),B.y()))
{}
REAL HRect::GetWidth() const
{
	return right-left;
}
REAL HRect::GetHeight() const
{
	return ABS(bottom-top);
}
bool HRect::IsContained(const Point2D &APoint2D)
{
	return (APoint2D.x() >= left) && (APoint2D.x() <= right) && (APoint2D.y() >= bottom) && (APoint2D.y() <= top);
}
bool HRect::IsContained(const HRect &AHRect)
{
	return (AHRect.left >= left) && (AHRect.right <= right) && (AHRect.bottom >= bottom) && (AHRect.top <= top);
}
void HRect::Scale(REAL factor)
{
	Point2D Center((right+left)/2,(top+bottom)/2);
	REAL width = right-left;
	REAL height = top-bottom;

	width *= factor;
	height *= factor;

	right = Center.x() + width/2;
	left = Center.x() - width/2;
	top = Center.y() + height/2;
	bottom = Center.y() - height/2;
}
void HRect::Move(const Point2D &Vector)
{
	right += Vector.x();
	left += Vector.x();
	top += Vector.y();
	bottom += Vector.y();
}
Point2D HRect::GetCenter() const
{
	return Point2D((left+right)/2,(top+bottom)/2);
}

//CLASS InertRef:
InertRef::InertRef(REAL AMass, REAL ARotInertia) :
	SigmaForce(0,0,0),
	SigmaForceMoment(0,0,0),
	Mass(AMass),
	RotInertia(ARotInertia)
{
	Ref::reset();
}
InertRef::InertRef() {}
void InertRef::Apply(const FixedVector &AForce)
{
	SigmaForce += AForce.Value;
	SigmaForceMoment += (AForce.Position-Position)^(AForce.Value);
}
void InertRef::TimeClick()
{
	Speed += (SigmaForce/Mass)*Ref::TimeIncrement;
	Momentum += (SigmaForceMoment/RotInertia)*Ref::TimeIncrement;
	SigmaForce.reset();
	SigmaForceMoment.reset();
	Position += Speed*Ref::TimeIncrement;
	Rotate(Momentum*Ref::TimeIncrement);
}

//CLASS FixedVector
FixedVector::FixedVector() {}
FixedVector::FixedVector(const Point3D &APosition, const Point3D &AValue) :
	Position(APosition),
	Value(AValue)
{}
void FixedVector::reset()
{
	Position.reset();
	Value.reset();
}
Contact::Contact() : Found(false) {}

REAL RaiseTo (REAL x, int n)
{
	REAL tmp = 1;
	for (int i=1; i<=n ; i++)
		tmp = x*tmp;
	return tmp;
}


unsigned int LargestPowOf2(unsigned int x)
{
	unsigned int tmp = 1;
	while((tmp<<1) <= x)
		tmp = tmp << 1;
	return tmp;
}

