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
/*  File:       SDL_Surface_to_OpenGL.h                          */
/*                                                               */
/*  Purpose:    This file contains functionality to convert an   */
/*              SDL surface into an OpenGL surface which, when   */
/*              utilized, will produce faster rendering service. */
/*                                                               */
/*  Created:    12/18/2008                                       */
/*  Last Edit:  12/18/2008                                       */
/*****************************************************************/

#ifndef _SDL_SURFACE_TO_OPENGL_
#define _SDL_SURFACE_TO_OPENGL_

struct SDL_Surface;
int convert_surface( SDL_Surface *surface );

#endif