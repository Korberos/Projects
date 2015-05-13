#include "Battle.h"

#include "DebugTools.h"
#include "RandomNumbers.h"
#include "SQL_Wrapper.h"
#include "GameServer.h"


BattleObject::BattleObject( const int owner_index, const std::string name, const int type, const int level, const int experience, const int max_health, const int attack, const int speed ) : 
	OwnerIndex( owner_index ),
	UnitName( name ),
	UnitType( type ),
	UnitLevel( level ),
	UnitExperience( experience ),
	UnitMaxHealth( max_health ),
	UnitAttack( attack ),
	UnitSpeed( speed ),
	BattlePosition( -1, -1 ),
	Health( UnitMaxHealth )
{
	for ( int i = 0; i < EXHAUSTION_TYPE_COUNT; ++i )
	{
		Exhausted[i] = false;
	}
}


Battle::Battle( const std::string battle_name, const int battle_type, const int map_index, const int players ) : 
	PlayerCount( 0 ),
	PlayerTurn( -1 ),
	BattleName( battle_name ),
	BattleType( battle_type ),
	PlayersType( players ),
	MapIdentifier( map_index ),
	BattlePhase( PHASE_WAITING_FOR_PLAYERS )
{
	memset( PlayerList, NULL, sizeof( PlayerType ) * MAX_INSTANCE_PLAYERS );
	ObjectList.clear();
	DirtyFlags.clear();
}


Battle::Battle( const Battle& other ) : 
	PlayerCount( 0 ),
	PlayerTurn( -1 ),
	BattleName( other.BattleName ),
	BattleType( other.BattleType ),
	PlayersType( other.PlayersType ),
	MapIdentifier( other.MapIdentifier ),
	BattlePhase( PHASE_WAITING_FOR_PLAYERS )
{
	memset( PlayerList, NULL, sizeof( PlayerType ) * MAX_INSTANCE_PLAYERS );
	ObjectList.clear();
	DirtyFlags.clear();
}


Battle::~Battle()
{
}


void Battle::Service( void )
{
	//  Check the current phase, and return return whether the battle has ended
	switch ( BattlePhase )
	{
	case PHASE_WAITING_FOR_PLAYERS:
		{
			Prepare_Players();
			break;
		}

	case PHASE_POPULATE:
		{
			Next_Turn( false );
			Populate();
			break;
		}

	case PHASE_UPKEEP:
		{
			Phase_Upkeep();
			break;
		}

	case PHASE_MOVING:
		{
			Phase_Moving();
			BattlePhase = PHASE_ACTION;
			break;
		}

	case PHASE_ACTION:
		{
			Phase_Action();
			BattlePhase = PHASE_STATUS;
			break;
		}

	case PHASE_STATUS:
		{
			Phase_Status();
			BattlePhase = PHASE_RESULT;
			break;
		}

	case PHASE_RESULT:
		{
			Phase_Result();

			int winning_player = -1;
			if ( Check_For_Winner( winning_player ) == true )
			{
				//  TODO:  Send the information about who won (and maybe battle stats)
				return;
			}

			Next_Turn();
			break;
		}
	}
}


bool Battle::Can_Add_Player( int& index )
{
	FAIL_IF ( "Too many players within a battle." && PlayerCount > MAX_INSTANCE_PLAYERS ) { return false; }
	if ( PlayerCount == MAX_INSTANCE_PLAYERS ) { return false; }

	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL )
		{
			index = i;
			return true;
		}
	}

	return false;
}


void Battle::Add_Player( const int player_index, const ServerConnection* connection )
{
	//  Note: Can_Add_Player should be called prior to
	//  this, so this function assumes it can not fail!
	FAIL_IF ( player_index < 0 ) { return; }
	FAIL_IF ( player_index >= MAX_INSTANCE_PLAYERS ) { return; }
	FAIL_IF ( PlayerList[player_index] != NULL ) { return; }

	PlayerList[player_index] = connection;
	++PlayerCount;

	//  Call the Service() function. It should move out of the WAITING_FOR_PLAYERS state if all players are in.
	Service();
}


