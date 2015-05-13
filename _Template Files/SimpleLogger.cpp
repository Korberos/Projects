#include "SimpleLogger.h"

#include <fstream>
#include "DebugTools.h"

#define MAX_LOG_SIZE_BYTES_IN_MEMORY 1024

SimpleLogger::SimpleLogger( std::string log_file ) : 
	LogFile( log_file ),
	LogSizeInBytes( 0 )
{
	LogList.clear();
}


SimpleLogger::~SimpleLogger()
{
	Write_Log_Data();
}


void SimpleLogger::Add_Line( std::string new_line )
{
	LogList.push_back( new_line );
	LogSizeInBytes += new_line.length() + 1;

	if ( LogSizeInBytes >= MAX_LOG_SIZE_BYTES_IN_MEMORY )
	{
		Write_Log_Data();
	}
}


void SimpleLogger::Write_Log_Data( void )
{
	//  Note: std::ios_base::trunc clears any information within the log before adding new information
	std::ofstream output( LogFile.c_str(), std::ios_base::trunc );
	FAIL_IF ( !output.good() || output.bad() ) { return; }

	for ( std::vector< std::string >::iterator log_iter = LogList.begin(); log_iter != LogList.end(); ++log_iter )
	{
		output << log_iter->c_str() << std::endl;
	}

	output.close();
	LogList.clear();
	LogSizeInBytes = 0;
}