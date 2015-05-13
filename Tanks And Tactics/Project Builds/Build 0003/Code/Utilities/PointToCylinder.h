////////////////////////////////////////////////////////////////////////////////
//
//	Name:			PointToCylinder.h
//
//	Description:	This file contains a collision function for
//					checking a 3D point to a arbitrarily placed
//					cylinder
//
//	Created:		December 18th, 2008
//
//	Last Modified:	January 31st, 2009
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_POINTTOCYLINDER_
#define _DB_POINTTOCYLINDER_

#include "./../Utilities/vec3.h"

bool Point_Cylinder_Collision(vec3f vCylinder1, vec3f vCylinder2, float Radius, vec3f vPoint);
bool Point_Cylinder_Collision_IgnoreHeight(vec3f vCylinder1, vec3f vCylinder2, float Radius, vec3f vPoint);

#endif