bool Battle::Remove_Player( const std::string& player_name, int& response )
{
	//  Determine if the player exists in this game...
	int player_index = -1;
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }
		if ( PlayerList[i]->CharacterName.compare( player_name ) == 0 )
		{
			player_index = i;
			break;
		}
	}

	//  If not, send back a response
	if ( player_index == -1 )
	{
		response = RESPONSE_PLAYER_DOES_NOT_EXIST_IN_GAME;
		return false;
	}

	Wipe_Out_Player( player_index );

	return true;
}


void Battle::Prepare_Players( void )
{
	if ( All_Players_Joined() )
	{
		BattlePhase = PHASE_POPULATE;
		Service();
	}
}


bool Battle::Move_Object( int object_index, int column, int row )
{
	//  Ensure that the moving object exists. Grab it.
	if ( object_index < 0 ) { return false; }
	if ( object_index > int(ObjectList.size()) ) { return false; }
	BattleObject* moving_object = ObjectList[object_index];

	//  If another object is in e chosen space, return false
	for ( ObjectListType::iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter )
	{
		if ( (*iter)->Get_X() == column && (*iter)->Get_Y() == row ) { return false; }
	}

	//  If the object is move exhausted, return false
	if ( moving_object->Get_Exhausted( MOVE_EXHAUSTION ) ) { return false; }

	//  If the space is farther away than the unit's range, return false
	int x_distance = abs( moving_object->Get_X() - column );
	int y_distance = abs( moving_object->Get_Y() - row );
	if ( x_distance + y_distance > moving_object->Get_Unit_Speed() ) { return false; }

	//  Exhaust the object
	moving_object->Set_Exhausted( MOVE_EXHAUSTION, true );

	moving_object->Set_Position( column, row );

	return true;
}


bool Battle::Attack_Object( int attacker, int target )
{
	//  Ensure that the attacking object exists. Grab it.
	if ( attacker < 0 ) { return false; }
	if ( attacker > int(ObjectList.size()) ) { return false; }
	BattleObject* attacker_object = ObjectList[attacker];

	//  If the object is attack exhausted, return false
	if ( attacker_object->Get_Exhausted( ATTACK_EXHAUSTION ) ) { return false; }

	//  Ensure that the target object exists. Grab it.
	if ( target < 0 ) { return false; }
	if ( target > int(ObjectList.size()) ) { return false; }
	BattleObject* target_object = ObjectList[target];

	//  Ensure that the attacker object belongs to the player whose turn it is
	if ( attacker_object->Get_Owner_Index() != Get_Current_Turn() ) { return false; }

	//  Ensure that the target is not on the same team
	if ( target_object->Get_Owner_Index() == Get_Current_Turn() ) { return false; }

	//  Ensure that the two objects are close enough for an attack
	//  TODO:  compare the distances with the unit's RANGE instead of 1
	int x_distance = abs( attacker_object->Get_X() - target_object->Get_X() );
	int y_distance = abs( attacker_object->Get_Y() - target_object->Get_Y() );
	if ( x_distance + y_distance > 1 ) { return false; }
	
	//  Exhaust the object
	attacker_object->Set_Exhausted( ATTACK_EXHAUSTION, true );

	return true;
}


void Battle::Destroy_Object( int object_index )
{
	FAIL_IF ( object_index < 0 ) { return; }
	FAIL_IF ( object_index > int(ObjectList.size()) ) { return; }

	int i = 0;
	for ( ObjectListType::iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter, ++i )
	{
		if ( i == object_index )
		{
			ObjectList.erase( iter );
			break;
		}
	}

	//  Update all players
	Update_Players_Destroyed_Object( object_index );
}


