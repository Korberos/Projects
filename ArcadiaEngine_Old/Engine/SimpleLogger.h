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
/*  File:       SimpleLogger.h                                   */
/*                                                               */
/*  Purpose:    This file contains a class that allows for       */
/*              simple logging to a text file. Simply create     */
/*              an instance of the class and designate a file    */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/


#ifndef _SIMPLE_LOGGER_H_
#define _SIMPLE_LOGGER_H_

#include <string>
#include <vector>

class SimpleLogger
{
public:
	SimpleLogger( std::string log_file );
	~SimpleLogger();

	void Add_Line( std::string new_line );

private:
	void Write_Log_Data( void );

	std::string LogFile;
	std::vector< std::string > LogList;
	unsigned int LogSizeInBytes;
};

#endif // _SIMPLE_LOGGER_H_