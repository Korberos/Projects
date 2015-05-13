////////////////////////////////////////////////////////////////////////////////
//
//	Name:			RandomNumber.h
//
//	Description:	This file contains functions that will return a
//					random number.
//
//					Note: This system can later be altered to
//					contain a list of numbers so that on-the-fly
//					generation can be eliminated
//
//	Created:		January 31st, 2009
//
//	Last Modified:	January 31st, 2009
//
////////////////////////////////////////////////////////////////////////////////

#include "RandomNumber.h"

void SeedNumberGenerator(void)
{
	srand((unsigned int)(time(0)));
}

int RandInt(int Low, int High)
{
	if (Low > High)
	{
		// Butterfly swap
		High ^= Low;
		Low ^= High;
		High ^= Low;
	}

	// Find out if the number is below 0, and compensate accordingly
	int SubtractBack = 0;
	if (Low < 0)
	{
		SubtractBack = -Low;
		Low = 0;
		High += SubtractBack;
	}

	return (Low + (rand() & (High - Low))) - SubtractBack;
}

float RandFloat(float Low, float High)
{
	if (Low > High)
	{
		// Simple Swap
		float Temp = Low;
		Low = High;
		High = Temp;
	}

	// Get integer versions of the numbers multiplied by 1000
	int nL = (int)(Low * 1000.0f);
	int nH = (int)(High * 1000.0f);

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