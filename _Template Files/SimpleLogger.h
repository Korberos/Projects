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