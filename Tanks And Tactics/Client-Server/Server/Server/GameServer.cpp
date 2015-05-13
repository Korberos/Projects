#include "GameServer.h"

#include <time.h>
#include <queue>
#include "DebugTools.h"
#include "SQL_Wrapper.h"
#include "LobbyManager.h"
#include "WorldController.h"
#include "BattleController.h"
#include "RandomNumbers.h"
#include "SimpleLogger.h"
#include "MapData.h"

static SimpleLogger ServerLogger( "Logs/Server_Log.txt" );

GameServer* GameServer::Get_Instance( void )
{
	static GameServer INSTANCE;
	return &INSTANCE;
}

bool GameServer::Initialize( void )
{
	//  Initialize the Winsock Wrapper
	Winsock_Init( BUFFER_TYPE_COUNT );

	//  If the socket is already defined (or not initialized to -1), write it to the log and exit
	if ( Socket >= 0 )
	{
		ServerLogger.Add_Line( "\'Socket\' is already defined. Can not initialize server." );
		return false;
	}

	// Initialize the Listening Port for Clients
	Socket = tcplisten( SERVER_PORT, 100, 1 );
	
	//  If the socket creation fails, write it to the log and exit
	if ( Socket < 0 )
	{
		ServerLogger.Add_Line( "Server listening socket creation failed." );
		return false;
	}
	
	setnagle(Socket, true);

	//  Connect to the game database. If it failed, write it to the log and exit
	if ( !ConnectToDatabase( DatabaseConnection, "localhost", "5432", "", "", "postgres", "postgres", "drew3739" ) )
	{
		ServerLogger.Add_Line( "The server could not connect to the game database." );
		return false;
	}

	//  Load the list of maps
	LoadMapList();
	return true;
}


void GameServer::Service( void )
{
	//  If anyone is attempting to join, allow them in
	Accept_New_Connections();

	//  Receive messages and react to them accordingly
	Receive_Messages();

	//  Service the Battle Controller
}


GameServer::GameServer() : 
	Socket( -1 )
{
	ConnectionList.clear();
	BattleController::MapList.clear();
}


GameServer::~GameServer()
{
	closesock( Socket );
}


void GameServer::LoadMapList( void )
{
	BattleController::MapList.clear();

	XML_File* map_list_xml = XML_File::Load_XML_File( "./Assets/MapList.xml" );
	if ( !map_list_xml ) return;

	XML_Tag* entire_xml = map_list_xml->Get_Encompassing_Tag();
	if ( entire_xml->Children.size() != 1 )
	{	
		delete map_list_xml;
		return;
	}

	XML_Tag* map_list = entire_xml->Find_Child("MapList", 1);
	if ( map_list == NULL )
	{	
		delete map_list_xml;
		return;
	}

	for ( XML_Tag::ChildListIterType iter = map_list->Children.begin(); iter != map_list->Children.end(); ++iter )
	{
		bool valid = true;
		valid &= ( strcmp( (*iter)->Get_Name(), "MapEntry" ) == 0 );
		valid &= ( (*iter)->Children.size() == 2 );
		valid &= ( (*iter)->Find_Child( "Name", 1 ) != NULL );
		valid &= ( (*iter)->Find_Child( "File", 1 ) != NULL );

		if ( valid == false )
		{
			delete map_list_xml;
			return;
		}

		std::pair< std::string, std::string > map_struct( (*iter)->Find_Child( "Name", 1 )->Get_Value(), (*iter)->Find_Child( "File", 1 )->Get_Value() );
		BattleController::MapList.push_back( map_struct );
	}
}

void GameServer::Accept_New_Connections( void )
{
	//  Check for an incoming client connection
	int new_connection = tcpaccept( Socket, 1 );
	while ( new_connection >= 0 )
	{
		//  TODO:  Look through a list of pre-loaded IP Addresses and MAC addresses that are banned. If this is one of them, send them a message and disconnect them immediately.

		//  Add the new connection to the list
		ConnectionList.push_back( ServerConnection( new_connection ) );

		//  Check for another incoming client connection
		new_connection = tcpaccept( Socket, 1 );
	}
}

