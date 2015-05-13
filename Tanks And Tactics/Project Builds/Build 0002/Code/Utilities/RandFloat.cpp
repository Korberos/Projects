////////////////////////////////////////////////////////////////////////////////
//
//	Name:			RandFloat.cpp
//
//	Description:	This file contains a function to quickly
//					create a random float between the two extremes
//					defined as arguments
//
//	Created:		January 27th, 2009
//
//	Last Modified:	January 27th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "RandFloat.h"

float RandFloat(float L, float H)
{
	// Get integer versions of the numbers multiplied by 1000
	int nL = (int)(L) * 1000;
	int nH = (int)(H) * 1000;

	// Find out if the number is below 0, and compensate accordingly
	int SubtractBack = 0;
	if (nL < 0)
	{
		SubtractBack = -nL;
		nL = 0;
		nH += SubtractBack;
	}

	// Get a random int between nL and nH
	int RandomInt = rand() % nH;
	RandomInt -= SubtractBack;

	// Convert the int to a float again
	float Return = ((float)RandomInt) / 1000.0f;

	// Return the random float
	return Return;
}