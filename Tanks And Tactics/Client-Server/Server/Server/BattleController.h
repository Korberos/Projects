#ifndef _BATTLE_CONTROLLER_
#define _BATTLE_CONTROLLER_

#include <string>
#include <vector>
#include "Battle.h"


class BattleController
{
public:
	static BattleController* Get_Instance( void );

	int Get_Battle_Count( void )												{ return int( BattleList.size() ); }
	int Get_Battle_Map_Index( int index )									{ if ( index < 0 || index >= Get_Battle_Count() ) return -1; return BattleList[index].Get_Map_Identifier(); }
	int Get_Player_Count( int battle_index )								{ if ( battle_index < 0 || battle_index >= Get_Battle_Count() ) return -1; return BattleList[battle_index].Get_Player_Count(); }
	const Battle::PlayerType* Get_Player_List( int battle_index )	{ if ( battle_index < 0 || battle_index >= Get_Battle_Count() ) return NULL; return BattleList[battle_index].Get_Player_List(); }

	int Create_New_Battle( const std::string& battle_name, const BattleType battle_type, const int map_index, const PlayersType players );
	bool Destroy_Battle( const Battle* battle );
	bool Join_Battle( int battle_index, const ServerConnection* connection );
	Battle* Get_Battle_Containing_Player( const ServerConnection* connection );
	bool Remove_Player( const ServerConnection* connection );

	typedef std::vector< std::pair< std::string, std::string > > MapListType;
	typedef MapListType::iterator MapListIteratorType;
	static MapListType MapList;

private:
	BattleController() {}
	~BattleController() {}

	typedef std::vector< Battle > BattleListType;
	BattleListType BattleList;
};

#endif