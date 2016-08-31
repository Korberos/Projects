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
/*  File:       DebugTools.h                                     */
/*                                                               */
/*  Purpose:    This file contains defined functionality which   */
/*              can be used to help find problems and inform     */
/*              of the true meaning of the issue.                */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#ifndef _DEBUG_TOOLS_
#define _DEBUG_TOOLS_

void Assert_Handler( const void *expression, const void *file_name, unsigned line_number );

#define ASSERT(exp)	(void) ((exp) || (Assert_Handler(#exp, __FILE__, __LINE__), 0) )
#define FAIL_IF(exp)	if ((exp) && (Assert_Handler(#exp, __FILE__, __LINE__), true))
#define ENFORCED_IF(exp) if ((exp) || (Assert_Handler(#exp, __FILE__, __LINE__), false))

#endif