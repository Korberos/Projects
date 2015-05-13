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
//	Last Modified:	December 18th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_POINTTOCYLINDER_
#define _DB_POINTTOCYLINDER_

#include "./../Utilities/vec3.h"

bool Point_Cylinder_Collision(vec3f vCylinder1, vec3f vCylinder2, float Radius, vec3f vPoint)
{
	vec3f CylinderLine;
	vec3f PointLine;
	float DotProduct;
	float Length;
	float Distance;
	
	// Gather needed information generated from the incoming data
	CylinderLine	= vCylinder2 - vCylinder1;
	PointLine		= vPoint - vCylinder1;
	Length			= ((vCylinder2.x - vCylinder1.x) * (vCylinder2.x - vCylinder1.x)) + ((vCylinder2.y - vCylinder1.y) * (vCylinder2.y - vCylinder1.y)) + ((vCylinder2.z - vCylinder1.z) * (vCylinder2.z - vCylinder1.z));

	// Create the Dot Product of CylinderLine and PointLine
	DotProduct = PointLine.x * CylinderLine.x + PointLine.y * CylinderLine.y + PointLine.z * CylinderLine.z;

	// If the Dot Product is less than 0, the point is behind the Cylinder's first point, and
	// if the Dot Product is greater than the length between the Begin and End points, the point
	// is beyond the Cylinder's second point.
	if ((DotProduct < 0.0f) || (DotProduct > Length)) return false;
	else
	{
		// If the length between the point and the closest part of the line is
		// greater than the radius of the cylinder, the point is outside the area
		Distance = ((PointLine.x * PointLine.x) + (PointLine.y * PointLine.y) + (PointLine.z * PointLine.z)) - ((DotProduct * DotProduct) / Length);
		if (Distance > (Radius * Radius)) return false;
		return true;
	}
}

bool Point_Cylinder_Collision_IgnoreHeight(vec3f vCylinder1, vec3f vCylinder2, float Radius, vec3f vPoint)
{
	vec3f CylinderLine;
	vec3f PointLine;
	float DotProduct;
	float Length;
	float Distance;
	
	// Gather needed information generated from the incoming data
	CylinderLine	= vCylinder2 - vCylinder1;
	PointLine		= vPoint - vCylinder1;
	Length			= ((vCylinder2.x - vCylinder1.x) * (vCylinder2.x - vCylinder1.x)) + ((vCylinder2.z - vCylinder1.z) * (vCylinder2.z - vCylinder1.z));

	// Create the Dot Product of CylinderLine and PointLine
	DotProduct = PointLine.x * CylinderLine.x + PointLine.z * CylinderLine.z;

	// If the Dot Product is less than 0, the point is behind the Cylinder's first point, and
	// if the Dot Product is greater than the length between the Begin and End points, the point
	// is beyond the Cylinder's second point.
	if ((DotProduct < 0.0f) || (DotProduct > Length)) return false;
	else
	{
		// If the length between the point and the closest part of the line is
		// greater than the radius of the cylinder, the point is outside the area
		Distance = ((PointLine.x * PointLine.x) + (PointLine.z * PointLine.z)) - ((DotProduct * DotProduct) / Length);
		if (Distance > (Radius * Radius)) return false;
		return true;
	}
}
#endif