void Battle::Next_Turn( bool check_remaining_units )
{
	//  Loop through the list of Players until we find the next available player
	while ( true )
	{
		++PlayerTurn;
		if ( PlayerTurn >= MAX_INSTANCE_PLAYERS )
		{
			PlayerTurn = 0;
		}
		if ( PlayerList[PlayerTurn] != NULL )
		{
			break;
		}
	}

	//  Remove any exhaustion on the units owned by the player whose turn it is
	bool has_remaining_units = false;
	for ( ObjectListType::iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter )
	{
		if ( (*iter)->Get_Owner_Index() != PlayerTurn ) { continue; }
		has_remaining_units = true;
		for ( int i = 0; i < EXHAUSTION_TYPE_COUNT; ++i )
		{
			(*iter)->Set_Exhausted( i, false );
		}
	}

	if ( check_remaining_units && !has_remaining_units )
	{
		Next_Turn();
		return;
	}

	//  Send a message to all players informing them that the next turn is starting
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			clearbuffer( BUFFER_PACKET );
			writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_NEXT_TURN, BUFFER_PACKET );

			writeint( PlayerTurn, BUFFER_PACKET );

			clearbuffer( BUFFER_PACKET_WITH_SIZE );
			writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
			copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
			sendmessage( PlayerList[i]->Socket, PlayerList[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
		}
	}

	//  Move to the upkeep phase of the new turn
	BattlePhase = PHASE_UPKEEP;
}


bool Battle::Load_In_Units( const int player_index )
{
	FAIL_IF ( player_index < 0 || player_index >= MAX_INSTANCE_PLAYERS ) { return false; }
	FAIL_IF ( PlayerList[player_index] == NULL ) { return false; }

	char sql_command[1024];
	sprintf_s( sql_command, 1024, "SELECT unit_name, unit_type, unit_level, unit_experience, unit_max_health, unit_attack, unit_speed FROM game_units WHERE character_id=%d LIMIT %d;", PlayerList[player_index]->CharacterID, 5 );
	PGresult* result = PQexec( GameServer::Get_Instance()->Get_Database_Connection(), sql_command );
	if ( PQresultStatus( result ) != PGRES_TUPLES_OK )
	{
		//  We can't access a logger from here, so just clear the results and return false
		PQclear( result );
		return false;
   }

	//  Create a battle object for each unit and add it to the battle with the given information
	BattleObject* new_object = NULL;
	for ( unsigned int i = 0; i < 5; ++i )
	{
		const std::string unit_name( PQgetvalue( result, i, 0 ) );
		const int unit_type( atoi( PQgetvalue( result, i, 1 ) ) );
		const int unit_level( atoi( PQgetvalue( result, i, 2 ) ) );
		const int unit_experience( atoi( PQgetvalue( result, i, 3 ) ) );
		const int unit_max_health( atoi( PQgetvalue( result, i, 4 ) ) );
		const int unit_attack( atoi( PQgetvalue( result, i, 5 ) ) );
		const int unit_speed( atoi( PQgetvalue( result, i, 6 ) ) );

		//  TODO:  Sanity check the information if possible

		new_object = new BattleObject( player_index, unit_name, unit_type, unit_level, unit_experience, unit_max_health, unit_attack, unit_speed );
		new_object->Set_Position( Random_Integer( 0, 39 ), Random_Integer( 0, 39 ) );
		ObjectList.push_back( new_object );
	}

	return true;
}


void Battle::Update_Players_Destroyed_Object( const int object_index ) const
{
	//  Send a message about the destruction of the object to all battle players
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_DESTROYED, BUFFER_PACKET );
	
	writeint( object_index, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	const PlayerType* player_list = Get_Player_List();
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( player_list[i] == NULL ) { continue; }
		sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


void Battle::Update_Players_Winner_Decided( const int winner ) const
{
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_WINNER, BUFFER_PACKET );
	
	writeint( winner, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	const PlayerType* player_list = Get_Player_List();
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( player_list[i] == NULL ) { continue; }
		sendmessage( player_list[i]->Socket, player_list[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
	}
}


bool Battle::Contains_Player( const ServerConnection* connection ) const
{
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }
		if ( PlayerList[i]->CharacterName.compare( connection->CharacterName ) == 0 )
		{
			return true;
		}
	}

	return false;
}


bool Battle::Check_For_Winner( int& winning_player )
{
	for ( std::vector< BattleObject* >::iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter )
	{
		if ( winning_player == -1 )
		{
			winning_player = (*iter)->Get_Owner_Index();
			continue;
		}

		if ( winning_player != (*iter)->Get_Owner_Index() )
		{
			return false;
		}
	}

	return true;
}


