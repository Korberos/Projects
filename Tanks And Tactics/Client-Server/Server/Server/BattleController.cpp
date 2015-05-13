#include "BattleController.h"

#include "DebugTools.h"
#include "MapData.h"

BattleController::MapListType BattleController::MapList;

BattleController* BattleController::Get_Instance( void )
{
	static BattleController INSTANCE;
	return &INSTANCE;
}


int BattleController::Create_New_Battle( const std::string& battle_name, const BattleType battle_type, const int map_index, const PlayersType players )
{
	//  Sanity check ( TODO:  Change these from an assert to a log file )
	FAIL_IF (  battle_name.empty() == true ) { return -1; }
	FAIL_IF ( battle_type < 0 || battle_type >= BATTLE_TYPE_COUNT ) { return -1; }
	FAIL_IF ( map_index < 0 ) { return -1; }
	FAIL_IF ( players < 0 || players >= PLAYER_TYPE_COUNT ) { return -1; }

	//  TODO:  Sanity check the information.
	//  The name should be valid and the map index should lead to a valid map

	//  Create the battle and add it to the vector. Return success.
	BattleList.push_back( Battle( battle_name, battle_type, map_index, players ) );
	return int(BattleList.size() - 1);
}


bool BattleController::Destroy_Battle( const Battle* battle )
{
	for ( BattleListType::iterator iter = BattleList.begin(); iter != BattleList.end(); ++iter )
	{
		if ( &(*iter) == battle )
		{
			BattleList.erase( iter );
			return true;
		}
	}

	return false;
}


bool BattleController::Join_Battle( int battle_index, const ServerConnection* connection )
{
	int player_index = -1;
	if ( BattleList[battle_index].Can_Add_Player( player_index ) )
	{
		//  Inform the player of the battle map they should be loading
		clearbuffer( BUFFER_PACKET );
		writechar( SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_SUMMONS, BUFFER_PACKET );
		
		Send_Map_Data_To_Player( (*connection), BattleList[battle_index].Get_Map_Identifier() );

		clearbuffer( BUFFER_PACKET_WITH_SIZE );
		writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
		copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
		sendmessage( connection->Socket, connection->IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );

		BattleList[battle_index].Add_Player( player_index, connection );

		return true;
	}

	return false;
}


Battle* BattleController::Get_Battle_Containing_Player( const ServerConnection* connection )
{
	for ( BattleListType::iterator battle_iter = BattleList.begin(); battle_iter != BattleList.end(); ++battle_iter )
	{
		if ( (*battle_iter).Contains_Player( connection ) )
		{
			return &(*battle_iter);
		}
	}

	return NULL;
}


bool BattleController::Remove_Player( const ServerConnection* connection )
{
	Battle* containing_battle = Get_Battle_Containing_Player( connection );
	FAIL_IF ( containing_battle == NULL ) { return false; }

	int response = -1;
	FAIL_IF ( containing_battle->Remove_Player( connection->CharacterName, response ) == false ) { return false; }

	if ( containing_battle->Get_Player_Count() == 0 )
	{
		return Destroy_Battle( containing_battle );
	}

	return true;
}