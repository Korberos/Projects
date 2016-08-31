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
/*  File:       RandomNumbers.cpp                                */
/*                                                               */
/*  Purpose:    This file contains functions that will return    */
/*              a random number of different types.              */
/*                                                               */
/*  Created:    01/31/2009                                       */
/*  Last Edit:  01/31/2009                                       */
/*****************************************************************/

#include "RandomNumbers.h"
#include <ctime>
#include <stdlib.h>

void Seed_Random_Number_Generator( void )
{
	srand((unsigned int)(time(0)));
}

int Random_Integer( int low, int high )
{
	if ( low == high )
	{
		return low;
	}

	if (low > high)
	{
		// Butterfly swap
		high ^= low;
		low ^= high;
		high ^= low;
	}

	// Find out if the number is below 0, and compensate accordingly
	int subtraction = 0;
	if (low < 0)
	{
		subtraction = -low;
		low = 0;
		high += subtraction;
	}

	return (low + (rand() % (high - low))) - subtraction;
}

float Random_Float( float low, float high )
{
	if ( low == high )
	{
		return low;
	}

	if ( low > high )
	{
		// Simple Swap
		float Temp = low;
		low = high;
		high = Temp;
	}

	//  Get integer versions of the numbers multiplied by 1000
	int int_low = int(low * 1000.0f);
	int int_high = int(high * 1000.0f);

	//  Find out if the number is below 0, and compensate accordingly
	int subtraction = 0;
	if ( int_low < 0 )
	{
		subtraction = -int_low;
		int_low = 0;
		int_high += subtraction;
	}

	//  Get a random int between int_low and int_high
	int random_int = rand() % int_high;
	random_int -= subtraction;

	//  Convert the int to a float again
	float return_value = float(random_int) / 1000.0f;

	//  Return the random float
	return return_value;
}