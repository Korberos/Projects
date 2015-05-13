////////////////////////////////////////////////////////////////////////////////
//
//	Name:			OpenGLFrame.h
//
//	Description:	This file contains a basic frame class for
//					use with OpenGL. The class is a vec3f copy
//					of the GLFrame class created by Richard
//					S. Wright Jr. with some functionality slightly
//					altered to better fit my needs
//
//	Created:		January 27th, 2009
//
//	Last Modified:	January 27th, 2009
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_OPENGLFRAME_
#define _DB_OPENGLFRAME_

#include <math.h>
#include <SDL_opengl.h>
#include "./vec3.h"

class COpenGLFrame
{
protected:
    vec3f vOrigin;
    vec3f vForward;
    vec3f vUp;

public:
	COpenGLFrame(void);

    //  Accessors
	inline	vec3f	GetOrigin(void)								{	return vOrigin;										}
	inline	void	CopyOrigin(vec3f& vPoint)					{	vPoint = vOrigin;									}
	inline	float	GetOriginX(void)							{	return vOrigin.x;									}
	inline	float	GetOriginY(void)							{	return vOrigin.y;									} 
	inline	float	GetOriginZ(void)							{	return vOrigin.z;									}
    inline	vec3f	GetForward(void)							{	return vForward;									}
	inline	void	CopyForward(vec3f& vPoint)					{	vPoint = vForward;									}
    inline	vec3f	GetUp(void)									{	return vUp;											}
	inline	void	CopyUp(vec3f& vPoint)						{	vPoint = vUp;										}
	inline	void	CopyZAxis(vec3f& vVector)					{	CopyForward(vVector);								}
	inline	void	CopyYAxis(vec3f& vVector)					{	CopyUp(vVector);									}
	inline	void	CopyXAxis(vec3f& vVector)					{	cross_product(vVector, vUp, vForward);				}
	inline	vec3f	GetRight(void)								{	vec3f A; cross_product(A, vUp, vForward); return A;	}

	//  Modifiers
	inline	void	SetOrigin(const vec3f vPoint)				{	vOrigin = vPoint;									}
	inline	void	SetOrigin(float x, float y, float z)		{	vOrigin = vec3f(x, y, z);							}
	inline	void	SetForward(const vec3f vPoint)				{	vForward = vPoint;									}
	inline	void	SetForward(float x, float y, float z)		{	vForward = vec3f(x, y, z);							}
	inline	void	SetUp(const vec3f vPoint)					{	vUp = vPoint;										}
	inline	void	SetUp(float x, float y, float z)			{	vUp = vec3f(x, y, z);								}
    inline	void	TranslateWorld(float x, float y, float z)	{	vOrigin += vec3f(x, y, z);							}
	inline	void	TranslateLocal(float x, float y, float z)	{	MoveForward(z); MoveUp(y); MoveRight(x);			}
	inline	void	MoveForward(float fDelta)					{	vOrigin += vForward * fDelta;						}
	inline	void	MoveUp(float fDelta)						{	vOrigin += vUp * fDelta;							}
	inline	void	MoveRight(float fDelta)						{	vOrigin += GetRight() * fDelta;						}
	inline	void	ApplyCameraTransform(bool bRotationOnly = false)    
	{
		// Create a 4x4 matrix
		float Matrix[16];
		vec3f X, Z;

		// Make rotation matrix
		// Z vector is reversed
		Z = vForward * -1.0f;
		
		// X vector = Y cross Z
		cross_product(X, vUp, Z);

		// Define the matrix
		Matrix[0] 	= X.x;
		Matrix[4] 	= X.y;
		Matrix[8] 	= X.z;
		Matrix[12] 	= 0.0f;
		Matrix[1] 	= vUp.x;
		Matrix[5] 	= vUp.y;
		Matrix[9] 	= vUp.z;
		Matrix[13] 	= 0.0f;
		Matrix[2] 	= Z.x;
		Matrix[6] 	= Z.y;
		Matrix[10] 	= Z.z;
		Matrix[14] 	= 0.0f;
		Matrix[3] 	= 0.0f;
		Matrix[7] 	= 0.0f;
		Matrix[11] 	= 0.0f;
		Matrix[15] 	= 1.0f;

		// Camera Transform   
		glMultMatrixf(Matrix);
	
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
};


#endif