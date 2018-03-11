#include "../Vector3.h"

struct BasicTriangle
{
	BasicTriangle( Vector3<float> p1, Vector3<float> p2, Vector3<float> p3 )
	{
		Points[0] = p1;
		Points[1] = p2;
		Points[2] = p3;

		Calculate_Normal();
	}
	Vector3<float> Points[3];
	Vector3<float> Normal;

	void Calculate_Normal( void )
	{
		Vector3<float>::Cross_Product( Normal, (Points[1] - Points[0]), (Points[2] - Points[0]) );
		Normal.Normalize();
	}
};

bool LineToTriangle(Vector3<float>& intersection, BasicTriangle& triangle, const Vector3<float>& start, const Vector3<float>& end);