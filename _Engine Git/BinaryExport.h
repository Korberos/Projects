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
/*  File:       BinaryExport.h                                   */
/*                                                               */
/*  Purpose:    This file contains functions that will take      */
/*              in an XML file and export binary data values     */
/*              which can then be loaded back easily. (Note:     */
/*              this is a preliminary for what game types will   */
/*              do to save data to the machine.                  */
/*                                                               */
/*  Created:    06/01/2010                                       */
/*  Last Edit:  06/15/2010                                       */
/*****************************************************************/

#ifndef _BINARY_EXPORT_
#define _BINARY_EXPORT_

bool BinaryExport( char* export_list_file );

#endif