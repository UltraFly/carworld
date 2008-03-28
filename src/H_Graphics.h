
#ifndef __H_GRAPHICS_H_
#define __H_GRAPHICS_H_

namespace std {}
using namespace std;

#include "H_Standard.h"

#include <map>
#include <string>

#include "H_Main.h"
#include "H_Geometry.h"
#include "H_Texture.h"

//font caracteristics
#define FONT_WIDTH 8
#define FONT_HEIGHT 13

//default camera values:
#define FIELD_OF_VIEW 90.0f
#define NEAR_PLANE	1.0f //near clipping plane at 1meter
#define FAR_PLANE	500.0f //far plane at 500m

//default values for variables
#define HGL_SHADOWS_DEFAULT						2
#define HGL_TEXTURE_MODE_DEFAULT				Hgl::LINEAR
#define HGL_VERTEX_ARRAYS_DEFAULT				true
#define HGL_EXT_COMPILED_VERTEX_ARRAYS_DEFAULT	true
#define HGL_FINISH_DEFAULT						false

//gl_list macros
#define HGL_DELETE_LIST(x) {if ((x)!=0) {glDeleteLists((x),1); (x) = 0;}}
#define HGL_NEW_LIST(x) {HGL_DELETE_LIST(x); (x) = glGenLists(1); glNewList((x), GL_COMPILE);}

//texture macros
#define HGL_DELETE_TEXTURE(x) {if ((x)!=0) {glDeleteTextures(1,&(x)); (x) = 0;}}

//CLASS Color
typedef Point<3,GLfloat> Color;

extern const Color Black;
extern const Color White;
extern const Color Grey;


//Hgl primitives:
//================
class Hgl
{
public:
	Hgl(HWindow *w);
	void MakeCurrent();
	static void SwapBuffers();
	static Hgl &GetCurrent();
	static map<string,Texture::Reference> &GetLoadedTextures();
public:
	enum Enum {
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
		LINEAR = GL_LINEAR,
		NEAREST = GL_NEAREST,
		NONE = GL_NONE,
		INVALID_ENUM = GL_INVALID_ENUM,
		SHADOW
	};
public:
	//small functions for use in critical loops are inlined:
	static inline void Normal(const Point<3,GLfloat> &APoint)
		{glNormal3fv((GLfloat*)&APoint);}
	static inline void Vertex(const Point<3,GLfloat> &APoint)
		{glVertex3fv((GLfloat*)&APoint);}
	static inline void Vertex(const Point<2,GLfloat> &APoint)
		{glVertex2fv((GLfloat*)&APoint);}
	static inline void TexCoord(const Point<2,GLfloat> &APoint)
		{glTexCoord2fv((GLfloat*)&APoint);}
	static inline void SetColor(const Color &AColor)
		{glColor3f(AColor.x(), AColor.y(), AColor.z());}
	static void ClearColor(const Color &AColor);
	static void Translate(const Point<3,GLfloat> &Location);	//translate to Location
	static void Rotate(REAL teta, const Point<3,GLfloat> &Axe);	//rotate in rad around axe
	static inline void Scale(REAL scale)
		{glScalef(scale,scale,scale);}
	static void LockArrays(int first, int count);
	static void UnlockArrays();
	static inline void Begin(int type)
		{glBegin((GLenum)type);}
	static inline void End()
		{glEnd();}
	static void Finish();
	static ostream &PrintVersion(ostream &out);
	static ostream &PrintExtentions(ostream &out);
	static ostream &PrintDebug(ostream &out);
	static void ResizeWindow(HRect &ClientRect);
	static HRect GetViewPort();
	static void WriteText(const char* AText, const Point<2,GLfloat> &APosition);
	static void WriteText(const char* AText, const Point<3,GLfloat> &APosition);
	static void LookFrom(const Ref & ARef);
	//static void Relocate(const Point<3,GLfloat> &Location, const Point<3,GLfloat> &Direction, const Point<3,GLfloat> &Up);
	//Relocate the following way:
	//Move to Location, Point in Direction
	//the up orientation is given by Up
	static void Relocate(const Ref &Position);
	static void SetLightSource(Point<3,GLfloat> LightDirection);
	static void Viewport(const HRect &ClientRect);
	static void ShadowTransform(const Point<3,GLfloat> &ALightPos, Point<3,GLfloat> APlane[3]);
	static void ShadowTransform(const Point<3,GLfloat> &ALightPos, const Contact &ShadowContact);
	static void PurgeError();
	static void ThrowError();

	static void Enable(Enum variable);
	static void Disable(Enum variable);
	static bool IsEnabled(Enum variable);

//Variable set/get methods:
	static void SetShadows(int value);
	static int GetShadows();

	static void SetTextureMode(Enum value);
	static Enum GetTextureMode();

	static void SetVertexArrays(bool value);
	static bool GetVertexArrays();

	static void SetExtCompiledVertexArrays(bool value);
	static bool GetExtCompiledVertexArrays();

	static void SetFinish(bool value);
	static bool GetFinish();
private:
	static bool &GetCapEnable(Enum variable);
private:
//the window this object is associated with:
	HWindow *m_window;
//pointers to extention functions
	PFNGLLOCKARRAYSEXTPROC glLockArrays;
	PFNGLUNLOCKARRAYSEXTPROC glUnlockArrays;
//members
	static Hgl *curr;
	map<string,Texture::Reference> LoadedTextures;
	GLuint fontOffset;
	HRect MyViewPort;
	bool Shadow;
	Command GLExtentions;
//variables
	int		gl_shadows;
	Enum	gl_texturemode;
	bool	gl_vertex_arrays;
	bool	gl_ext_compiled_vertex_array;
	bool	gl_finish;
};

const char* ToStr(Hgl::Enum val);
Hgl::Enum ToHglEnum(const char* str);

istream &operator>> (istream &in, Hgl::Enum &A);
ostream &operator<< (ostream &out, const Hgl::Enum &A);

#endif //__H_GRAPHICS_H_
