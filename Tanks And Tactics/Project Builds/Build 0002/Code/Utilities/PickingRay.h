////////////////////////////////////////////////////////////////////////////////
//
//	Name:			PickingRay.h
//
//	Description:	This file contains a calculation function
//					getting a picking ray, given information
//					from the camera and the click position on
//					the screen.
//
//	Created:		January 22nd, 2009
//
//	Last Modified:	January 22nd, 2009
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_PICKINGRAY_
#define _DB_PICKINGRAY_

#include <SDL/SDL_opengl.h>
#include "./../Global.h"
#include "./vec3.h"
#include "./OpenGLFrame.h"

vec3f CalculatePickingRay(unsigned int ScreenX, unsigned int ScreenY, COpenGLFrame& Cam);

#endif