#include "LobbyManager.h"

#include "DebugTools.h"
#include "BattleController.h"

Lobby::Lobby( const std::string &lobby_name, int battle_type, int map_index, int players_index ) : 
	LobbyName( lobby_name ),
	BattleType( battle_type ),
	MapIndex( map_index ),
	PlayersType( players_index ),
	PlayerCount( 0 ),
	LobbyLeader( -1 )
{
	memset( PlayerList, 0, sizeof( PlayerType ) * MAX_INSTANCE_PLAYERS );
}


Lobby::~Lobby()
{
}


void Lobby::Add_Player( ServerConnection* connection )
{
	FAIL_IF ( "Too many players attempting to join a lobby." && PlayerCount >= MAX_INSTANCE_PLAYERS ) { return; }

	//  Find the first free player slot and fill it
	int new_player_index = -1;
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL )
		{
			PlayerList[i] = connection;
			new_player_index = i;
			break;
		}
	}

	FAIL_IF ( "No open spot could be found for the new player." && new_player_index == -1 ) { return; }
	++PlayerCount;

	//  If there is no lobby leader, set the new player to the leader
	if ( LobbyLeader == -1 )
	{
		LobbyLeader = new_player_index;
	}
	
	//  Inform all players in the lobby of the new player
	Announce_New_Player( new_player_index );

	//  Send the new player the player list for the lobby
	Send_Player_List( new_player_index );
}


bool Lobby::Remove_Player( const ServerConnection* connection )
{
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == connection )
		{
			int socket = PlayerList[i]->Socket;
			// PlayerList[i]->CurrentLocation = PL_NOLOCATION;
			// PlayerList[i]->LocationID = -1;
			PlayerList[i] = NULL;

			--PlayerCount;

			//  If the player is the lobby leader, make the next player the leader
			if ( LobbyLeader == i )
			{
				New_Lobby_Leader();
			}

			return true;
		}
	}

	return false;
}


bool Lobby::Contains_Player( const ServerConnection* connection ) const
{
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == connection ) { return true; }
	}
	
	return false;
}


void Lobby::Move_To_Battle( int battle_index )
{
	BattleController* battle_controller = BattleController::Get_Instance();
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			//  TODO:  Fix this shit. I need a defined method for sending battle confirmation to all players
			battle_controller->Join_Battle( battle_index, PlayerList[i] );
		}
	}
}


void Lobby::Customize( const std::string& lobby_name, int battle_type, int map_index, int players_index )
{
	LobbyName = lobby_name;
	BattleType = battle_type;
	MapIndex = map_index;
	PlayersType = players_index;
}


