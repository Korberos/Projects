#include "LineToTriangle.h"

bool LineToTriangle(Vector3<float>& intersection, BasicTriangle& triangle, const Vector3<float>& start, const Vector3<float>& end)
{
	// Create a vector called lineNormal and make it point from the start of the line to the end of the line.
	// Set lineNormal to the length of one.
	Vector3<float> lineNormal = (end - start).Normalize();

	// Take the dot product of the line’s start point and the triangle’s normal and store it in startOffSet.
	float startOffSet = Vector3<float>::Dot_Product(start, triangle.Normal);

	// Take the dot product of lineNormal and the triangle’s normal and store it in U.
	float U = Vector3<float>::Dot_Product(lineNormal, triangle.Normal);

	// Take the dot product of triangle’s normal and one of the triangle’s vertices and store it in planeOffSet.
	float planeOffSet = Vector3<float>::Dot_Product(triangle.Normal, triangle.Points[0]);

	// Subtract startOffSet from planeOffset, then divide by U and store that in distance.
	float Distance = (planeOffSet - startOffSet) / U;

	// If the distance is less than zero or greater than the length of the line stop testing.
	if ((Distance < 0.0f) || (Distance > (end - start).Magnitude())) return false;

	// Scale lineNormal by distance.
	lineNormal *= Distance;

	// Add lineNormal to the start of the line to get the project point to the triangle’s plane.
	Vector3<float> projectPoint = start + lineNormal;

	// Use the point in triangle collision test to see if the projected point is in the triangle.
	Vector3<float> V0		= triangle.Points[0];
	Vector3<float> V1		= triangle.Points[1];
	Vector3<float> V2		= triangle.Points[2];
	Vector3<float> testP		= projectPoint;
	Vector3<float> edge0		= triangle.Points[1] - triangle.Points[0];
	Vector3<float> edge1		= triangle.Points[2] - triangle.Points[1];
	Vector3<float> edge2		= triangle.Points[0] - triangle.Points[2];

	Vector3<float> normal0;
	Vector3<float>::Cross_Product(normal0, edge0, triangle.Normal);
	normal0.Normalize();

	Vector3<float> normal1;
	Vector3<float>::Cross_Product(normal1, edge1, triangle.Normal);
	normal1.Normalize();

	Vector3<float> normal2;
	Vector3<float>::Cross_Product(normal2, edge2, triangle.Normal);
	normal2.Normalize();

	Vector3<float> testVec0 = testP - V0;
	Vector3<float> testVec1 = testP - V1;
	Vector3<float> testVec2 = testP - V2;

	if (Vector3<float>::Dot_Product(testVec0, normal0) > 0) return false;
	if (Vector3<float>::Dot_Product(testVec1, normal1) > 0) return false;
	if (Vector3<float>::Dot_Product(testVec2, normal2) > 0) return false;

	intersection = projectPoint;
	return true;
}