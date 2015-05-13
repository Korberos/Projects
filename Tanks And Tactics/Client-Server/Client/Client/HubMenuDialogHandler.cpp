#include "HubMenuDialogHandler.h"

#include <SDL.h>
#include "ClientConnection.h"
#include "GameClient.h"
#include "WorldController.h"
#include "BattleController.h"
#include "CharacterMenuDialogHandler.h"
#include "LobbyMenuDialogHandler.h"
#include "GameMenuDialogHandler.h"
#include "../../ClientServerCommon.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"

std::string HubMenuDialogHandler::CharacterName = "";

void HubMenuDialogHandler::Activate( void )
{
	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientHubMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	BarracksDialogFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("BarracksDialogFrame"));
	ENFORCED_IF ( BarracksDialogFrame != NULL )
	{
		BarracksDialogFrame->Set_Visible( false );
	}

	SummonerDialogFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("SummonerDialogFrame"));
	ENFORCED_IF ( SummonerDialogFrame != NULL )
	{
		SummonerDialogFrame->Set_Visible( false );
	}

	JoinRandomLobbyTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("JoinRandomLobbyButton"));
	ENFORCED_IF ( JoinRandomLobbyTextButton != NULL )
	{
		Register_Event_Handler( JoinRandomLobbyTextButton, IT_UNCLICK, &HubMenuDialogHandler::Request_Random_Lobby_Join );
	}

	JoinRandomBattleTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("JoinRandomBattleButton"));
	ENFORCED_IF ( JoinRandomBattleTextButton != NULL )
	{
		Register_Event_Handler( JoinRandomBattleTextButton, IT_UNCLICK, &HubMenuDialogHandler::Request_Random_Battle_Join );
	}

	CharacterMenuTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CharacterMenuButton"));
	ENFORCED_IF ( CharacterMenuTextButton != NULL )
	{
		Register_Event_Handler( CharacterMenuTextButton, IT_UNCLICK, &HubMenuDialogHandler::Load_Character_Menu );
	}

	CustomBattleTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CreateCustomBattleButton"));
	ENFORCED_IF ( CustomBattleTextButton != NULL )
	{
		Register_Event_Handler( CustomBattleTextButton, IT_UNCLICK, &HubMenuDialogHandler::Load_Custom_Lobby_Menu );
	}

	CancelDialogTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CancelDialogTextButton"));
	ENFORCED_IF ( CancelDialogTextButton != NULL )
	{
		Register_Event_Handler( CancelDialogTextButton, IT_UNCLICK, &HubMenuDialogHandler::Cancel_Dialog );
	}

	Camera.SetOrigin(1.0f, 1.0f, -5.0f);
	Camera.SetForward(0.0f, 0.0f, 1.0f);
	Camera.Normalize();

	WorldController::Get_Instance()->Load_World_Geometry( "Assets/Maps/HubMap.xml" );

	DebugConsole::Get_Instance()->Add_String( "Hub Menu Initialized" );
}


void HubMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
}


void HubMenuDialogHandler::Input( void )
{
	BASECLASS::Input();

	InputSystem* input_system = InputSystem::Get_Instance();

	//  Move the camera based on the input and the amount of time that has passed
	if (input_system->Get_Key(SDLK_a))		{ CameraActionFlags |= CAMERA_MOVE_FORWARD; }
	if (input_system->Get_Key(SDLK_d))		{ CameraActionFlags |= CAMERA_MOVE_BACKWARDS; }
	if (input_system->Get_Key(SDLK_w))		{ CameraActionFlags |= CAMERA_MOVE_LEFT; }
	if (input_system->Get_Key(SDLK_s))		{ CameraActionFlags |= CAMERA_MOVE_RIGHT; }
	if (input_system->Get_Key(SDLK_q))		{ CameraActionFlags |= CAMERA_SPIN_LEFT; }
	if (input_system->Get_Key(SDLK_e))		{ CameraActionFlags |= CAMERA_SPIN_RIGHT; }
	if (input_system->Get_Key(SDLK_UP))		{ CameraActionFlags |= CAMERA_LOOK_UP; }
	if (input_system->Get_Key(SDLK_DOWN))	{ CameraActionFlags |= CAMERA_LOOK_DOWN; }
	if (input_system->Get_Key(SDLK_LEFT))	{ CameraActionFlags |= CAMERA_LOOK_LEFT; }
	if (input_system->Get_Key(SDLK_RIGHT))	{ CameraActionFlags |= CAMERA_LOOK_RIGHT; }

	static int test_status = -1;
	if ( input_system->Get_Key( SDLK_SPACE ) == 1 )
	{
		++test_status;
		if ( test_status >= 2 ) { test_status = 0; }
		switch ( test_status )
		{
		case 0:
			Load_Barracks_Menu();
			break;

		case 1:
			Load_Summoner_Menu();
			break;
		}
	}
}


