
#include "H_Standard.h"
#include "H_Graphics.h"
#include "H_Texture.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

//CLASS Texture
Texture::Texture(const char *FileName) : MyReference(Reference::GetReference(FileName))
{}

string Texture::Title()
{
	return TitleOf(MyReference->Name);
}

void Texture::Use()
{
	Hgl::SetColor(Color(1,1,1));
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
{

}

bool IsPowerOfTwo(int n)
{
	return (n & (n - 1))==0;
}

void Texture::Reference::LoadImage(const char *FileName)
{
// This surface will tell us the details of the image
	SDL_Surface* surface = IMG_Load(FileName);	
	if (surface!=NULL)
	{ 
	// Check that the image's width is a power of 2
		if (!IsPowerOfTwo(surface->w))
		{
			cout << "warning: image.bmp's width is not a power of 2" << endl;
		}
		
	// Also check if the height is a power of 2
		if (!IsPowerOfTwo(surface->h))
		{
			cout << "warning: image.bmp's height is not a power of 2" << endl;
		}
	 
	// get the number of channels in the SDL surface
		const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surface->format);
		if (format==NULL)
		{
			SDL_DestroySurface(surface);
			throw HException(string("SDL could not inspect : \"")+FileName+ "\"." + string(SDL_GetError()));
		}

		GLint nOfColors = format->bytes_per_pixel;
		GLenum texture_format;
		switch (nOfColors)
		{
		case 4:     // contains an alpha channel
			{
				if (format->Rmask == 0x000000ff)
						texture_format = GL_RGBA;
				else
						texture_format = GL_BGRA;
			}
			break;
		case 3:     // no alpha channel
			{
				if (format->Rmask == 0x000000ff)
						texture_format = GL_RGB;
				else
						texture_format = GL_BGR;
			}
			break;
		default:
			SDL_DestroySurface(surface);
			throw HException(string("The image : \"")+FileName+("\" is not truecolor."));
		}
	        
		// Have OpenGL generate a TexIndex object handle for us
		glGenTextures( 1, &TexIndex );
	 
		// Bind the TexIndex object
		glBindTexture( GL_TEXTURE_2D, TexIndex );

		// Edit the TexIndex object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
						  texture_format, GL_UNSIGNED_BYTE, surface->pixels );
		
		cout << "opened "<< surface->w << "x" << surface->h <<" image file: \"" << FileName << "\"" << endl;
		SDL_DestroySurface(surface);

		ApplyMode(Hgl::GetCurrent().GetTextureMode());
	} 
	else
	{
		throw HException(string("SDL could not load : \"")+FileName+ "\"." + string(SDL_GetError()));
	}    
}

Texture::Reference::~Reference()
{
	//cout << "deleting reference to " << (char*)Name << "\n";
	HGL_DELETE_TEXTURE(TexIndex);
}
