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
/*  File:       RandomNumbers.h                                  */
/*                                                               */
/*  Purpose:    This file contains functions that will return    */
/*              a random number of different types.              */
/*                                                               */
/*  Created:    01/31/2009                                       */
/*  Last Edit:  01/31/2009                                       */
/*****************************************************************/

#ifndef _RANDOM_NUMBERS_
#define _RANDOM_NUMBERS_

void Seed_Random_Number_Generator( void );
int Random_Integer( int low, int high );
float Random_Float( float low, float high );

#endif