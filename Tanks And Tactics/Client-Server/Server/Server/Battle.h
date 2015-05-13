#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <vector>
#include <string>
#include <algorithm>
#include "ServerConnection.h"

//  Battle Object - the managed objects (units, etc.) in the battle
class BattleObject
{
public:
	BattleObject( const int owner_index, const std::string name, const int type, const int level, const int experience, const int max_health, const int attack, const int speed );
	~BattleObject();

	const int Get_Owner_Index( void )			const { return OwnerIndex; }
	const std::string& Get_Unit_Name( void )	const { return UnitName; }
	const int Get_Unit_Type( void )				const { return UnitType; }
	const int Get_Unit_Level( void )				const { return UnitLevel; }
	const int Get_Unit_Experience( void )		const { return UnitExperience; }
	const int Get_Unit_Max_Health( void )		const { return UnitMaxHealth; }
	const int Get_Unit_Attack( void )			const { return UnitAttack; }
	const int Get_Unit_Speed( void )				const { return UnitSpeed; }
	const int Get_X( void )							const { return BattlePosition.first; }
	const int Get_Y( void )							const { return BattlePosition.second; }
	const bool Get_Exhausted( int index )		const { if ( index < 0 || index > EXHAUSTION_TYPE_COUNT ) { return false; } return Exhausted[index]; }
	const int Get_Health( void )					const { return Health; }

	void Set_Position( int column, int row )			{ BattlePosition.first = column; BattlePosition.second = row; }
	void Set_Exhausted( int index, bool value )		{ if ( index < 0 || index > EXHAUSTION_TYPE_COUNT ) { return; } Exhausted[index] = value; }
	void Set_Health( int health )							{ Health = min( max( health, 0 ), UnitMaxHealth ); }

private:
	const int OwnerIndex;
	const std::string UnitName;
	const int UnitType;
	const int UnitLevel;
	const int UnitExperience;
	const int UnitMaxHealth;
	const int UnitAttack;
	const int UnitSpeed;

	std::pair< int, int > BattlePosition;
	bool Exhausted[EXHAUSTION_TYPE_COUNT];
	int Health;
};


//  Battle - the controller of actions within the battle
class Battle
{
public:
	Battle( const std::string battle_name, const int battle_type, const int map_index, const int players );
	Battle( const Battle& other );
	~Battle();
	
	void Service( void );
	bool Can_Add_Player( int& index );
	void Add_Player( const int player_index, const ServerConnection* connection );
	bool Remove_Player( const std::string& player_name, int& response );
	void Prepare_Players( void );
	bool Move_Object( int object_index, int column, int row );
	bool Attack_Object( int attacker, int target );
	void Destroy_Object( int object_index );
	void Next_Turn( bool check_remaining_units = true );
	bool Load_In_Units( const int player_index );

	void Update_Players_Destroyed_Object( const int object_index ) const;
	void Update_Players_Winner_Decided( const int winner ) const;

	typedef const ServerConnection* PlayerType;
	const PlayerType* Get_Player_List( void )		const { return PlayerList; }
	const int Get_Player_Count( void )				const { return PlayerCount; }
	const int Get_Map_Identifier( void )			const { return MapIdentifier; }
	const int Get_Current_Turn( void )				const { return PlayerTurn; }
	const int Get_Player_Account_ID( int index )	const { return PlayerList[index]->AccountID; }
	
	bool Contains_Player( const ServerConnection* connection ) const;
	bool Check_For_Winner( int& winning_player );

private:
	void Populate( void );
	void Wipe_Out_Player( const int player_index );

	bool All_Players_Joined( void );

	void Phase_Upkeep( void );		//  All upkeep occurs. Counters tick and payment is taken if needed.
	void Phase_Moving( void );		//  All movement from the player is sent to all other players.
	void Phase_Action( void );		//  All actions from the player are sent to all other players.
	void Phase_Status( void );		//  All statuses are updated (poison hurts objects, etc)
	void Phase_Result( void );		//  All results are calculated. Objects are removed if necessary.

	PlayerType PlayerList[MAX_INSTANCE_PLAYERS];
	int PlayerCount;

	typedef std::vector< BattleObject* > ObjectListType;
	ObjectListType ObjectList;
	int PlayerTurn;

	//  TODO:  Find out why I can't make these const without the error appearing
	/*const */std::string BattleName;
	/*const */int BattleType;
	/*const */int PlayersType;
	/*const */int MapIdentifier;

	static enum DIRTY_FLAGS { PLAYERS_DIRTY, OBJECTS_DIRTY, DIRTY_FLAG_COUNT };
	std::vector< bool > DirtyFlags;

	static enum BATTLE_PHASE { PHASE_WAITING_FOR_PLAYERS, PHASE_POPULATE, PHASE_UPKEEP, PHASE_MOVING, PHASE_ACTION, PHASE_STATUS, PHASE_RESULT, PHASE_COMPLETE, BATTLE_PHASE_COUNT };
	int BattlePhase;

	static enum INTERACTION_RESPONSES { RESPONSE_PLAYER_ALREADY_EXISTS_IN_GAME, RESPONSE_PLAYER_DOES_NOT_EXIST_IN_GAME, INTERACTION_RESPONSE_COUNT };
};

#endif // _BATTLE_H_