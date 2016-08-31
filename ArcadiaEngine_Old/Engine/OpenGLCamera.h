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
/*  File:       OpenGLCamera.h                                   */
/*                                                               */
/*  Purpose:    This file contains a class that will act as a    */
/*              basic camera in an OpenGL environment. There     */
/*              can be multiple instances and they are easily    */
/*              manageable, as each is self-contained.           */
/*                                                               */
/*  Created:    01/27/2009                                       */
/*  Last Edit:  01/27/2009                                       */
/*****************************************************************/

#ifndef _DB_OPENGLFRAME_
#define _DB_OPENGLFRAME_

#include <math.h>
#include <SDL_opengl.h>
#include "Vector3.h"

class COpenGLFrame
{
protected:
    Vector3<float> vOrigin;
    Vector3<float> vForward;
    Vector3<float> vUp;

public:
	COpenGLFrame(void);

    //  Accessors
	inline	Vector3<float>	GetOrigin(void)								{	return vOrigin;										}
	inline	void	CopyOrigin(Vector3<float>& vPoint)					{	vPoint = vOrigin;									}
	inline	float	GetOriginX(void)											{	return vOrigin.x;									}
	inline	float	GetOriginY(void)											{	return vOrigin.y;									} 
	inline	float	GetOriginZ(void)											{	return vOrigin.z;									}
    inline	Vector3<float>	GetForward(void)								{	return vForward;									}
	inline	void	CopyForward(Vector3<float>& vPoint)					{	vPoint = vForward;									}
    inline	Vector3<float>	GetUp(void)										{	return vUp;											}
	inline	void	CopyUp(Vector3<float>& vPoint)						{	vPoint = vUp;										}
	inline	void	CopyZAxis(Vector3<float>& vVector)					{	CopyForward(vVector);								}
	inline	void	CopyYAxis(Vector3<float>& vVector)					{	CopyUp(vVector);									}
	inline	void	CopyXAxis(Vector3<float>& vVector)					{	Vector3<float>::Cross_Product(vVector, vUp, vForward);		}
	inline	Vector3<float>	GetRight(void) const							{	Vector3<float> A; Vector3<float>::Cross_Product(A, vUp, vForward); return A;	}

	//  Modifiers
	inline	void	SetOrigin(const Vector3<float> vPoint)				{	vOrigin = vPoint;									}
	inline	void	SetOrigin(float x, float y, float z)				{	vOrigin = Vector3<float>(x, y, z);							}
	inline	void	SetForward(const Vector3<float> vPoint)			{	vForward = vPoint;									}
	inline	void	SetForward(float x, float y, float z)				{	vForward = Vector3<float>(x, y, z);							}
	inline	void	SetUp(const Vector3<float> vPoint)					{	vUp = vPoint;										}
	inline	void	SetUp(float x, float y, float z)						{	vUp = Vector3<float>(x, y, z);								}
    inline	void	TranslateWorld(float x, float y, float z)			{	vOrigin += Vector3<float>(x, y, z);							}
	inline	void	TranslateLocal(float x, float y, float z)			{	MoveForward(z); MoveUp(y); MoveRight(x);			}
	inline	void	MoveForward(float fDelta)								{	vOrigin += vForward * fDelta;						}
	inline	void	MoveUp(float fDelta)										{	vOrigin += vUp * fDelta;							}
	inline	void	MoveRight(float fDelta)									{	vOrigin += GetRight() * fDelta;						}
	inline	void	ApplyCameraTransform(bool bRotationOnly = false)    
	{
		// Create a 4x4 matrix
		Vector3<float> X, Z;

		// Make rotation matrix
		// Z vector is reversed
		Z = vForward * -1.0f;
		
		// X vector = Y cross Z
		Vector3<float>::Cross_Product(X, vUp, Z);

		// Define the matrix
		float matrix[16] = 
		{
			X.x,		vUp.x,		Z.x,		0.0f,
			X.y,		vUp.y,		Z.y,		0.0f,
			X.z,		vUp.z,		Z.z,		0.0f,
			0.0f,		0.0f,			0.0f,		1.0f
		};

		// Camera Transform   
		glMultMatrixf( matrix );
	
		// If Rotation only, then do not do the translation
		if (bRotationOnly == false) glTranslatef(-vOrigin.x, -vOrigin.y, -vOrigin.z);
	}

	//  General Functions
	void GetMatrix(float* Matrix, bool bRotationOnly = false);
	void ApplyActorTransform(bool bRotationOnly = false);
	void RotateLocalX(float fAngle);
	void RotateLocalY(float fAngle);
	void RotateLocalZ(float fAngle);
	void RotateWorld(float fAngle, float x, float y, float z);
	void Normalize(void);
	void RotateMatrix(float* Matrix, float Angle, float x, float y, float z);
	void Determine_Ray( int mouse_x, int mouse_y, int window_width, int window_height, float fov, float far, Vector3<float>& start, Vector3<float>& end ) const;
};


#endif