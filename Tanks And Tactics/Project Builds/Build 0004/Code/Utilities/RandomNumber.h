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

#ifndef _DB_RANDOMNUMBER_
#define _DB_RANDOMNUMBER_

#include <stdlib.h>
#include <ctime>

void	SeedNumberGenerator(void);
int		RandInt(int Low, int High);
float	RandFloat(float Low, float High);

#endif