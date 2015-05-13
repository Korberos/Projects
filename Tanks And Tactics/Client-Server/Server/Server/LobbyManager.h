#ifndef _LOBBY_MANAGER_H_
#define _LOBBY_MANAGER_H_

#include <vector>
#include "ServerConnection.h"

class Battle;

class Lobby
{
public:
	Lobby( const std::string &lobby_name, int battle_type, int map_index, int players_index );
	~Lobby();

	void Add_Player( ServerConnection* connection );
	bool Remove_Player( const ServerConnection* connection );
	bool Contains_Player( const ServerConnection* connection ) const;
	void Move_To_Battle( int battle_index );
	void Customize( const std::string& lobby_name, int battle_type, int map_index, int players_index );

	typedef ServerConnection* PlayerType;
	const int Get_Player_Count( void ) const { return PlayerCount; }
	const PlayerType* Get_Player_List( void ) const { return PlayerList; }
	bool Is_Full( void ) const { return PlayerCount >= MAX_INSTANCE_PLAYERS; }
	int Get_Lobby_Leader( void ) const { return LobbyLeader; }

	const std::string& Get_Lobby_Name( void ) const { return LobbyName; }
	const int Get_Battle_Type( void ) const { return BattleType; }
	const int Get_Map_Index( void ) const { return MapIndex; }
	const int Get_Players_Type( void ) const { return PlayersType; }
		
private:
	void New_Lobby_Leader( void );
	void Announce_New_Player( const int player_index );
	void Send_Player_List( const int player_index );

	PlayerType PlayerList[MAX_INSTANCE_PLAYERS];
	int PlayerCount;
	int LobbyLeader;

	std::string LobbyName;
	int BattleType;
	int MapIndex;
	int PlayersType;
};


class LobbyManager
{
public:
	static LobbyManager* Get_Instance( void );
	
	bool Customize_Lobby( ServerConnection* connection, const std::string& lobby_name, int battle_type, int map_index, int players_index, Lobby*& lobby );
	bool Join_Lobby( const int lobby_index, ServerConnection* connection );
	bool Join_Random_Lobby( ServerConnection* connection, Lobby*& return_lobby );
	void Remove_Player( ServerConnection* connection );
	bool Is_Player_In_Lobby( const ServerConnection* connection, int& lobby_index );
	Lobby* Get_Lobby_Containing_Player( const ServerConnection* connection );
	bool Remove_Lobby( const Lobby* lobby );

	const int Get_Lobby_Count( void ) const { return int(LobbyList.size()); }

private:
	LobbyManager();
	~LobbyManager();

	typedef std::vector< Lobby* > LobbyListType;
	LobbyListType LobbyList;
};

#endif // _LOBBY_MANAGER_H_