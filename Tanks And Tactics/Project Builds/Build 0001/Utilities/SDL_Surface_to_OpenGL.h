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

#ifndef _DB_SDLSURFACETOOPENGL_
#define _DB_SDLSURFACETOOPENGL_

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

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

int convert_surface(SDL_Surface* surface);

#endif