
#include "H_Standard.h"
#include "H_Graphics.h"
#include "H_Texture.h"
#include "H_Image.h"

//CLASS Texture
Texture::Texture(const char *FileName) : MyReference(Reference::GetReference(FileName))
{}

string Texture::Title()
{
	return TitleOf(MyReference->Name);
}

void Texture::Use()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, MyReference->TexIndex);
}

void Texture::SetRenderMode(unsigned int NewRenderMode)
{
	Reference::SetRenderMode(NewRenderMode);
}

Texture::~Texture()
{
	MyReference->UnReference();
}

//CLASS Texture::Reference
map<string,Texture::Reference> &Texture::Reference::LoadedTextures()
{
	return Hgl::GetLoadedTextures();
}

void Texture::Reference::SetRenderMode(unsigned int NewTextureMode)
{
	for (map<string,Reference>::iterator I = LoadedTextures().begin() ; I != LoadedTextures().end() ; I++)
		(*I).second.ApplyMode(NewTextureMode);
}

//if this texture file is already referenced, return a pointer to
//that reference, else create a new reference and return a pointer to it
Texture::Reference* Texture::Reference::GetReference(const char *FileName)
{
	Reference &ref = (LoadedTextures())[FileName];
	if (ref.TexIndex != 0) //the image is already loaded
		ref.ReferenceCount++;
	else //it neads to be loaded
		ref.LoadImage(FileName);
	return &ref;
}

void Texture::Reference::ApplyMode(unsigned int NewTextureMode)
{
	glBindTexture(GL_TEXTURE_2D, TexIndex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	switch (NewTextureMode)
	{
	case GL_NONE :
		glDisable(GL_TEXTURE_2D);
		break;
	case GL_NEAREST :
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		break;
	case GL_LINEAR :
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	case GL_LINEAR_MIPMAP_LINEAR :
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	}
}

void Texture::Reference::UnReference()
{
	ReferenceCount--;
	if (ReferenceCount <= 0) //if the reference count is null, remove it
		LoadedTextures().erase(Name.c_str());
}

Texture::Reference::Reference() :
	ReferenceCount(1),
	TexIndex(0)
{}

void Texture::Reference::LoadImage(const char *FileName)
{
	Name = FileName;

	int GLWidth, GLHeight;
	unsigned char *GLData = NULL;

	//cout << "    " << FileName << "\n";

	H_ReadImage(FileName,GLData,GLWidth,GLHeight);
	
//to do with textures:
	glGenTextures (1, &TexIndex);
	ApplyMode(Hgl::GetTextureMode());
	/*my_gluBuild2DMipmaps(
		(GLsizei)GLWidth, (GLsizei)GLHeight, (GLubyte*)GLData
	);*/
	/*gluBuild2DMipmaps(
		GL_TEXTURE_2D, 3, (GLsizei)GLWidth, (GLsizei)GLHeight, GL_RGB,
		GL_UNSIGNED_BYTE, (GLubyte*)GLData
	);*/
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		GLWidth, GLHeight,
		0 , GL_RGB, GL_UNSIGNED_BYTE,
		GLData);
	delete [] GLData;
}

Texture::Reference::~Reference()
{
	//cout << "deleting reference to " << (char*)Name << "\n";
	HGL_DELETE_TEXTURE(TexIndex);
}
