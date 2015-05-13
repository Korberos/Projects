#ifndef _GAME_CLIENT_
#define _GAME_CLIENT_

#include "InputSystem.h"
#include "TaskScheduler.h"
#include "./../../ClientServerCommon.h"
#include "WinsockWrapper/WinsockWrapper.h"
#include "DebugTools.h"

//  Typedefs
typedef void (*VoidFunction)( void );
typedef void (*FloatFunction)( float );
typedef std::vector< VoidFunction > VoidFunctionList;
typedef std::vector< FloatFunction > FloatFunctionList;
typedef VoidFunctionList::iterator VoidFunctionListIter;
typedef FloatFunctionList::iterator FloatFunctionListIter;

//  Function Declarations
void Load_Menu_Main( void );
void Load_Menu_Character( void );
void Load_Menu_Map_Select( int socket );
void Load_Menu_Map_Select_Deferred( int& socket );
void RequestMapInformation( int socket );
void Load_Menu_Game( void );
void Load_Menu_Game_Deferred( void );

void World_Controller_Input( float time_slice );
void World_Controller_Update( float time_slice );
void World_Controller_Render( void );

class LoadMapSelectMenuScheduledTask : public ScheduledTask
{
public:
	LoadMapSelectMenuScheduledTask( int& socket, float time_allotted = 0.0f ) : 
		Socket( socket )
	{
		TimeAllotted = time_allotted;
	}

	~LoadMapSelectMenuScheduledTask() {}
	
	virtual bool Event( void )
	{
		Load_Menu_Map_Select( Socket );
		return true;
	}

private:
	int& Socket;
};

class LoadGameMenuScheduledTask : public ScheduledTask
{
public:
	LoadGameMenuScheduledTask( float time_allotted = 0.0f )
	{
		TimeAllotted = time_allotted;
	}
	~LoadGameMenuScheduledTask() {}
	
	virtual bool Event( void )
	{
		Load_Menu_Game();
		return true;
	}
};


class GameClient
{
public:
	static GameClient* Get_Instance( void );

	bool Initialize( void );
	void Input( float time_slice );
	void Update( float time_slice );
	void Render3D( void );
	void Render2D( void );

	int Get_Server_Socket( void ) const { return ServerSocket; }

private:
	GameClient()
	{
		MessageBuffer = createbuffer();
		ServerSocket = -1;
	}

	~GameClient()
	{
		ENFORCED_IF ( ServerSocket != -1 )
		{
			closesock( ServerSocket );
			ServerSocket = -1;
		}
	}

	void ReceiveMessages( void );

public:
	void Remove_Input_Function( FloatFunction );
	void Remove_Update_Function( FloatFunction );
	void Remove_Render_3D_Function( VoidFunction );
	void Remove_Render_2D_Function( VoidFunction );

private:
	// Input, Update, and Render additionals
	FloatFunctionList InputFunctions;
	FloatFunctionList UpdateFunctions;
	VoidFunctionList Render3DFunctions;
	VoidFunctionList Render2DFunctions;

	int MessageBuffer;
	int ServerSocket;
};

#endif