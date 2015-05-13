////////////////////////////////////////////////////////////////////////////////
//
//	Name:			LineToTriangle.h
//
//	Description:	This file contains a collision function for
//					checking a 3D line to a arbitrarily placed
//					triangle
//
//	Created:		December 27th, 2008
//
//	Last Modified:	December 27th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_LINETOTRIANGLE_
#define _DB_LINETOTRIANGLE_

#include "vec3.h"

struct tL2T
{
public:
	tL2T(vec3f Pos0, vec3f Pos1, vec3f Pos2, vec3f* Nor = 0);
	tL2T(void);

	void CalculateNormal(void);

	vec3f	Position[3];
	vec3f	Normal;
};

bool LineToTriangle(vec3f& Intersect, tL2T Triangle, vec3f vStart, vec3f vEnd);

#endif