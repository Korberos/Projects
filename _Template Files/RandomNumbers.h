////////////////////////////////////////////////////////////////////////////////
//
//	Name:			RandomNumbers.h
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

#ifndef _RANDOM_NUMBERS_
#define _RANDOM_NUMBERS_

void Seed_Random_Number_Generator( void );
int Random_Integer( int low, int high );
float Random_Float( float low, float high );

#endif