void Battle::Populate( void )
{
	//  TODO:  Load the players' loadouts from the database instead of just creating
	//  a random array of units

	//  Send a message to each player with their local ID
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			clearbuffer( BUFFER_PACKET );
			writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_PLAYER_INDEX, BUFFER_PACKET );

			writeint( i, BUFFER_PACKET );

			clearbuffer( BUFFER_PACKET_WITH_SIZE );
			writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
			copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
			sendmessage( PlayerList[i]->Socket, PlayerList[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
		}
	}

	//  Load in all units for all players in the game
	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			Load_In_Units( i );
		}
	}

	//  Begin creating a message to send to all players
	clearbuffer( BUFFER_PACKET );
	writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_CREATED, BUFFER_PACKET );

	const int unit_count = 5;
	writeint( PlayerCount * unit_count, BUFFER_PACKET );

	//  Send the unit data to all players
	for ( int i = 0; i < unit_count * PlayerCount; ++i )
	{
		writeint( ObjectList[i]->Get_Owner_Index(), BUFFER_PACKET );
		writestring( ObjectList[i]->Get_Unit_Name().c_str(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Type(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Level(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Experience(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Max_Health(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Attack(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Unit_Speed(), BUFFER_PACKET );

		writeint( ObjectList[i]->Get_X(), BUFFER_PACKET );
		writeint( ObjectList[i]->Get_Y(), BUFFER_PACKET );
	}

	//  Send the message to all players
	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );

	for ( unsigned int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] != NULL )
		{
			sendmessage( PlayerList[i]->Socket, PlayerList[i]->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
		}
	}

	BattlePhase = PHASE_UPKEEP;
	Service();
}


void Battle::Wipe_Out_Player( const int player_index )
{
	//  Go through all objects and find any object owned by the given player
	std::vector< int > destruction_list;
	int i = 0;
	int destroyed = 0;
	for ( ObjectListType::iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter, ++i )
	{
		if ( (*iter)->Get_Owner_Index() != player_index ) { continue; }
		destruction_list.push_back( i - destroyed );
		++destroyed;
	}

	//  Remove the leaving player
	PlayerList[player_index] = NULL;
	--PlayerCount;

	//  Go through all destruction objects and destroy them
	for ( std::vector< int >::iterator iter = destruction_list.begin(); iter != destruction_list.end(); ++iter )
	{
		Destroy_Object( (*iter) );
	}

	//  If there is a winner, find out and inform all players
	int winner = -1;
	if ( Check_For_Winner( winner ) )
	{
		Update_Players_Winner_Decided( winner );
	}
}


bool Battle::All_Players_Joined( void )
{
	int players = 0;
	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i] == NULL ) { continue; }
		if ( PlayerList[i]->CurrentLocation == PL_BATTLE ) { ++players; }
	}
	ASSERT ( players <= PlayerCount );

	FAIL_IF ( players < 0 ) { return false; }
	switch ( PlayersType )
	{
	case PLAYER_TYPE_1_V_1:
		FAIL_IF ( players > 2 ) { return false; }
		return ( players == 2 );
		break;

	case PLAYER_TYPE_2_V_2:
	case PLAYER_TYPE_4_PLAYER_MELEE:
		FAIL_IF ( players > 4 ) { return false; }
		return ( players == 4 );
		break;

	case PLAYER_TYPE_3_PLAYER_MELEE:
		FAIL_IF ( players > 3 ) { return false; }
		return ( players == 3 );
		break;
	}

	return false;
}


void Battle::Phase_Upkeep( void )
{
	//  TODO:  Upkeep all upkeeps. Spend points, count down/up on timers, etc.

	BattlePhase = PHASE_MOVING;
}


void Battle::Phase_Moving( void )
{
	//  TODO:  Update all movement. Send it to the players.

	BattlePhase = PHASE_ACTION;
}


void Battle::Phase_Action( void )
{
	//  TODO:  Update all actions. Send them to the players.

	BattlePhase = PHASE_STATUS;
}


void Battle::Phase_Status( void )
{
	//  TODO:  Update all statuses. Healing effects, poision, etc.

	BattlePhase = PHASE_RESULT;
}


void Battle::Phase_Result( void )
{
	//  TODO:  Determine the results of the round. Kill off objects, etc.
}