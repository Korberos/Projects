////////////////////////////////////////////////////////////////////////////////
//
//	Name:			PickingRay.cpp
//
//	Description:	This file contains a calculation function
//					getting a picking ray, given information
//					from the camera and the click position on
//					the screen.
//
//	Created:		January 22nd, 2009
//
//	Last Modified:	January 22nd, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "PickingRay.h"

vec3f CalculatePickingRay(unsigned int ScreenX, unsigned int ScreenY, COpenGLFrame& Cam)
{
	// Calculate the Picking Ray
	vec3f PickingRay;
	PickingRay  = Cam.GetForward();
	PickingRay -= Cam.GetRight() * (((float)ScreenX / (float)(WINDOW_WIDTH-1))  * 2.0f - 1.0f) * 0.31f;
	PickingRay += Cam.GetUp() * (((float)ScreenY / (float)(WINDOW_HEIGHT-1)) * 2.0f - 1.0f) * 0.415f * vec3f(-1.0f, -1.0f, -1.0f);;
	PickingRay.normalize();
	PickingRay *= 10.0f;

	return PickingRay;
}