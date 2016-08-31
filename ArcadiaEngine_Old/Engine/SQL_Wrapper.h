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
/*  File:       SQL_Wrapper.h                                    */
/*                                                               */
/*  Purpose:    This file contains functionality for writing     */
/*              and reading information to and from an SQL       */
/*              database through the PostGreSQL library.         */
/*                                                               */
/*  Created:    08/27/2009                                       */
/*  Last Edit:  08/27/2009                                       */
/*****************************************************************/

#ifndef _SQL_WRAPPER_
#define _SQL_WRAPPER_

#pragma comment(lib, "libpq.lib")
#include <libpq-fe.h>
#include "SimpleLogger.h"

static SimpleLogger DatabaseLogger( "Logs/Database_Log.txt" );

bool ConnectToDatabase( PGconn*& connection, char* host_name, char* port, char* options, char* tty, char* database_name, char* username, char* password );
void DisconnectFromDatabase( PGconn* connection );
bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, int& integer_value );
bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, float& float_value );
bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, char* string_value, unsigned int string_length );
bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, int new_value );
bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, float new_value );
bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, char* new_value );

#endif