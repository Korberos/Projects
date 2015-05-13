////////////////////////////////////////////////////////////////////////////////
//
//	Name:			LineToSphere.cpp
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

#include "LineToSphere.h"

bool LineToSphereCollision(vec3f o, vec3f d, vec3f s, float r)
{
	vec3f CamToSphere = o - s;
	
	float a = dot_product(d, d);
	float b = dot_product(d, CamToSphere) * 2.0f;
	float c = dot_product(CamToSphere, CamToSphere) - (r * r);
	
	float discriminant = b * b - 4.0f * a * c;
	
	if (discriminant < 0) return false;
	
	return true;
}