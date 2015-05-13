#include "PointInTriangleCollision.h"

bool PointInTriangleCollision(vec3f Vertex_1, vec3f Vertex_2, vec3f Vertex_3, vec3f Normal, vec3f TestPoint)
{
		vec3f Edge_1 = Vertex_2 - Vertex_1;
		vec3f Edge_2 = Vertex_3 - Vertex_2;
		vec3f Edge_3 = Vertex_1 - Vertex_3;

		vec3f Normal_1;
		cross_product(Normal_1, Edge_1, Normal);
		Normal_1.normalize();

		vec3f Normal_2;
		cross_product(Normal_2, Edge_2, Normal);
		Normal_2.normalize();

		vec3f Normal_3;
		cross_product(Normal_3, Edge_3, Normal);
		Normal_3.normalize();

		vec3f TestVector_1 = TestPoint - Vertex_1;
		vec3f TestVector_2 = TestPoint - Vertex_2;
		vec3f TestVector_3 = TestPoint - Vertex_3;

		if (dot_product(TestVector_1, Normal_1) > 0) return false;
		if (dot_product(TestVector_2, Normal_2) > 0) return false;
		if (dot_product(TestVector_3, Normal_3) > 0) return false;

		return true;
}