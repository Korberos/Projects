#ifndef _GAME_SERVER_
#define _GAME_SERVER_

#pragma comment(lib, "libpq.lib")
#include <libpq-fe.h>

#include <vector>
#include <list>
#include "ServerConnection.h"

class GameServer
{
public:
	static GameServer* Get_Instance( void );

	bool Initialize( void );
	void Service( void );

	PGconn* Get_Database_Connection( void ) { return DatabaseConnection; }

private:
	GameServer();
	~GameServer();

	void LoadMapList( void );

	void Accept_New_Connections( void );
	void Receive_Messages( void );
	void Remove_Connection( ServerConnection& connection );
	void Send_News_To_Player( ServerConnection& connection, int number_of_entries );
	void Register_Account( ServerConnection& connection );
	void Login_Account( ServerConnection& connection );
	void Send_Character_Data_To_Player( ServerConnection& connection );
	void Create_Character( ServerConnection& connection );
	void Delete_Character( ServerConnection& connection );
	bool Create_Starter_Loadout( ServerConnection& connection, int character_id, const char* char_name );
	void Choose_Character( ServerConnection& connection );
	void Send_Map_List_To_Player( ServerConnection& connection );
	void Process_Custom_Lobby_Request( ServerConnection& connection );
	void Process_Random_Lobby_Join_Request( ServerConnection& connection );
	void Process_Random_Battle_Join_Request( ServerConnection& connection );
	void Process_Battle_Entry_Confirmation( ServerConnection& connection );
	void Process_Lobby_Chat_Message_Request( ServerConnection& connection );
	void Process_Lobby_Start_Battle_Request( ServerConnection& connection );
	void Process_Battle_Chat_Message_Request( ServerConnection& connection );
	void Process_Battle_Object_Move_Request( ServerConnection& connection );
	void Process_Battle_Object_Attack_Request( ServerConnection& connection );
	void Process_Battle_End_Turn_Request( ServerConnection& connection );
	void Process_Player_Leaving_Battle( ServerConnection& connection );

	int Socket;

	typedef std::list< ServerConnection > ConnectionListType;
	typedef ConnectionListType::iterator ConnectionListIteratorType;
	ConnectionListType ConnectionList;
	PGconn* DatabaseConnection;
};

#endif