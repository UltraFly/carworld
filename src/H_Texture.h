
#ifndef __H_TEXTURE_H_
#define __H_TEXTURE_H_

namespace std {}
using namespace std;

#include <GL/gl.h>
#include "H_Geometry.h"

#include <map>
#include <string>

//This is the public interface for textures:
/*
class Texture
{
public:
    Texture(const char *name);
	void Use();
	static void SetRenderMode(unsigned int NewRenderMode);
    ~Texture();
};
*/

//the Reference subclass is intended to eliminate multiply
//opened image files, thus saving video memory.
class Texture
{
private:
	class Reference
	{
	public:
		static Reference* GetReference(const char *FileName);
		static void SetRenderMode(unsigned int NewRenderMode);
		void Use();
		void UnReference();
		~Reference();

		Reference();
		void LoadImage(const char *FileName);
		void ApplyMode(unsigned int NewTextureMode);
	private:
		static map<string,Reference> &LoadedTextures();
		string Name;
		unsigned int ReferenceCount;
		GLuint TexIndex;

		friend class Texture;
	};
public:
    Texture(const char *name);
	string Title();
	void Use();
	static void SetRenderMode(unsigned int NewRenderMode);
    ~Texture();
private:
	Reference *MyReference;
};

#endif //__H_PICTURE_H_
