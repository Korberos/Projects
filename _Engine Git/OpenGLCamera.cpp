/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       OpenGLCamera.cpp                                 */
/*                                                               */
/*  Purpose:    This file contains a class that will act as a    */
/*              basic camera in an OpenGL environment. There     */
/*              can be multiple instances and they are easily    */
/*              manageable, as each is self-contained.           */
/*                                                               */
/*  Created:    01/27/2009                                       */
/*  Last Edit:  01/27/2009                                       */
/*****************************************************************/

#include "OpenGLCamera.h"

COpenGLFrame::COpenGLFrame(void)
{
	vOrigin		= Vector3<float>(0.0f, 0.0f, 0.0f);
	vUp			= Vector3<float>(0.0f, 1.0f, 0.0f);
	vForward		= Vector3<float>(0.0f, 0.0f, -1.0f);
}

void COpenGLFrame::GetMatrix(float* Matrix, bool bRotationOnly)
{
	Vector3<float> vRight = GetRight();

	Matrix[0] = vRight.x;
	Matrix[1] = vRight.y;
	Matrix[2] = vRight.z;
	Matrix[3] = 0.0f;
	Matrix[4] = vUp.x;
	Matrix[5] = vUp.y;
	Matrix[6] = vUp.z;
	Matrix[7] = 0.0f;     
	Matrix[8] = vForward.x;
	Matrix[9] = vForward.y;
	Matrix[10] = vForward.z;
	Matrix[11] = 0.0f;
	Matrix[12] = ((bRotationOnly == true) ? 0.0f : vOrigin.x);
	Matrix[13] = ((bRotationOnly == true) ? 0.0f : vOrigin.y);
	Matrix[14] = ((bRotationOnly == true) ? 0.0f : vOrigin.z);
	Matrix[15] = 1.0f;
}


void COpenGLFrame::ApplyActorTransform(bool bRotationOnly)
{
	float RotationMatrix[16];
	GetMatrix(RotationMatrix, bRotationOnly);

	// Apply rotation to the current matrix
	glMultMatrixf(RotationMatrix);
}

// Rotate around local X Axes - Note all rotations are in radians
void COpenGLFrame::RotateLocalX(float fAngle)
{
	float Matrix[16];
	Vector3<float> vRight = GetRight();

	// Just Rotate around the Right vector
	// Create a rotation matrix around my Right (X) vector
	RotateMatrix(Matrix, fAngle, vRight.x, vRight.y, vRight.z);

	Vector3<float> vNewVect;

	// Inline 3x3 matrix multiply for rotation only
	vNewVect.x = Matrix[0] * vForward.x + Matrix[4] * vForward.y + Matrix[8] *  vForward.z;
	vNewVect.y = Matrix[1] * vForward.x + Matrix[5] * vForward.y + Matrix[9] *  vForward.z;
	vNewVect.z = Matrix[2] * vForward.x + Matrix[6] * vForward.y + Matrix[10] * vForward.z;
	vForward = vNewVect;

	// Update pointing up vector
	vNewVect.x = Matrix[0] * vUp.x + Matrix[4] * vUp.y + Matrix[8] *  vUp.z;
	vNewVect.y = Matrix[1] * vUp.x + Matrix[5] * vUp.y + Matrix[9] *  vUp.z;
	vNewVect.z = Matrix[2] * vUp.x + Matrix[6] * vUp.y + Matrix[10] * vUp.z;
	vUp = vNewVect;
}

// Rotate around local Y
void COpenGLFrame::RotateLocalY(float fAngle)
{
	float Matrix[16];

	// Just Rotate around the up vector
	// Create a rotation matrix around my Up (Y) vector
	RotateMatrix(Matrix, fAngle, vUp.x, vUp.y, vUp.z);

	Vector3<float> vNewVect;

    // Rotate forward pointing vector (inlined 3x3 transform)
	vNewVect.x = Matrix[0] * vForward.x + Matrix[4] * vForward.y + Matrix[8] *  vForward.z;
	vNewVect.y = Matrix[1] * vForward.x + Matrix[5] * vForward.y + Matrix[9] *  vForward.z;
	vNewVect.z = Matrix[2] * vForward.x + Matrix[6] * vForward.y + Matrix[10] * vForward.z;
	vForward = vNewVect;
}

// Rotate around local Z
void COpenGLFrame::RotateLocalZ(float fAngle)
{
	float Matrix[16];

	// Only the up vector needs to be rotated
	RotateMatrix(Matrix, fAngle, vForward.x, vForward.y, vForward.z);

	Vector3<float> vNewVect;
	vNewVect.x = Matrix[0] * vUp.x + Matrix[4] * vUp.y + Matrix[8] *  vUp.z;
	vNewVect.y = Matrix[1] * vUp.x + Matrix[5] * vUp.y + Matrix[9] *  vUp.z;
	vNewVect.z = Matrix[2] * vUp.x + Matrix[6] * vUp.y + Matrix[10] * vUp.z;
	vUp = vNewVect;
}

