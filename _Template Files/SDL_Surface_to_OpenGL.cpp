#include "SDL_Surface_to_OpenGL.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include "DebugTools.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define RMASK 0xFF000000
    #define GMASK 0x00FF0000
    #define BMASK 0x0000FF00
    #define AMASK 0x000000FF
#else
    #define RMASK 0x000000FF
    #define GMASK 0x0000FF00
    #define BMASK 0x00FF0000
    #define AMASK 0xFF000000
#endif

int convert_surface( SDL_Surface *surface )
{
	char* error = NULL;

	// new dimensions
	int w = surface->w;
	int h = surface->h;

	SDL_PixelFormat *pixf = SDL_GetVideoSurface()->format;
	SDL_Surface *image = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32, BMASK, GMASK, RMASK, AMASK );

	error = SDL_GetError();
	ASSERT( strlen( error ) == 0 );

	SDL_SetAlpha(surface, 0, 0);
	SDL_BlitSurface( surface, NULL, image, NULL );

	error = SDL_GetError();
	ASSERT( strlen( error ) == 0 );

	GLuint txid;
	glGenTextures( 1, &txid );
	glBindTexture( GL_TEXTURE_2D, txid );

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	ENFORCED_IF ( surface->pitch / surface->w == 4 && "All images must be properly saved as 32-bit." )
	{
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
	}
	else
	{
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
	}

	SDL_FreeSurface( image );
	return txid;
}