void HubMenuDialogHandler::Update( float time_slice )
{
	BASECLASS::Update( time_slice );

	if ( CameraActionFlags != 0 )
	{
		if ( CameraActionFlags & CAMERA_MOVE_FORWARD )		{ Camera.MoveRight(1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_MOVE_BACKWARDS )	{ Camera.MoveRight(-1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_MOVE_LEFT )			{ Camera.MoveForward(1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_MOVE_RIGHT )			{ Camera.MoveForward(-1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_SPIN_LEFT )			{ Camera.MoveUp(1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_SPIN_RIGHT )			{ Camera.MoveUp(-1.0f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_LOOK_UP )				{ Camera.RotateLocalX(-0.4f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_LOOK_DOWN )			{ Camera.RotateLocalX(0.4f * (time_slice)); }
		if ( CameraActionFlags & CAMERA_LOOK_LEFT )			{ Camera.RotateWorld(0.4f * (time_slice), 0.0f, 1.0f, 0.0f); }
		if ( CameraActionFlags & CAMERA_LOOK_RIGHT )			{ Camera.RotateWorld(-0.4f * (time_slice), 0.0f, 1.0f, 0.0f); }
		CameraActionFlags = 0;
	}
}


void HubMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();
}


void HubMenuDialogHandler::Render3D( void )
{
	BASECLASS::Render3D();

	Camera.ApplyCameraTransform();
	WorldController::Get_Instance()->Render_Map();
}


bool HubMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_JOIN_RANDOM_LOBBY_RESPONSE:
		Process_Join_Random_Lobby_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_JOIN_RANDOM_BATTLE_RESPONSE:
		Process_Join_Random_Battle_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_JOINED:
		LobbyMenuDialogHandler::Process_Lobby_Player_Joined_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_LIST:
		LobbyMenuDialogHandler::Process_Lobby_Player_List_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_NEW_LEADER:
		LobbyMenuDialogHandler::Process_Lobby_New_Leader_Message( message_buffer_id );
		break;

	default:
		return false;
		break;
	}

	return true;
}


void HubMenuDialogHandler::Load_Character_Menu( void )
{
	CharacterMenuDialogHandler::Set_As_Active_Dialog();
}


void HubMenuDialogHandler::Load_Custom_Lobby_Menu( void )
{
	LobbyMenuDialogHandler::Set_Custom( true );
	LobbyMenuDialogHandler::Set_As_Active_Dialog();
}


void HubMenuDialogHandler::Load_Game_Menu( void )
{
	GameMenuDialogHandler::Set_As_Active_Dialog();
}


void HubMenuDialogHandler::Load_Barracks_Menu( void )
{
	BarracksDialogFrame->Set_Visible( true );
	SummonerDialogFrame->Set_Visible( false );
	CancelDialogTextButton->Set_Visible( true );

	JoinRandomLobbyTextButton->Set_Visible( false );
	JoinRandomBattleTextButton->Set_Visible( false );
	CharacterMenuTextButton->Set_Visible( false );
	CustomBattleTextButton->Set_Visible( false );
}

void HubMenuDialogHandler::Load_Summoner_Menu( void )
{
	BarracksDialogFrame->Set_Visible( false );
	SummonerDialogFrame->Set_Visible( true );
	CancelDialogTextButton->Set_Visible( true );

	JoinRandomLobbyTextButton->Set_Visible( false );
	JoinRandomBattleTextButton->Set_Visible( false );
	CharacterMenuTextButton->Set_Visible( false );
	CustomBattleTextButton->Set_Visible( false );
}

void HubMenuDialogHandler::Cancel_Dialog( void )
{
	BarracksDialogFrame->Set_Visible( false );
	SummonerDialogFrame->Set_Visible( false );
	CancelDialogTextButton->Set_Visible( false );

	JoinRandomLobbyTextButton->Set_Visible( true );
	JoinRandomBattleTextButton->Set_Visible( true );
	CharacterMenuTextButton->Set_Visible( true );
	CustomBattleTextButton->Set_Visible( true );
}


void HubMenuDialogHandler::Request_Random_Lobby_Join( void )
{
	ClientConnection::Request_Random_Lobby_Join( GameClient::Get_Instance()->Get_Server_Socket() );
}


void HubMenuDialogHandler::Request_Random_Battle_Join( void )
{
	ClientConnection::Request_Random_Battle_Join( GameClient::Get_Instance()->Get_Server_Socket() );
}


void HubMenuDialogHandler::Process_Join_Random_Lobby_Response( int message_buffer_id )
{
	char response = readchar( message_buffer_id );
	std::string lobby_name;
	std::string map_name;
	int map_index;
	int battle_type;
	int players_type;

	switch ( response )
	{
	case RANDOM_LOBBY_JOIN_SUCCESS:
		lobby_name = std::string( readstring( message_buffer_id ) );
		map_name = std::string( readstring( message_buffer_id ) );
		map_index = readint( message_buffer_id );
		battle_type = readint( message_buffer_id );
		players_type = readint( message_buffer_id );

		LobbyMenuDialogHandler::Set_Lobby_Information( lobby_name, map_name, map_index, BattleType(battle_type), PlayersType(players_type) );
		LobbyMenuDialogHandler::Set_Custom( false );
		LobbyMenuDialogHandler::Set_As_Active_Dialog();
		break;

	case RANDOM_LOBBY_JOIN_FAILURE_NO_LOBBIES:
		DebugConsole::Get_Instance()->Add_String( "Failed to join random lobby. No lobbies currently active." );
		break;

	case RANDOM_LOBBY_JOIN_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Failed to join random lobby for unknown reason. Please contact an administrator. ");
		break;
	}
}


void HubMenuDialogHandler::Process_Join_Random_Battle_Response( int message_buffer_id )
{
	char response = readchar( message_buffer_id );
	switch ( response )
	{
	case RANDOM_BATTLE_JOIN_SUCCESS:
		if ( readchar( message_buffer_id ) == SUCCESS )
		{
			Process_Map_Data_Response( message_buffer_id );
			Load_Game_Menu();
		}
		else
		{
			DebugConsole::Get_Instance()->Add_String( "Sucessfully found a random battle, but map loading failed. Please contact an administrator." );
		}
		break;

	case RANDOM_BATTLE_JOIN_FAILURE_NO_BATTLES:
		DebugConsole::Get_Instance()->Add_String( "Failed to join random battle. No battles currently active." );
		break;

	case RANDOM_BATTLE_JOIN_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Failed to join random battle for unknown reason. Please contact an administrator. ");
		break;
	}
}


void HubMenuDialogHandler::Process_Map_Data_Response( int message_buffer_id )
{
	MapDataStruct data_struct;
	data_struct.Width = readuint( message_buffer_id );
	data_struct.Height = readuint( message_buffer_id );
	data_struct.HeightList = new float[data_struct.Width * data_struct.Height];
	for ( unsigned int i = 0; i < data_struct.Width * data_struct.Height; ++i )
	{
		data_struct.HeightList[i] = readfloat( message_buffer_id );
	}

	bool has_palette = ( readchar( message_buffer_id ) != 0 );
	if ( has_palette )
	{
		data_struct.Palette = new ColorPalette;
		data_struct.Palette->Width = readuint( message_buffer_id );
		data_struct.Palette->Height = readuint( message_buffer_id );
		data_struct.Palette->Colors = new Vector3<float>[data_struct.Palette->Width * data_struct.Palette->Height];
		for ( unsigned int i = 0; i < data_struct.Palette->Width * data_struct.Palette->Height; ++i )
		{
			data_struct.Palette->Colors[i].x = readfloat( message_buffer_id );
			data_struct.Palette->Colors[i].y = readfloat( message_buffer_id );
			data_struct.Palette->Colors[i].z = readfloat( message_buffer_id );
		}
	}

	WorldController::Get_Instance()->Load_World_Geometry( data_struct );
	BattleController::Get_Instance()->Load_World_Data( data_struct );
}