// Reset axes to make sure they are orthonormal. This should be called on occasion
// if the matrix is long-lived and frequently transformed.
void COpenGLFrame::Normalize(void)
{
	Vector3<float> vRight = GetRight();

	// Use the Right vector to recalculate forward vector
	Vector3<float>::Cross_Product(vForward, vRight, vUp);	

	// Also check for unit length...
	vUp.Normalize();
	vForward.Normalize();
}

// Rotate in world coordinates...
void COpenGLFrame::RotateWorld(float fAngle, float x, float y, float z)
{
	float Matrix[16];

	// Create the Rotation matrix
	RotateMatrix(Matrix, fAngle, x, y, z);

	Vector3<float> vNewVect;
	
	// Transform the up axis (inlined 3x3 rotation)
	vNewVect.x = Matrix[0] * vUp.x + Matrix[4] * vUp.y + Matrix[8] *  vUp.z;	
	vNewVect.y = Matrix[1] * vUp.x + Matrix[5] * vUp.y + Matrix[9] *  vUp.z;	
	vNewVect.z = Matrix[2] * vUp.x + Matrix[6] * vUp.y + Matrix[10] * vUp.z;	
	vUp = vNewVect;

	// Transform the forward axis
	vNewVect.x = Matrix[0] * vForward.x + Matrix[4] * vForward.y + Matrix[8] *  vForward.z;	
	vNewVect.y = Matrix[1] * vForward.x + Matrix[5] * vForward.y + Matrix[9] *  vForward.z;	
	vNewVect.z = Matrix[2] * vForward.x + Matrix[6] * vForward.y + Matrix[10] * vForward.z;	
	vForward = vNewVect;
}

void COpenGLFrame::RotateMatrix(float* Matrix, float Angle, float x, float y, float z)
{
	float mag, s, c;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	s = (float)sin(Angle);
	c = (float)cos(Angle);

	mag = (float)sqrt(x * x + y * y + z * z);

	// Identity matrix
	if (mag == 0.0f)
	{
		Matrix[0]	= 1.0f;
		Matrix[1]	= 0.0f;
		Matrix[2]	= 0.0f;
		Matrix[3]	= 0.0f;
		Matrix[4]	= 0.0f;
		Matrix[5]	= 1.0f;
		Matrix[6]	= 0.0f;
		Matrix[7]	= 0.0f;
		Matrix[8]	= 0.0f;
		Matrix[9]	= 0.0f;
		Matrix[10]	= 1.0f;
		Matrix[11]	= 0.0f;
		Matrix[12]	= 0.0f;
		Matrix[13]	= 0.0f;
		Matrix[14]	= 0.0f;
		Matrix[15]	= 1.0f;
		return;
	}

	// Normalize the Rotation Matrix
	x /= mag;
	y /= mag;
	z /= mag;

    #define M(row,col)  Matrix[col*4+row]

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	one_c = 1.0f - c;

	M(0,0) = (one_c * xx) + c;
	M(0,1) = (one_c * xy) - zs;
	M(0,2) = (one_c * zx) + ys;
	M(0,3) = 0.0f;

	M(1,0) = (one_c * xy) + zs;
	M(1,1) = (one_c * yy) + c;
	M(1,2) = (one_c * yz) - xs;
	M(1,3) = 0.0f;

	M(2,0) = (one_c * zx) - ys;
	M(2,1) = (one_c * yz) + xs;
	M(2,2) = (one_c * zz) + c;
	M(2,3) = 0.0f;

	M(3,0) = 0.0f;
	M(3,1) = 0.0f;
	M(3,2) = 0.0f;
	M(3,3) = 1.0f;

    #undef M
}

void COpenGLFrame::Determine_Ray( int mouse_x, int mouse_y, int window_width, int window_height, float fov, float distance, Vector3<float>& start, Vector3<float>& end ) const
{
	float xf = 2.0f * ((float)(mouse_x - (1024/2))) / ((float)1024);
	float yf = 2.0f * -((float)(mouse_y - (768/2))) / ((float)768);

	float MAGIC_NUMBER = 1.298f;
	xf /= MAGIC_NUMBER;
	yf /= MAGIC_NUMBER * float(1024) / float(768);

	Vector3<float> ray_direction( xf, yf, 1.0f );
	ray_direction.Normalize();

	Vector3<float> invert_right( GetRight() * -1.0f );

	Vector3<float> rayDir;
	rayDir.x  = ray_direction.x * invert_right.x + ray_direction.y * vUp.x + ray_direction.z * vForward.x;
	rayDir.y  = ray_direction.x * invert_right.y + ray_direction.y * vUp.y + ray_direction.z * vForward.y;
	rayDir.z  = ray_direction.x * invert_right.z + ray_direction.y * vUp.z + ray_direction.z * vForward.z;
	rayDir.Normalize();

	start = vOrigin;
	end = vOrigin + rayDir * distance;
}