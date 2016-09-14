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
/*  File:       DebugTools.cpp                                   */
/*                                                               */
/*  Purpose:    This file contains defined functionality which   */
/*              can be used to help find problems and inform     */
/*              of the true meaning of the issue.                */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#include "DebugTools.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

void Assert_Handler( const void *expression, const void *file_name, unsigned line_number )
{
	static char assertbuf[4096];
	char temp[1024];

	sprintf_s(assertbuf, 1024, "\n ::: Assertion failed!\n");
	temp[sizeof(temp)-1] = 0;

	sprintf_s( temp, 1024, "File: %s\n", file_name );
	strcat_s( assertbuf, 4096, temp );

	sprintf_s( temp, 1024, "Line: %d\n", line_number );
	strcat_s( assertbuf, 4096, temp );

	sprintf_s( temp, 1024, "Expression: %s\n", expression );
	strcat_s( assertbuf, 4096, temp );

	assert(assertbuf && false);
}