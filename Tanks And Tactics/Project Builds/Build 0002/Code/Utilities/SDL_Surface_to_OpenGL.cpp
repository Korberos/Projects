////////////////////////////////////////////////////////////////////////////////
//
//	Name:			SDL_Surface_to_OpenGL.h
//
//	Description:	This file contains a conversion function for
//					taking an SDL_Surface and making an OpenGL
//					surface which, when used, will process faster
//
//	Created:		December 18th, 2008
//
//	Last Modified:	December 18th, 2008
//
////////////////////////////////////////////////////////////////////////////////
#include "SDL_Surface_to_OpenGL.h"

int convert_surface( SDL_Surface *surface )
{
    // new dimensions
    int w = surface->w;
    int h = surface->h;

    SDL_PixelFormat *pixf = SDL_GetVideoSurface()->format;
	SDL_Surface *image = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32, BMASK, GMASK, RMASK, AMASK );
	
	SDL_SetAlpha(surface, 0, 0);
    SDL_BlitSurface( surface, NULL, image, NULL );

    GLuint txid;
    glGenTextures( 1, &txid );
    glBindTexture( GL_TEXTURE_2D, txid );

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (surface->pitch / surface->w == 4)
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