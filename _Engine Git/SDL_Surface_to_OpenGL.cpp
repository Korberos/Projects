/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       SDL_Surface_to_OpenGL.cpp                        */
/*                                                               */
/*  Purpose:    This file contains functionality to convert an   */
/*              SDL surface into an OpenGL surface which, when   */
/*              utilized, will produce faster rendering service. */
/*                                                               */
/*  Created:    12/18/2008                                       */
/*  Last Edit:  12/18/2008                                       */
/*****************************************************************/

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
	const char* error = NULL;

	// new dimensions
	int w = surface->w;
	int h = surface->h;

	SDL_PixelFormat *pixf = surface->format;
	SDL_Surface *image = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32, BMASK, GMASK, RMASK, AMASK );

	error = SDL_GetError();
	ASSERT( strlen( error ) == 0 );

	SDL_SetSurfaceAlphaMod(surface, 0);
	SDL_BlitSurface( surface, NULL, image, NULL );

	error = SDL_GetError();
	ASSERT( strlen( error ) == 0 );

	GLuint txid;
	glGenTextures( 1, &txid );

	GL_MAX_TEXTURE_SIZE;
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