void GameServer::Receive_Messages( void )
{
	//  Check for messages from each connection
	for ( ConnectionListIteratorType iter = ConnectionList.begin(); iter != ConnectionList.end(); )
	{
		int buffer_size = buffsize( (*iter).MessageBuffer );
		int length = buffer_size > 4 ? readint( (*iter).MessageBuffer, true ) - buffer_size : 0;	
		int bytes_received = receivemessage( (*iter).Socket, 0, BUFFER_RECEIVE, length );
		if ( bytes_received == 0 )
		{
			Remove_Connection( (*iter) );
			iter = ConnectionList.erase( iter );
			continue;
		}
		else if ( bytes_received < 0 )
		{
			iter++;
			continue;
		}

		//  If we've gotten this far, we have received data
		copybuffer( (*iter).MessageBuffer, BUFFER_RECEIVE );

		//  If we haven't received enough data to even get the size, ignore the buffer for now
		if ( buffsize( (*iter).MessageBuffer ) < sizeof(int) )
		{
			iter++;
			continue;
		}

		int packet_size = readint( (*iter).MessageBuffer, true );
		if ( buffsize( (*iter).MessageBuffer ) < packet_size )
		{
			iter++;
			continue;
		}
		setpos( getpos( true, (*iter).MessageBuffer ) + sizeof(packet_size), (*iter).MessageBuffer );

		char message_id = readchar( (*iter).MessageBuffer );
		switch ( message_id )
		{
		case CLIENT_TO_SERVER_MESSAGE_ID_PONG:
			(*iter).PingCheck = 0;
			//  TODO:  Actually send out SERVER_TO_CLIENT_MESSAGE_ID_PING and iterate PingCheck. If it reaches 10, send a message alerting to disconnect, and disconnect.
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_NEWS_REQUEST:
			Send_News_To_Player( (*iter), int(readchar( (*iter).MessageBuffer )) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_REGISTER_REQUEST:
			Register_Account( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_LOGIN_REQUEST:
			Login_Account( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_DATA_REQUEST:
			Send_Character_Data_To_Player( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_CREATION_REQUEST:
			Create_Character( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_DELETION_REQUEST:
			Delete_Character( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_CHARACTER_CHOICE_REQUEST:
			Choose_Character( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_MAP_LIST_REQUEST:
			Send_Map_List_To_Player( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_CUSTOM_LOBBY_REQUEST:
			Process_Custom_Lobby_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_JOIN_RANDOM_LOBBY_REQUEST:
			Process_Random_Lobby_Join_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_LOBBY_CHAT_MESSAGE_REQUEST:
			Process_Lobby_Chat_Message_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_LOBBY_START_BATTLE_REQUEST:
			Process_Lobby_Start_Battle_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_JOIN_RANDOM_BATTLE_REQUEST:
			Process_Random_Battle_Join_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_ENTRY_CONFIRMATION:
			Process_Battle_Entry_Confirmation( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_CHAT_MESSAGE_REQUEST:
			Process_Battle_Chat_Message_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_OBJECT_MOVE_REQUEST:
			Process_Battle_Object_Move_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_OBJECT_ATTACK_REQUEST:
			Process_Battle_Object_Attack_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_END_TURN_REQUEST:
			Process_Battle_End_Turn_Request( (*iter) );
			break;

		case CLIENT_TO_SERVER_MESSAGE_ID_LEAVING_BATTLE:
			Process_Player_Leaving_Battle( (*iter) );
			break;

		default:
			char message_id_string[128];
			sprintf_s( message_id_string, "Invalid message ID [%d] sent from account [%d]", message_id, (*iter).AccountID );
			ServerLogger.Add_Line( message_id_string );
			break;
		}

		clearbuffer( (*iter).MessageBuffer );
		iter++;
	}
}


void GameServer::Remove_Connection( ServerConnection& connection )
{
	switch ( connection.CurrentLocation )
	{
	case PL_LOBBY:
		{
			LobbyManager* lobby_manager = LobbyManager::Get_Instance();
			lobby_manager->Remove_Player( &connection );
		}
		break;

	case PL_BATTLE:
		{
			BattleController* battle_controller = BattleController::Get_Instance();
			battle_controller->Remove_Player( &connection );
		}

	default:
		break;
	}

	connection.CurrentLocation = PL_NOLOCATION;
	connection.LocationID = -1;
}


void GameServer::Send_News_To_Player( ServerConnection& connection, int number_of_entries )
{
	//if ( connection.CurrentLocation != PL_MAINMENU )
	//{
	//	char message_id_string[128];
	//	sprintf_s( message_id_string, "Player with id [%d] is attempting to gather news when not in the main menu.", (*iter).AccountID );
	//	ServerLogger.Add_Line( message_id_string );
	//	return;
	//}

	char sql_command[1024];
	sprintf_s( sql_command, 1024, "SELECT entry_created_date, news_headline, news_body, headline_link FROM game_news_feed ORDER BY entry_created_date DESC LIMIT %d;", number_of_entries );
	PGresult* result = PQexec( DatabaseConnection, sql_command );
	if ( PQresultStatus( result ) != PGRES_TUPLES_OK )
	{
		//  Log this error to the Server Logger
		char log_message[512];
		sprintf_s( log_message, "Retrieval of news failed. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
		ServerLogger.Add_Line( log_message );

		PQclear( result );
		return;
   }

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_NEWS_RESPONSE, BUFFER_PACKET );
	for ( int i = 0; i < number_of_entries; ++i )
	{
		writestring( PQgetvalue( result, i, 0 ), BUFFER_PACKET );
		writestring( PQgetvalue( result, i, 1 ), BUFFER_PACKET );
		writestring( PQgetvalue( result, i, 2 ), BUFFER_PACKET );
	}

	PQclear( result );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Register_Account( ServerConnection& connection )
{
	//if ( connection.CurrentLocation != PL_MAINMENU )
	//{
	//	char message_id_string[128];
	//	sprintf_s( message_id_string, "Player with id [%d] is attempting to register when not in the main menu.", (*iter).AccountID );
	//	ServerLogger.Add_Line( message_id_string );
	//	return;
	//}

	time_t current_time;
	time( &current_time );
	char current_time_string[256];
	sprintf_s( current_time_string, 255, "TIMESTAMP WITH TIME ZONE \'epoch\' + %d * INTERVAL \'1 second\'", current_time );

	char username[32];
	char password[32];
	char* temp_string;

	temp_string = readstring( connection.MessageBuffer );
	strcpy_s( username, 32, temp_string );

	temp_string = readstring( connection.MessageBuffer );
	strcpy_s( password, 32, temp_string );

	char table_entry_identifier[64];
	sprintf_s( table_entry_identifier, 64, "UPPER(username) = UPPER(\'%s\')", username );

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_REGISTER_RESPONSE, BUFFER_PACKET );

	int accounts_with_this_name = 0;
	if ( !GetValue( DatabaseConnection, "COUNT(account_id)", "game_accounts", table_entry_identifier, accounts_with_this_name ) )
	{
		writechar( REGISTRATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
	}
	else if (accounts_with_this_name != 0)
	{
		writechar( REGISTRATION_RESPONSE_ACCOUNT_TAKEN, BUFFER_PACKET );
	}
	else
	{
		int account_count;
		if ( !GetValue( DatabaseConnection, "COUNT(*)", "game_accounts", NULL, account_count ) )
		{
			writechar( REGISTRATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else
		{
			char sql_command[1024];
			sprintf_s( sql_command, "INSERT INTO game_accounts VALUES(%d, \'%s\', \'%s\', %s);", account_count, username, password, current_time_string );
			PGresult* result = PQexec( DatabaseConnection, sql_command );
			if ( PQresultStatus(result) != PGRES_COMMAND_OK )
			{
				//  Log this error to the Server Logger
				char log_message[512];
				sprintf_s( log_message, "Failed to add game account. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
				ServerLogger.Add_Line( log_message );

				PQclear( result );
				writechar( REGISTRATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
			}
			else
			{
				PQclear( result );
				writechar( REGISTRATION_RESPONSE_SUCCESS, BUFFER_PACKET );
			}
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Login_Account( ServerConnection& connection )
{
	//if ( connection.CurrentLocation != PL_MAINMENU )
	//{
	//	char message_id_string[128];
	//	sprintf_s( message_id_string, "Player with id [%d] is attempting to log in when not in the main menu.", (*iter).AccountID );
	//	ServerLogger.Add_Line( message_id_string );
	//	return;
	//}

	char username[32];
	char password[32];
	char* temp_string;

	temp_string = readstring( connection.MessageBuffer );
	strcpy_s( username, 32, temp_string );

	temp_string = readstring( connection.MessageBuffer );
	strcpy_s( password, 32, temp_string );

	char table_entry_identifier[64];
	sprintf_s( table_entry_identifier, 64, "UPPER(username) = UPPER(\'%s\')", username );

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOGIN_RESPONSE, BUFFER_PACKET );

	int accounts_with_this_name = 0;
	if ( !GetValue( DatabaseConnection, "COUNT(account_id)", "game_accounts", table_entry_identifier, accounts_with_this_name ) )
	{
		writechar( LOGIN_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
	}
	else if (accounts_with_this_name == 0)
	{
		writechar( LOGIN_RESPONSE_ACCOUNT_DOES_NOT_EXIST, BUFFER_PACKET );
	}
	else
	{
		int account_count;
		if ( !GetValue( DatabaseConnection, "COUNT(*)", "game_accounts", table_entry_identifier, account_count ) )
		{
			writechar( LOGIN_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else if ( account_count != 1 )
		{
			writechar( LOGIN_RESPONSE_MULTIPLE_ACCOUNTS_EXIST, BUFFER_PACKET );
		}
		else
		{
			char sql_command[1024];
			sprintf_s( sql_command, "SELECT account_password FROM game_accounts WHERE %s;", table_entry_identifier );
			PGresult* result = PQexec( DatabaseConnection, sql_command );
			if ( PQresultStatus(result) != PGRES_TUPLES_OK )
			{
				//  Log this error to the Server Logger
				char log_message[512];
				sprintf_s( log_message, "Failed to retrieve password for account %s. SQL Error: %s", username, PQerrorMessage( DatabaseConnection ) );
				ServerLogger.Add_Line( log_message );

				PQclear( result );
				writechar( REGISTRATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
			}
			else
			{
				char real_password[32];
				strcpy_s( real_password, 32, PQgetvalue( result, 0, 0 ) );

				if ( _strcmpi( password, real_password ) != 0 )
				{
					writechar( LOGIN_RESPONSE_PASSWORD_INCORRECT, BUFFER_PACKET );
				}
				else
				{
					writechar( LOGIN_RESPONSE_SUCCESS, BUFFER_PACKET );
					connection.CurrentLocation = PL_CHARACTERSELECT;

					sprintf_s( sql_command, "SELECT account_id FROM game_accounts WHERE %s;", table_entry_identifier );
					PGresult* result = PQexec( DatabaseConnection, sql_command );
					if ( PQresultStatus(result) != PGRES_TUPLES_OK )
					{
						//  Log this error to the Server Logger
						char log_message[512];
						sprintf_s( log_message, "Failed to retrieve password for account %s. SQL Error: %s", username, PQerrorMessage( DatabaseConnection ) );
						ServerLogger.Add_Line( log_message );

						PQclear( result );
						writechar( REGISTRATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
					}
					connection.AccountID = atoi( PQgetvalue( result, 0, 0 ) );
				}

				PQclear( result );
			}
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Send_Character_Data_To_Player( ServerConnection& connection )
{
	char sql_command[1024];

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_DATA_RESPONSE, BUFFER_PACKET );

	if ( connection.AccountID == -1 )
	{
		//  Log this error to the Server Logger
		ServerLogger.Add_Line( "Player is not logged into an account, but is trying to gather character data." );
		writechar( CHARACTER_DATA_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
	}
	else
	{
		sprintf_s( sql_command, "SELECT COUNT(character_id) FROM game_characters WHERE account_id=%d;", connection.AccountID );
		PGresult* result = PQexec( DatabaseConnection, sql_command );
		if ( PQresultStatus(result) != PGRES_TUPLES_OK )
		{
			//  Log this error to the Server Logger
			char log_message[512];
			sprintf_s( log_message, "Failed to retrieve a basic character count. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
			ServerLogger.Add_Line( log_message );

			PQclear( result );
			writechar( CHARACTER_DATA_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else
		{
			int character_count = atoi( PQgetvalue( result, 0, 0 ) );
			PQclear( result );

			if ( character_count < 0 || character_count > 3 )
			{
				//  Log this error to the Server Logger
				char log_message[512];
				sprintf_s( log_message, "The character count for account [%d] is %d.", connection.AccountID, character_count );
				ServerLogger.Add_Line( log_message );

				writechar( CHARACTER_DATA_RESPONSE_CHARACTER_COUNT_OFF, BUFFER_PACKET );
			}
			else
			{
				sprintf_s( sql_command, "SELECT character_id, character_name, character_type, character_level, character_victories, character_defeats FROM game_characters WHERE account_id=%d;", connection.AccountID );
				PGresult* result = PQexec( DatabaseConnection, sql_command );
				if ( PQresultStatus(result) != PGRES_TUPLES_OK )
				{
					//  Log this error to the Server Logger
					char log_message[512];
					sprintf_s( log_message, "Failed to retrieve character information for account [%d]. SQL Error: %s", connection.AccountID, PQerrorMessage( DatabaseConnection ) );
					ServerLogger.Add_Line( log_message );

					PQclear( result );
					writechar( CHARACTER_DATA_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
				}
				else
				{
					writechar( CHARACTER_DATA_RESPONSE_SUCCESS, BUFFER_PACKET );

					writeint( character_count, BUFFER_PACKET );

					for ( int i = 0; i < character_count; ++i )
					{
						int id = atoi( PQgetvalue( result, i, 0 ) );
						writeint( id, BUFFER_PACKET );											// ID
						if ( id == -1 ) continue;

						writestring( PQgetvalue( result, i, 1 ), BUFFER_PACKET );		// NAME
						writeint( atoi( PQgetvalue( result, i, 2 ) ), BUFFER_PACKET );	// TYPE
						writeint( atoi( PQgetvalue( result, i, 3 ) ), BUFFER_PACKET );	// LEVEL
						writeint( atoi( PQgetvalue( result, i, 4 ) ), BUFFER_PACKET );	// VICTORIES
						writeint( atoi( PQgetvalue( result, i, 5 ) ), BUFFER_PACKET );	// DEFEATS
					}
				}
			}
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Create_Character( ServerConnection& connection )
{
	int character_type;
	char character_name[32]; // NOTE:  The client needs to enforce this limit!
	char* temp_string;
	char table_entry_identifier[64];
	char sql_command[1024];

	character_type = readint( connection.MessageBuffer );

	temp_string = readstring( connection.MessageBuffer );
	strcpy_s( character_name, 31, temp_string );

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_CREATION_RESPONSE, BUFFER_PACKET );

	sprintf_s( sql_command, "SELECT COUNT(character_id) FROM game_characters WHERE account_id=%d;", connection.AccountID );
	PGresult* result = PQexec( DatabaseConnection, sql_command );
	if ( PQresultStatus(result) != PGRES_TUPLES_OK )
	{
		//  Log this error to the Server Logger
		char log_message[512];
		sprintf_s( log_message, "Failed to retrieve basic character count for account [%d]. SQL Error: %s", connection.AccountID, PQerrorMessage( DatabaseConnection ) );
		ServerLogger.Add_Line( log_message );

		PQclear( result );
		writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
	}
	else
	{
		int character_count = atoi( PQgetvalue( result, 0, 0 ) );
		PQclear( result );

		if ( character_count < 0 || character_count >= 3 )
		{
			writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else
		{
			int characters_with_this_name = 0;
			sprintf_s( table_entry_identifier, 64, "UPPER(character_name) = UPPER(\'%s\')", character_name );
			if ( !GetValue( DatabaseConnection, "COUNT(character_id)", "game_characters", table_entry_identifier, characters_with_this_name ) )
			{
				writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
			}
			else if (characters_with_this_name != 0)
			{
				char log_message[128];
				sprintf_s( log_message, "There are multiple characters with the name \"%s\" in the database.", character_name );
				ServerLogger.Add_Line( log_message );

				writechar( CHARACTER_CREATION_RESPONSE_CHARACTER_NAME_IN_USE, BUFFER_PACKET );
			}
			else
			{
				int character_count = 0;
				if ( !GetValue( DatabaseConnection, "COUNT(*)", "game_characters", NULL, character_count ) )
				{
					writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
				}
				else
				{
					sprintf_s( sql_command, 1023, "INSERT INTO game_characters VALUES(%d, %d, \'%s\', %d, %d, %d, %d);", connection.AccountID, character_count, character_name, character_type, 1, 0, 0 );
					PGresult* result = PQexec( DatabaseConnection, sql_command );
					if ( PQresultStatus(result) != PGRES_COMMAND_OK )
					{
						//  Log this error to the Server Logger
						char log_message[512];
						sprintf_s( log_message, "Failed to add a new character to database. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
						ServerLogger.Add_Line( log_message );

						PQclear( result );
						writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
					}
					else
					{
						if ( Create_Starter_Loadout( connection, character_count, character_name ) == false )
						{
							//  Log this error to the Server Logger
							char log_message[512];
							sprintf_s( log_message, "Failed to add a new starter unit collection to database. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
							ServerLogger.Add_Line( log_message );

							PQclear( result );

							writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
						}
						else
						{
							PQclear( result );
							writechar( CHARACTER_CREATION_RESPONSE_SUCCESS, BUFFER_PACKET );
						}
					}
				}
			}
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Delete_Character( ServerConnection& connection )
{
	int character_id;
	char sql_command[1024];

	character_id = readint( connection.MessageBuffer );

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_DELETION_RESPONSE, BUFFER_PACKET );

	//  Find the character, ensure it belongs to the connected account, and delete it
	sprintf_s( sql_command, "SELECT COUNT(character_id) FROM game_characters WHERE account_id=%d AND character_id=%d;", connection.AccountID, character_id );
	PGresult* result = PQexec( DatabaseConnection, sql_command );
	if ( PQresultStatus(result) != PGRES_TUPLES_OK )
	{
		//  Log this error to the Server Logger
		char log_message[512];
		sprintf_s( log_message, "Failed to retrieve basic character count. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
		ServerLogger.Add_Line( log_message );

		PQclear( result );
		writechar( CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
	}
	else
	{
		int character_count = atoi( PQgetvalue( result, 0, 0 ) );
		PQclear( result );

		sprintf_s( sql_command, 1023, "DELETE FROM game_characters WHERE character_id=%d;", character_id );
		PGresult* result = PQexec( DatabaseConnection, sql_command );
		if ( PQresultStatus(result) != PGRES_COMMAND_OK )
		{
			//  Log this error to the Server Logger
			char log_message[512];
			sprintf_s( log_message, "Failed to delete a character from the database. SQL Error: %s", PQerrorMessage( DatabaseConnection ) );
			ServerLogger.Add_Line( log_message );

			PQclear( result );
			writechar( CHARACTER_DELETION_RESPONSE_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else
		{
				PQclear( result );
				writechar( CHARACTER_DELETION_RESPONSE_SUCCESS, BUFFER_PACKET );
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


bool GameServer::Create_Starter_Loadout( ServerConnection& connection, int character_id, const char* char_name )
{
	//  For now, just give 5 starter units; 
	std::string sql_command_string;
	char sql_command[1024];
	char unit_name[64];

	int unit_count = -1;
	GetValue( DatabaseConnection, "COUNT(*)", "game_units", 0, unit_count );
	if ( unit_count < 0 ) { return false; }
	
	sql_command_string += "BEGIN; ";

	sprintf_s( sql_command, 1023, "INSERT INTO game_units VALUES( %d, %d, \'%s\', 0, 1, 0, %d, %d, %d );", character_id, unit_count, char_name, 5, 2, 6 );
	sql_command_string += sql_command;
	
	for ( int i = 1; i < 5; ++i )
	{
		sprintf_s( unit_name, "%s_unit_%d", char_name, i );
		sprintf_s( sql_command, 1023, "INSERT INTO game_units VALUES( %d, %d, \'%s\', 0, 1, 0, %d, %d, %d );", character_id, unit_count + i, unit_name, 2, 1, 4 );
		sql_command_string += sql_command;
	}
	
	sql_command_string += "COMMIT;";

	PGresult* result = PQexec( DatabaseConnection, sql_command_string.c_str() );
	return ( PQresultStatus(result) == PGRES_COMMAND_OK );
}


void GameServer::Choose_Character( ServerConnection& connection )
{
	int character_id = readint( connection.MessageBuffer );
	if ( character_id < 0 )
	{
		char log_message[512];
		sprintf_s( log_message, "Account %d attempting to choose an invalid character with ID %d", connection.AccountID, character_id );
		ServerLogger.Add_Line( log_message );
		return;
	}
	connection.CharacterID = character_id;

	//  TODO:  Set connection.CharacterName to the character name from the database
	char sql_command[1024];
	sprintf_s( sql_command, "SELECT character_name FROM game_characters WHERE character_id=%d;", character_id );
	PGresult* result = PQexec( DatabaseConnection, sql_command );
	if ( PQresultStatus(result) != PGRES_TUPLES_OK )
	{
		//  Log this error to the Server Logger
		char log_message[512];
		sprintf_s( log_message, "Failed to retrieve basic character name for character id [%d]. SQL Error: %s", character_id, PQerrorMessage( DatabaseConnection ) );
		ServerLogger.Add_Line( log_message );

		PQclear( result );
	}
	else
	{
		connection.CharacterName = PQgetvalue( result, 0, 0 );
		PQclear( result );
	}
}


void GameServer::Send_Map_List_To_Player( ServerConnection& connection )
{
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_MAP_LIST_RESPONSE, BUFFER_PACKET );

	unsigned int map_list_size = (unsigned int)(BattleController::MapList.size());
	writeuint( map_list_size, BUFFER_PACKET );
	for ( unsigned int i = 0; i < map_list_size; ++i )
	{
		writestring( BattleController::MapList[i].first.c_str(), BUFFER_PACKET );
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Process_Custom_Lobby_Request( ServerConnection& connection )
{
	std::string lobby_name( readstring( connection.MessageBuffer ) );
	BattleType battle_type = BattleType(readint( connection.MessageBuffer ));
	int map_index = readint( connection.MessageBuffer );
	PlayersType players_index = PlayersType(readint( connection.MessageBuffer ));

	//  TODO:  Check the player connection to ensure they are not in another battle or lobby

	LobbyManager* lobby_manager = LobbyManager::Get_Instance();
	bool customizing = ( connection.CurrentLocation == PL_LOBBY );
	Lobby* existing_lobby = NULL;
	bool success = lobby_manager->Customize_Lobby( &connection, lobby_name, battle_type, map_index, players_index, existing_lobby );
	if ( customizing )
	{
		//  If we are just customizing an already existing lobby, send the new info to all lobby players
		clearbuffer( BUFFER_PACKET );
		writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_INFORMATION, BUFFER_PACKET );

		writestring( existing_lobby->Get_Lobby_Name().c_str(), BUFFER_PACKET );
		writestring( BattleController::MapList[ existing_lobby->Get_Map_Index() ].first.c_str(), BUFFER_PACKET );
		writeint( existing_lobby->Get_Map_Index(), BUFFER_PACKET );
		writeint( existing_lobby->Get_Battle_Type(), BUFFER_PACKET );
		writeint( existing_lobby->Get_Players_Type(), BUFFER_PACKET );

		clearbuffer( BUFFER_PACKET_WITH_SIZE );
		writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
		copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

		//  Get all players in the lobby and send the message to each one
		const Lobby::PlayerType* player_list = existing_lobby->Get_Player_List();
		for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
		{
			if ( player_list[i] == NULL ) { continue; }
			sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
		}
		return;
	}

	//  TODO: Eventually there may be other responses than success or failure... so make an unsigned int instead of just sending a bool
	unsigned char response = success ? LOBBY_CREATION_SUCCESS : LOBBY_CREATION_FAILURE;

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_CREATE_LOBBY_RESPONSE, BUFFER_PACKET );

	writechar( response, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Process_Random_Lobby_Join_Request( ServerConnection& connection )
{
	LobbyManager* lobby_manager = LobbyManager::Get_Instance();
	Lobby* lobby = NULL;
	const int lobby_count = lobby_manager->Get_Lobby_Count();
	int random_lobby_join_response = RANDOM_LOBBY_JOIN_UNKNOWN_FAILURE;
	if ( lobby_count < 1 )
	{
		random_lobby_join_response = RANDOM_LOBBY_JOIN_FAILURE_NO_LOBBIES;
	}
	else
	{
		if ( lobby_manager->Join_Random_Lobby( &connection, lobby ) )
		{
			//  TODO:  Give lobbies an identifying ID (the hash of their name or the time of their creation, maybe?) so that the server can find them easily)
			//  For now, the LocationID will be -1, meaning you have to look for the correct lobby :(
			connection.CurrentLocation = PL_LOBBY;
			connection.LocationID = -1;

			random_lobby_join_response = RANDOM_LOBBY_JOIN_SUCCESS;
		}
		else
		{
			//  TODO:  Make a new lobby if Join_Random_Lobby fails. Currently this is an unknown error because we should be creating a new lobby for the player.
			random_lobby_join_response = RANDOM_LOBBY_JOIN_UNKNOWN_FAILURE;
		}
	}

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_JOIN_RANDOM_LOBBY_RESPONSE, BUFFER_PACKET );
	
	writechar( random_lobby_join_response, BUFFER_PACKET );

	if ( random_lobby_join_response == RANDOM_LOBBY_JOIN_SUCCESS )
	{
		writestring( lobby->Get_Lobby_Name().c_str(), BUFFER_PACKET );
		writestring( BattleController::MapList[ lobby->Get_Map_Index() ].first.c_str(), BUFFER_PACKET );
		writeint( lobby->Get_Map_Index(), BUFFER_PACKET );
		writeint( lobby->Get_Battle_Type(), BUFFER_PACKET );
		writeint( lobby->Get_Players_Type(), BUFFER_PACKET );
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Process_Random_Battle_Join_Request( ServerConnection& connection )
{
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_JOIN_RANDOM_BATTLE_RESPONSE, BUFFER_PACKET );

	BattleController* battle_controller = BattleController::Get_Instance();
	if ( battle_controller->Get_Battle_Count() == 0 )
	{
		writechar( RANDOM_BATTLE_JOIN_FAILURE_NO_BATTLES, BUFFER_PACKET );
	}
	else
	{
		int battle_index = Random_Integer( 0, battle_controller->Get_Battle_Count() );
		int map_index = battle_controller->Get_Battle_Map_Index( battle_index );
		if ( map_index == -1 )
		{
			writechar( RANDOM_BATTLE_JOIN_UNKNOWN_FAILURE, BUFFER_PACKET );
		}
		else
		{
			writechar( RANDOM_BATTLE_JOIN_SUCCESS, BUFFER_PACKET );
			battle_controller->Join_Battle( battle_index, &connection );
			Send_Map_Data_To_Player( connection, map_index );

			//  TODO:  Give battles an identifying ID (the hash of their name or the time of their creation, maybe?) so that the server can find them easily)
			//  For now, the LocationID will be -1, meaning you have to look for the correct lobby :(
			connection.CurrentLocation = PL_BATTLE;
			connection.LocationID = -1;
		}
	}

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


void GameServer::Process_Battle_Entry_Confirmation( ServerConnection& connection )
{
	Battle* containing_battle = BattleController::Get_Instance()->Get_Battle_Containing_Player( &connection );
	if ( containing_battle == NULL )
	{
		char message_id_string[128];
		sprintf_s( message_id_string, "Player with id [%d] is attempting to confirm battle entry when not within a battle.", connection.AccountID );
		ServerLogger.Add_Line( message_id_string );
	}

	connection.CurrentLocation = PL_BATTLE;
	containing_battle->Prepare_Players();
}


void GameServer::Process_Lobby_Chat_Message_Request( ServerConnection& connection )
{
	std::string chat_string( readstring( connection.MessageBuffer ) );
	if ( chat_string.empty() ) { return; }

	LobbyManager* lobby_manager = LobbyManager::Get_Instance();
	Lobby* containing_lobby = lobby_manager->Get_Lobby_Containing_Player( &connection );
	if ( containing_lobby == NULL )
	{
		ServerLogger.Add_Line( "Player not within a lobby is sending a lobby chat message." );
		return;
	}

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_CHAT_MESSAGE_RESPONSE, BUFFER_PACKET );

	writestring( chat_string.c_str(), BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	const Lobby::PlayerType* player_list = containing_lobby->Get_Player_List();
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( player_list[i] == NULL ) { continue; }
		sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


void GameServer::Process_Lobby_Start_Battle_Request( ServerConnection& connection )
{
	char failure_id = -1;
	Lobby* lobby = NULL;
	int battle_index = -1;
	LobbyManager* lobby_manager = LobbyManager::Get_Instance();

	if ( connection.CurrentLocation != PL_LOBBY )
	{
		failure_id = LOBBY_START_BATTLE_FAILURE_NOT_IN_LOBBY;
	}
	else
	{
		lobby = lobby_manager->Get_Lobby_Containing_Player( &connection );
		if ( lobby == NULL )
		{
			failure_id = LOBBY_START_BATTLE_FAILURE_NOT_IN_LOBBY;
		}
		else
		{
			//  TODO:  Eventually, the lobby will have a function to check if it has enough players
			//  to start the battle type it is set to. Check that function when it exists.
			//  For now, let's just make sure the lobby isn't just the one person
			if ( lobby->Get_Player_Count() <= 1 )
			{
				failure_id = LOBBY_START_BATTLE_FAILURE_NOT_ENOUGH_PLAYERS;
			}
			else
			{
				BattleController* battle_controller = BattleController::Get_Instance();
				battle_index = battle_controller->Create_New_Battle( lobby->Get_Lobby_Name(), BattleType(lobby->Get_Battle_Type()), lobby->Get_Map_Index(), PlayersType(lobby->Get_Players_Type()) );
				if ( battle_index == -1 )
				{
					failure_id = LOBBY_START_BATTLE_FAILURE_BATTLE_CREATION_FAILED;
				}
			}
		}
	}

	//  If we failed, inform the lobby leader
	if ( failure_id != -1 )
	{
		clearbuffer( BUFFER_PACKET );
		writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_START_BATTLE_RESPONSE, BUFFER_PACKET );
		
		writechar( failure_id, BUFFER_PACKET );

		clearbuffer( BUFFER_PACKET_WITH_SIZE );
		writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
		copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
		sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );

		return;
	}
	
	//  If we get this far, we succeeded, so we should just send everyone in the lobby into the battle
	lobby->Move_To_Battle( battle_index );
	lobby_manager->Remove_Lobby( lobby );
}


void GameServer::Process_Battle_Chat_Message_Request( ServerConnection& connection )
{
	std::string chat_string( readstring( connection.MessageBuffer ) );
	if ( chat_string.empty() ) { return; }

	BattleController* battle_controller = BattleController::Get_Instance();
	Battle* containing_battle = battle_controller->Get_Battle_Containing_Player( &connection );
	if ( containing_battle == NULL )
	{
		ServerLogger.Add_Line( "Player not within a battle is sending a battle chat message." );
		return;
	}

	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_CHAT_MESSAGE_RESPONSE, BUFFER_PACKET );

	writestring( chat_string.c_str(), BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	const Battle::PlayerType* player_list = containing_battle->Get_Player_List();
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( player_list[i] == NULL ) { continue; }
		sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


void GameServer::Process_Battle_Object_Move_Request( ServerConnection& connection )
{
	//  TODO:  Actually check whether the object can move to the spot, and that it isn't full
	int object_index = readint( connection.MessageBuffer );
	int column = readint( connection.MessageBuffer );
	int row = readint( connection.MessageBuffer );

	Battle* containing_battle = BattleController::Get_Instance()->Get_Battle_Containing_Player( &connection );
	bool move_successful = containing_battle->Move_Object( object_index, column, row );

	if ( move_successful )
	{
		clearbuffer( BUFFER_PACKET );
		writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_MOVED, BUFFER_PACKET );
		
		writeint( object_index, BUFFER_PACKET );
		writeint( column, BUFFER_PACKET );
		writeint( row, BUFFER_PACKET );

		clearbuffer( BUFFER_PACKET_WITH_SIZE );
		writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
		copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

		const Battle::PlayerType* player_list = containing_battle->Get_Player_List();
		for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
		{
			if ( player_list[i] == NULL ) { continue; }
			sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
		}
	}
}


void GameServer::Process_Battle_Object_Attack_Request( ServerConnection& connection )
{
	int attacker = readint( connection.MessageBuffer );
	int target = readint( connection.MessageBuffer );

	Battle* containing_battle = BattleController::Get_Instance()->Get_Battle_Containing_Player( &connection );
	bool attack_successful = containing_battle->Attack_Object( attacker, target );

	//  TODO:  Put in a case for when an object is only damaged, not destroyed
	//	Perhaps a unit status change message (damage, status, movement, etc)

	//  Send a message to all players saying that the target unit was destroyed
	if ( attack_successful )
	{
		//  Actually destroy the unit on the server side
		containing_battle->Destroy_Object( target );

		//  If there is a winner, find out and inform all players
		int winner = -1;
		if ( containing_battle->Check_For_Winner( winner ) )
		{
			containing_battle->Update_Players_Winner_Decided( winner );
		}
	}
}


void GameServer::Process_Battle_End_Turn_Request( ServerConnection& connection )
{
	//  Ensure that the sender is actually within a battle. Grab the battle.
	Battle* player_battle = BattleController::Get_Instance()->Get_Battle_Containing_Player( &connection );
	if ( player_battle == NULL )
	{
		std::string player_tag( "Player " );
		player_tag += connection.CharacterName;
		player_tag += " attempting to end a turn when no battle containing that character can be found.";
		ServerLogger.Add_Line( player_tag );
		return;
	}

	//  Ensure that the sender is actually the player whose turn it is.
	const int current_turn = player_battle->Get_Current_Turn();
	if ( player_battle->Get_Player_Account_ID( current_turn ) != connection.AccountID )
	{
		std::string turn_end_cheat_msg( "Account " );
		turn_end_cheat_msg += connection.AccountName;
		turn_end_cheat_msg += " may be attempting to cheat. Player sent End Turn message during someone else's turn.";
		ServerLogger.Add_Line( turn_end_cheat_msg );
		return;
	}

	//  We've gotten this far. End the current turn.
	player_battle->Next_Turn();
}


void GameServer::Process_Player_Leaving_Battle( ServerConnection& connection )
{
	BattleController* battle_controller = BattleController::Get_Instance();
	battle_controller->Remove_Player( &connection );

	connection.CurrentLocation = PL_NOLOCATION;
	connection.LocationID = -1;
}