void Lobby::New_Lobby_Leader( void )
{
	//  TODO:  Write a system for determining who the last player to commit an action is within the lobby. This will decrease the chances of the new leader being AFK.
	LobbyLeader = -1;
	if ( PlayerCount == 0 ) { return; }

	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			LobbyLeader = i;
		}
	}

	FAIL_IF ( LobbyLeader == -1 ) { return; }
	
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_NEW_LEADER, BUFFER_PACKET );
	
	writeint( LobbyLeader, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }
		sendmessage( PlayerList[i]->Socket, PlayerList[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


void Lobby::Announce_New_Player( const int player_index )
{
	//  If the new player is the only player in the lobby, ignore the announcement
	if ( PlayerCount <= 1 ) { return; }

	//  Send an announcement of the new player to any pre-existing players
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_JOINED, BUFFER_PACKET );
	
	writeint( player_index, BUFFER_PACKET );
	writechar( LOBBY_PLAYER_EXISTS | !LOBBY_PLAYER_LOCAL | !LOBBY_PLAYER_LEADER, BUFFER_PACKET );
	writestring( PlayerList[player_index]->CharacterName.c_str(), BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }
		if ( i == player_index ) { continue; }
		sendmessage( PlayerList[i]->Socket, PlayerList[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


void Lobby::Send_Player_List( const int player_index )
{
	//  Send the list of all players in the lobby, including the receiver (who just joined)
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_LIST, BUFFER_PACKET );

	writeint( PlayerCount, BUFFER_PACKET );

	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }

		writeint( i, BUFFER_PACKET );
		writestring( PlayerList[i]->CharacterName.c_str(), BUFFER_PACKET );
	}

	writeint( LobbyLeader, BUFFER_PACKET );
	writeint( player_index, BUFFER_PACKET );
	
	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	
	sendmessage( PlayerList[player_index]->Socket, PlayerList[player_index]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}


LobbyManager* LobbyManager::Get_Instance( void )
{
	static LobbyManager INSTANCE;
	return &INSTANCE;
}


bool LobbyManager::Customize_Lobby( ServerConnection* connection, const std::string &lobby_name, int battle_type, int map_index, int players_index, Lobby*& lobby )
{
	//  TODO:  Sanity check the values
	if ( false /*sanity check failure*/ ) { return false; }

	//  If the connection is already attached to a lobby, customize that lobby. Otherwise, create a new lobby
	if ( connection->CurrentLocation == PL_LOBBY )
	{
		lobby = Get_Lobby_Containing_Player( connection );
		//  TODO:  output an error instead of asserting!
		ENFORCED_IF ( lobby != NULL )
		{
			lobby->Customize( lobby_name, battle_type, map_index, players_index );
		}
	}

	//  If a lobby already exists with this name, return false
	//  TODO:  Return an error code
	for ( LobbyListType::iterator lobby_iter = LobbyList.begin(); lobby_iter != LobbyList.end(); ++lobby_iter )
	{
		if ( (*lobby_iter)->Contains_Player( connection ) )
		{
			return false;
		}
	}

	Lobby* new_lobby = new Lobby( lobby_name, battle_type, map_index, players_index );
	new_lobby->Add_Player( connection );
	
	//  TODO:  Give lobbies an identifying ID (the hash of their name or the time of their creation, maybe?) so that the server can find them easily)
	//  For now, the LocationID will be -1, meaning you have to look for the correct lobby :(
	connection->CurrentLocation = PL_LOBBY;
	connection->LocationID = -1;

	LobbyList.push_back( new_lobby );
	return true;
}


//  TODO:  WHEREVER THIS ENDS UP BEING CALLED, REMEMBER TO SET THE CURRENT LOCATION TO PL_LOBBY
bool LobbyManager::Join_Lobby( const int lobby_index, ServerConnection* connection )
{
	//  Check that the given lobby exists
	if ( lobby_index >= int(LobbyList.size()) )
	{
		return false;
	}

	//  Check if the player it in another lobby. If so, make him exit that lobby.
	int existing_lobby_index = -1;
	if ( Is_Player_In_Lobby( connection, existing_lobby_index ) )
	{
		Lobby* remove_lobby = LobbyList[existing_lobby_index];
		remove_lobby->Remove_Player( connection );
	}

	//  Grab the appropriate lobby and add the player to it
	Lobby* lobby = LobbyList[lobby_index];
	lobby->Add_Player( connection );
	
	//  TODO:  Give lobbies an identifying ID (the hash of their name or the time of their creation, maybe?) so that the server can find them easily)
	//  For now, the LocationID will be -1, meaning you have to look for the correct lobby :(
	connection->CurrentLocation = PL_LOBBY;
	connection->LocationID = -1;

	return true;
}


bool LobbyManager::Join_Random_Lobby( ServerConnection* connection, Lobby*& return_lobby )
{
	//  Check if the player it in another lobby. If so, make him exit that lobby.
	int existing_lobby_index = -1;
	if ( Is_Player_In_Lobby( connection, existing_lobby_index ) )
	{
		Lobby* remove_lobby = LobbyList[existing_lobby_index];
		remove_lobby->Remove_Player( connection );
	}

	for ( LobbyListType::iterator iter = LobbyList.begin(); iter != LobbyList.end(); ++iter )
	{
		if ( (*iter)->Is_Full() ) { continue; }

		(*iter)->Add_Player( connection );
		return_lobby = (*iter);
		return true;
	}

	return false;
}


void LobbyManager::Remove_Player( ServerConnection* connection )
{
	for ( LobbyListType::iterator lobby_iter = LobbyList.begin(); lobby_iter != LobbyList.end(); ++lobby_iter )
	{
		if ( (*lobby_iter)->Contains_Player( connection ) == false ) { continue; }
		ASSERT( (*lobby_iter)->Remove_Player( connection ) );

		if ( (*lobby_iter)->Get_Player_Count() == 0 )
		{
			LobbyList.erase( lobby_iter );
		}

		return;
	}
}


bool LobbyManager::Is_Player_In_Lobby( const ServerConnection* connection, int& lobby_index )
{
	for ( LobbyListType::iterator lobby_iter = LobbyList.begin(); lobby_iter != LobbyList.end(); ++lobby_iter )
	{
		if ( (*lobby_iter)->Contains_Player( connection ) )
		{
			//  TODO:  Return an index once we have a set list that doesn't rearrange
			return true;
		}
	}

	return false;
}


Lobby* LobbyManager::Get_Lobby_Containing_Player( const ServerConnection* connection )
{
	for ( LobbyListType::iterator lobby_iter = LobbyList.begin(); lobby_iter != LobbyList.end(); ++lobby_iter )
	{
		if ( (*lobby_iter)->Contains_Player( connection ) )
		{
			return (*lobby_iter);
		}
	}

	return NULL;
}


bool LobbyManager::Remove_Lobby( const Lobby* lobby )
{
	for ( LobbyListType::iterator lobby_iter = LobbyList.begin(); lobby_iter != LobbyList.end(); ++lobby_iter )
	{
		if ( (*lobby_iter) == lobby )
		{
			LobbyList.erase( lobby_iter );
			return true;
		}
	}

	return false;
}


LobbyManager::LobbyManager()
{
}


LobbyManager::~LobbyManager()
{
}
