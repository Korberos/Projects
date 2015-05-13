#ifndef _CLIENT_CONNECTION_
#define _CLIENT_CONNECTION_

#include "WinsockWrapper/WinsockWrapper.h"
#include "./../../ClientServerCommon.h"
#include "DebugTools.h"
#include "DebugConsole.h"

#define SERVER_IP				"drewbanyai.zapto.org"
#define NEWS_ENTRIES_REQUESTED	5

namespace ClientConnection
{
	bool Connect_To_Server( int& socket, char* ip, int port );
	void Request_News_Feed( int socket );
	void Request_Login( int socket, const char* username, const char* password );
	void Request_Register( int socket, const char* username, const char* password );
	void Request_Character_Data( int socket );
	void Request_Character_Creation( int commander_type, const std::string& character_name, int socket );
	void Request_Character_Deletion( int character_id, int socket );
	void Request_Character_Choice( int socket, int character_id );
	void Request_Map_List( int socket );
	void Request_Random_Lobby_Join( int socket );
	void Request_Random_Battle_Join( int socket );
	void Request_Custom_Lobby( int socket, const std::string& lobby_name, int battle_type, int map_index, int players_index );
	void Request_Lobby_Chat_Message( int socket, const char* chat_string );
	void Request_Lobby_Start_Battle( int socket );
	void Request_Battle_Entry_Confirmation( int socket );
	void Request_Battle_Chat_Message( int socket, const char* chat_string );
	void Request_End_Turn_Message( int socket );
	void Leave_Battle( int socket );
}

#endif