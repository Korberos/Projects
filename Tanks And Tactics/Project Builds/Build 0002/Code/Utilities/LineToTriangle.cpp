////////////////////////////////////////////////////////////////////////////////
//
//	Name:			LineToTriangle.cpp
//
//	Description:	This file contains a collision function for
//					checking a 3D line to a arbitrarily placed
//					triangle
//
//	Created:		December 27th, 2008
//
//	Last Modified:	January 25th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "LineToTriangle.h"

tL2T::tL2T(vec3f Pos0, vec3f Pos1, vec3f Pos2, vec3f* Nor)
{
	Position[0] = Pos0;
	Position[1] = Pos1;
	Position[2] = Pos2;

	if (Nor != 0)
	{
		Normal.x = Nor->x;
		Normal.y = Nor->y;
		Normal.z = Nor->z;
		return;
	}

	CalculateNormal();
}

tL2T::tL2T(void)
{
	// Create a Position
	Position[0].x = 0.0f;
	Position[0].y = 0.0f;
	Position[0].z = 0.0f;
	Position[1].x = 1.0f;
	Position[1].y = 0.0f;
	Position[1].z = 0.0f;
	Position[2].x = 1.0f;
	Position[2].y = 0.0f;
	Position[2].z = 1.0f;

	CalculateNormal();
}

void tL2T::CalculateNormal(void)
{
	// Calculate the Normal
	cross_product(Normal, (Position[1] - Position[0]), (Position[2] - Position[0]));
	Normal.normalize();
}

bool LineToTriangle(vec3f& Intersect, tL2T Triangle, vec3f vStart, vec3f vEnd)
{
	// Create a vector called lineNormal and make it point from the start of the line to the end of the line.
	// Set lineNormal to the length of one.
	vec3f lineNormal = (vEnd - vStart).normalize();

	// Take the dot product of the line’s start point and the triangle’s normal and store it in startOffSet.
	float startOffSet = dot_product(vStart, Triangle.Normal);

	// Take the dot product of lineNormal and the triangle’s normal and store it in U.
	float U = dot_product(lineNormal, Triangle.Normal);

	// Take the dot product of triangle’s normal and one of the triangle’s vertices and store it in planeOffSet.
	float planeOffSet = dot_product(Triangle.Normal, Triangle.Position[0]);

	// Subtract startOffSet from planeOffset, then divide by U and store that in distance.
	float Distance = (planeOffSet - startOffSet) / U;

	// If the distance is less than zero or greater than the length of the line stop testing.
	if ((Distance < 0.0f) || (Distance > (vEnd - vStart).magnitude())) return false;

	// Scale lineNormal by distance.
	lineNormal *= Distance;

	// Add lineNormal to the start of the line to get the project point to the triangle’s plane.
	vec3f projectPoint = vStart + lineNormal;

	// Use the point in triangle collision test to see if the projected point is in the triangle.
	vec3f V0		= Triangle.Position[0];
	vec3f V1		= Triangle.Position[1];
	vec3f V2		= Triangle.Position[2];
	vec3f testP		= projectPoint;
	vec3f edge0		= Triangle.Position[1] - Triangle.Position[0];
	vec3f edge1		= Triangle.Position[2] - Triangle.Position[1];
	vec3f edge2		= Triangle.Position[0] - Triangle.Position[2];

	vec3f normal0;
	cross_product(normal0, edge0, Triangle.Normal);
	normal0.normalize();

	vec3f normal1;
	cross_product(normal1, edge1, Triangle.Normal);
	normal1.normalize();

	vec3f normal2;
	cross_product(normal2, edge2, Triangle.Normal);
	normal2.normalize();

	vec3f testVec0 = testP - V0;
	vec3f testVec1 = testP - V1;
	vec3f testVec2 = testP - V2;

	if (dot_product(testVec0, normal0) > 0) return false;
	if (dot_product(testVec1, normal1) > 0) return false;
	if (dot_product(testVec2, normal2) > 0) return false;

	Intersect = projectPoint;
	return true;

/*
	vec3f Vector;
	float DotProduct;
	float T;

	// Determine the Vector of the intersection line
	Vector = (Bottom - Top).normalize();

	// The DotProduct of the Normal and the Vector
	DotProduct = dot_product(Triangle.Normal, Vector);

	if (DotProduct < 0.0f)
	{
		//Find point of intersect to triangle plane.
		//find t to intersect point
		T = -(Triangle.Normal.x * (Top.x - Triangle.Position[0].x) + Triangle.Normal.y * (Top.y - Triangle.Position[0].y) + Triangle.Normal.z * (Top.z - Triangle.Position[0].z)) / DotProduct;

		// if ds is neg line started past triangle so can't hit triangle.
		if (T < 0.0f) return false;

		(*Intersect) = Top + (Vector * T);

		float DotTest[3];
		vec3f CrossTest;

		cross_product(CrossTest, (Triangle.Position[1] - Triangle.Position[0]), ((*Intersect) - Triangle.Position[0]));
		DotTest[0] = dot_product(CrossTest, Triangle.Normal);
		if (DotTest[0] < 0.0f) return false;
		
		cross_product(CrossTest, (Triangle.Position[2] - Triangle.Position[1]), ((*Intersect) - Triangle.Position[1]));
		DotTest[1] = dot_product(CrossTest, Triangle.Normal);
		if (DotTest[1] < 0.0f) return false;
		
		cross_product(CrossTest, (Triangle.Position[0] - Triangle.Position[2]), ((*Intersect) - Triangle.Position[2]));
		DotTest[2] = dot_product(CrossTest, Triangle.Normal);
		if (DotTest[2] < 0.0f) return false;

		return true;
	}

	return false;
*/
}