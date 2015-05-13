////////////////////////////////////////////////////////////////////////////////
//
//	Name:			SQL_Wrapper.cpp
//
//	Description:	This file contains wrapper functions for
//					accessing a database through PostGreSQL
//					and it's library libpq
//
//	Created:		August 27th, 2009
//
//	Last Modified:	September 25th, 2010
//
////////////////////////////////////////////////////////////////////////////////

#include "SQL_Wrapper.h"

#include <stdlib.h>

bool ConnectToDatabase( PGconn*& connection, char* host_name, char* port, char* options, char* tty, char* database_name, char* username, char* password )
{
	if (database_name == "")	return false;
	if (username == "")			return false;
	if (password == "")			return false;

	connection = PQsetdbLogin( host_name, port, options, tty, database_name, username, password );
	if ( PQstatus( connection ) != CONNECTION_OK )
	{
		PQfinish( connection );
		connection = NULL;
		return false;
	}

	return true;
}


void DisconnectFromDatabase( PGconn* connection )
{
	if ( connection == NULL ) { return; }

	PQfinish( connection );
	connection = NULL;
}


bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, int& integer_value )
{
	if ( connection == NULL )							return false;
	if ( value_name == NULL || value_name == "" )			return false;
	if ( table_name == NULL || table_name == "" )			return false;

	char sql_command[1024];
	sprintf_s( sql_command, "SELECT %s FROM %s%s%s;", value_name, table_name, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec(connection, sql_command);
	if ( PQresultStatus( result ) != PGRES_TUPLES_OK )
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	char* result_string = PQgetvalue( result, 0, 0 );
	if ( result_string == NULL ) return false;

	integer_value = atoi( result_string );
	PQclear( result );
	return true;
}


bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, float& float_value )
{
	if ( connection == NULL )						return false;
	if ( value_name == NULL || value_name == "" )		return false;
	if ( table_name == NULL || table_name == "" )		return false;

	char sql_command[1024];
	sprintf_s( sql_command, "SELECT %s FROM %s%s%s;", value_name, table_name, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec( connection, sql_command );
	if ( PQresultStatus( result ) != PGRES_TUPLES_OK )
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	char* result_string = PQgetvalue( result, 0, 0 );
	if ( result_string == NULL ) return false;

	float_value = float( atof( result_string ) );
	PQclear( result );
	return true;
}


bool GetValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, char* string_value, unsigned int string_length )
{
	if ( connection == NULL )						return false;
	if ( value_name == NULL || value_name == "" )		return false;
	if ( table_name == NULL || table_name == "" )		return false;

	char sql_command[1024];
	sprintf_s( sql_command, "SELECT %s FROM %s%s%s;", value_name, table_name, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec( connection, sql_command );
	if ( PQresultStatus( result ) != PGRES_TUPLES_OK )
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	char* result_string = PQgetvalue( result, 0, 0 );
	if ( result_string == NULL ) return false;

	strcpy_s( string_value, string_length, result_string );
	PQclear( result );
	return true;
}


bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, int new_value )
{
	if ( connection == NULL )								return false;
	if ( value_name == NULL || value_name == "" )				return false;
	if ( table_name == NULL || table_name == "" )				return false;
	if ( row_identifier == NULL || row_identifier == "" )		return false;

	char sql_command[1024];
	sprintf_s( sql_command, "UPDATE %s SET %s=%d%s%s;", table_name, value_name, new_value, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec( connection, sql_command );
	if ( PQresultStatus( result ) != PGRES_COMMAND_OK )
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	PQclear( result );
	return true;
}


bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, float new_value )
{
	if ( connection == NULL )								return false;
	if ( value_name == NULL || value_name == "" )				return false;
	if ( table_name == NULL || table_name == "" )				return false;
	if ( row_identifier == NULL || row_identifier == "" )		return false;

	char sql_command[1024];
	sprintf_s( sql_command, "UPDATE %s SET %s=%d%s%s;", table_name, value_name, new_value, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec( connection, sql_command );
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	PQclear( result );
	return true;
}


bool UpdateValue( PGconn* connection, char* value_name, char* table_name, char* row_identifier, char* new_value )
{
	if ( connection == NULL )											return false;
	if ( value_name == NULL || value_name == "" )				return false;
	if ( table_name == NULL || table_name == "" )				return false;
	if ( row_identifier == NULL || row_identifier == "" )		return false;

	char sql_command[1024];
	sprintf_s( sql_command, "UPDATE %s SET %s=%d%s%s;", table_name, value_name, new_value, row_identifier == 0 ? "" : " WHERE ", row_identifier == 0 ? "" : row_identifier );
	PGresult* result = PQexec( connection, sql_command );
	if ( PQresultStatus(result) != PGRES_COMMAND_OK )
    {
		DatabaseLogger.Add_Line( PQerrorMessage( connection ) );
		PQclear( result );
		return false;
    }

	PQclear( result );
	return true;
}