////////////////////////////////////////////////////////////////////////////////
//
//	Name:			LineToSphere.h
//
//	Description:	This file contains a collision function for
//					checking a 3D line to a arbitrarily placed
//					sphere
//
//	Created:		January 28th, 2009
//
//	Last Modified:	January 28th, 2009
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_LINETOSPHERE_
#define _DB_LINETOSPHERE_

#include <math.h>
#include "./vec3.h"

bool LineToSphereCollision(vec3f o, vec3f d, vec3f s, float r);

#endif