#include "LineSegmentTriangleCheck.h"

#include "PointInTriangleCollision.h"

bool LineSegmentTriangleCheck(vec3f &CollisionPoint, int &TriangleIndex, MapController* Map, const vec3f &LineStart, const vec3f &LineEnd)
{
	int				VertexCount		= Map->Get_VertexCount();
	MC_Vertex*		Vertices		= Map->Get_Vertices();

	int				GeometryCount	= Map->Get_GeometryCount();
	MC_Geometry*	Geometry		= Map->Get_Geometry();

	//~	Step 1
	// Create a vector called _LineNormal and make it point from the start of the
	// line to the end of the line, then set _LineNormal to the length of one.
	vec3f _LineNormal = LineEnd - LineStart;
	_LineNormal.normalize();

	for (int i = 0; i < GeometryCount; i += 1)
	{
		// Take the dot product of the line segment's start point and the triangle's
		// normal and store it in _StartOffSet;
		float _StartOffSet = dot_product(LineStart, Vertices[Geometry[i].VertexID[0]].Normal);

		// Take the dot product of _LineNormal and the triangle's normal and store it
		// in _U.
		float _U = dot_product(_LineNormal, Vertices[Geometry[i].VertexID[0]].Normal);

		// Take the dot product of the the triangle's normal and one of the triangle's
		//vertices and store it in _PlaneOffSet
		float _PlaneOffSet = dot_product(Vertices[Geometry[i].VertexID[0]].Normal, Vertices[Geometry[i].VertexID[0]].Position);

		// Subtract _StartOffSet from _PlaneOffSet, then divide it by U and store that
		// in _Distance
		float _Distance = (_PlaneOffSet - _StartOffSet) / _U;

		// If the distance is less than 0 or greater than the length of the line, stop
		// testing
		if (_Distance < 0.0f) continue;
		if (_Distance > (LineEnd - LineStart).magnitude()) continue;

		// Scale _LineNormal by _Distance.
		_LineNormal *= _Distance;

		// Add _LineNormal to the line segment's start point to get the project point
		// to the triangle's plane
		vec3f _ProjectPoint = LineStart + _LineNormal;

		// Use the Point-In-Triangle Collision Test to see if the projected point is
		// in the triangle
		if (PointInTriangleCollision(Vertices[Geometry[i].VertexID[0]].Position, Vertices[Geometry[i].VertexID[1]].Position, Vertices[Geometry[i].VertexID[2]].Position, Vertices[Geometry[i].VertexID[0]].Normal, _ProjectPoint) == false) continue;

		// If you have gotten this far, you have the triangle you are looking for. Set
		// the output values and return true
		TriangleIndex = i;
		CollisionPoint = _ProjectPoint;
		return true;
	}

	// If the entire loop goes without finding a triangle, no triangle fits your line,
	// so return false
	return false;
}