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

#ifndef _SDL_SURFACE_TO_OPENGL_
#define _SDL_SURFACE_TO_OPENGL_

struct SDL_Surface;
int convert_surface( SDL_Surface *surface );

#endif