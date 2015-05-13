#include "ClientConnection.h"

bool ClientConnection::Connect_To_Server( int& socket, char* ip, int port )
{
	FAIL_IF ( socket >= 0 )
	{
		DebugConsole::Get_Instance()->Add_String( "Socket is already defined." );
		return false;
	}

	socket = tcpconnect( ip, port, 1 );
	FAIL_IF ( socket < 0 )
	{
		DebugConsole::Get_Instance()->Add_String( "Connecting to server failed!" );
		return false;
	}

	DebugConsole::Get_Instance()->Add_String( "Successfully connected to server." );
	return true;
}


void ClientConnection::Request_News_Feed( int socket )
{
	FAIL_IF ( socket < 0 ) { return; }

	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_NEWS_REQUEST, BUFFER_PACKET );
	writechar( NEWS_ENTRIES_REQUESTED, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Login( int socket, const char* username, const char* password )
{
	FAIL_IF ( socket < 0 ) { return; }

	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_LOGIN_REQUEST, BUFFER_PACKET );
	writestring( username, BUFFER_PACKET );
	writestring( password, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Register( int socket, const char* username, const char* password )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_REGISTER_REQUEST, BUFFER_PACKET );
	writestring( username, BUFFER_PACKET );
	writestring( password, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Character_Data( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_DATA_REQUEST, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Character_Creation( int commander_type, const std::string& character_name, int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_CREATION_REQUEST, BUFFER_PACKET );

	writeint( commander_type, BUFFER_PACKET );
	writestring( character_name.c_str(), BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Character_Deletion( int character_id, int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_DELETION_REQUEST, BUFFER_PACKET );

	writeint( character_id, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Character_Choice( int socket, int character_id )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_CHOICE_REQUEST, BUFFER_PACKET );

	writeint( character_id, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Map_List( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_MAP_LIST_REQUEST, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Random_Lobby_Join( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_JOIN_RANDOM_LOBBY_REQUEST, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Random_Battle_Join( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_JOIN_RANDOM_BATTLE_REQUEST, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Custom_Lobby( int socket, const std::string& lobby_name, int battle_type, int map_index, int players_index )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_CUSTOM_LOBBY_REQUEST, BUFFER_PACKET );

	writestring( lobby_name.c_str(), BUFFER_PACKET );
	writeint( battle_type, BUFFER_PACKET );
	writeint( map_index, BUFFER_PACKET );
	writeint( players_index, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Lobby_Chat_Message( int socket, const char* chat_string )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_LOBBY_CHAT_MESSAGE_REQUEST, BUFFER_PACKET );

	writestring( chat_string, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Lobby_Start_Battle( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_LOBBY_START_BATTLE_REQUEST, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Battle_Entry_Confirmation( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_ENTRY_CONFIRMATION, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_Battle_Chat_Message( int socket, const char* chat_string )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_CHAT_MESSAGE_REQUEST, BUFFER_PACKET );

	writestring( chat_string, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Request_End_Turn_Message( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_END_TURN_REQUEST, BUFFER_PACKET );
	
	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void ClientConnection::Leave_Battle( int socket )
{
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_LEAVING_BATTLE, BUFFER_PACKET );
	
	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}