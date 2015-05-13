#ifndef _SERVER_CONNECTION_
#define _SERVER_CONNECTION_

#include "./../../ClientServerCommon.h"
#include "WinsockWrapper/WinsockWrapper.h"

struct ServerConnection
{
public:
	ServerConnection( int socket )
	{
		Socket				= socket;

		char* ip = tcpip( Socket );
		strcpy_s(IPAddress, 16, ip);

		PingCheck			= 0;
		CurrentLocation	= PL_MAINMENU;
		LocationID			= -1;
		MessageBuffer		= createbuffer();
		AccountID			= -1;
		AccountName			= "";
		CharacterID			= -1;
		CharacterName		= "";
	}

	int					Socket;
	char					IPAddress[16];
	int					PingCheck;
	PlayerLocation		CurrentLocation;
	int					LocationID;
	int					MessageBuffer;
	int					AccountID;
	std::string			AccountName;
	int					CharacterID;
	std::string			CharacterName;

private:
};

#endif