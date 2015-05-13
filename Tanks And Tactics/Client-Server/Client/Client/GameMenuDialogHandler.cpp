#include "GameMenuDialogHandler.h"

#include <SDL.h>
#include "ClientConnection.h"
#include "GameClient.h"
#include "BattleController.h"
#include "FontController.h"
#include "WorldController.h"
#include "HubMenuDialogHandler.h"
#include "../../ClientServerCommon.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"
#include "DialogSystem/Elements/MenuElement_EditBox.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"

const int MAX_BATTLE_CHAT_STRINGS = 80;
const std::string BATTLE_CHAT_FONT("Arial-12");

void GameMenuDialogHandler::Activate( void )
{
	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientGameMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	HubMenuButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("HubMenuButton"));
	ENFORCED_IF ( HubMenuButton != NULL )
	{
		HubMenuButton->Set_Visible( false );
		Register_Event_Handler( HubMenuButton, IT_UNCLICK, &GameMenuDialogHandler::Load_Hub_Menu );
	}

	CurrentTurnTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("CurrentTurnTextBlock"));
	ENFORCED_IF ( CurrentTurnTextBlock != NULL )
	{
		CurrentTurnTextBlock->Set_Visible( false );
	}

	EndTurnTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("EndTurnTextButton"));
	ENFORCED_IF ( EndTurnTextButton != NULL )
	{
		EndTurnTextButton->Set_Visible( false );
		Register_Event_Handler( EndTurnTextButton, IT_UNCLICK, &GameMenuDialogHandler::End_Current_Turn );
	}

	UnitOptionsFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("UnitOptionsFrame"));
	ENFORCED_IF ( UnitOptionsFrame != NULL )
	{
		UnitOptionsFrame->Set_Visible( false );

		UnitNameTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>( Menu->Get_Element( "UnitNameTextBlock" ) );
		ASSERT ( UnitNameTextBlock != NULL );
			
		UnitTypeTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>( Menu->Get_Element("UnitTypeTextBlock" ) );
		ASSERT ( UnitTypeTextBlock != NULL );
			
		UnitLevelTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>( Menu->Get_Element( "UnitLevelTextBlock" ) );
		ASSERT ( UnitLevelTextBlock != NULL );
			
		UnitSpeedTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>( Menu->Get_Element( "UnitSpeedTextBlock" ) );
		ASSERT ( UnitSpeedTextBlock != NULL );

		MoveUnitTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("MoveButton"));
		ENFORCED_IF ( MoveUnitTextButton != NULL )
		{
			Register_Event_Handler( MoveUnitTextButton, IT_CLICK, &GameMenuDialogHandler::Mid_Click );
			Register_Event_Handler( MoveUnitTextButton, IT_UNCLICK, &GameMenuDialogHandler::Set_Move_Mode );
		}

		AttackUnitTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("AttackButton"));
		ENFORCED_IF ( AttackUnitTextButton != NULL )
		{
			Register_Event_Handler( AttackUnitTextButton, IT_CLICK, &GameMenuDialogHandler::Mid_Click );
			Register_Event_Handler( AttackUnitTextButton, IT_UNCLICK, &GameMenuDialogHandler::Set_Attack_Mode );
		}
	}

	ChatBoxFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("ChatBoxFrame"));
	ENFORCED_IF ( ChatBoxFrame != NULL )
	{
		Register_Event_Handler( ChatBoxFrame, IT_MOVE, &GameMenuDialogHandler::Realign_Chat );

		ChatBoxFrame->Set_Visible( ChatVisible );
		Realign_Chat();

		ChatInputEditBox = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element( "ChatInputEditBox" ));
		ASSERT ( ChatInputEditBox != NULL );

		EnterTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element( "EnterTextButton" ));
		ENFORCED_IF ( EnterTextButton != NULL )
		{
			Register_Event_Handler( EnterTextButton, IT_UNCLICK, &GameMenuDialogHandler::Request_Add_Chat_Line );
		}
	}

	ToggleChatVisibleButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element( "ToggleChatVisibleButton" ));
	ENFORCED_IF ( ToggleChatVisibleButton != NULL )
	{
		ToggleChatVisibleButton->Set_Text( ChatVisible ? "Hide Chat" : "Show Chat" );
		Register_Event_Handler( ToggleChatVisibleButton, IT_UNCLICK, &GameMenuDialogHandler::Toggle_Chat_Visible );
	}

	ScoreboardFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element( "ScoreboardFrame" ));
	ENFORCED_IF ( ScoreboardFrame != NULL )
	{
		ScoreboardFrame->Set_Visible( false );
		WinnerOrLoserTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("WinnerOrLoserTextBlock"));
		ASSERT ( WinnerOrLoserTextBlock != NULL );
	}

	Camera.SetOrigin(1.0f, 1.0f, -5.0f);
	Camera.SetForward(0.0f, 0.0f, 1.0f);
	Camera.Normalize();

	ChatFont = FontController::Get_Instance()->Get_Font( BATTLE_CHAT_FONT.c_str() );
	ENFORCED_IF( ChatFont != NULL )
	{
		StringDisplayCount = ChatDimensions.second / (4 + ChatFont->Get_Font_Height() ) - 1;
	}

	BattleController::Get_Instance()->New_Battle();
	BattleController::Get_Instance()->Set_Camera( &Camera );

	DebugConsole::Get_Instance()->Add_String( "Game Menu Initialized" );
}


void GameMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
}


void GameMenuDialogHandler::Input( void )
{
	MidClick = false;
	BASECLASS::Input();
	if ( MidClick ) { return; }

	//  If the chat box is visible, do not take any other input after the menu
	if ( ChatVisible || !BattleInputEnabled ) { return; }

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

	BattleController* battle_controller = BattleController::Get_Instance();
	if ( battle_controller->Input() )
	{
		const BattleUnit* selected_unit = static_cast<const BattleUnit*>(battle_controller->Get_Selected_Unit());
		if ( selected_unit != NULL )
		{
			const std::string& name = selected_unit->Get_Unit_Name();
			const int type = selected_unit->Get_Unit_Type();
			const int level = selected_unit->Get_Unit_Level();
			const int experience = selected_unit->Get_Unit_Experience();
			const int health = selected_unit->Get_Unit_Health();
			const int attack = selected_unit->Get_Unit_Attack();

			char index_string[16];
			_itoa_s( battle_controller->Get_Selected_Unit_Index(), index_string, 16, 10 );

			char type_string[16];
			_itoa_s( selected_unit->Get_Unit_Type(), type_string, 16, 10 );

			char level_string[16];
			_itoa_s( selected_unit->Get_Unit_Level(), level_string, 16, 10 );

			char speed_string[16];
			_itoa_s( selected_unit->Get_Unit_Speed(), speed_string, 16, 10 );

			//  Set the UnitOptionsFrame info boxes with the unit information
			UnitNameTextBlock->Set_Text( "Name: " + selected_unit->Get_Unit_Name() + " (" + std::string(index_string) + ")" );
			UnitTypeTextBlock->Set_Text( "Type: " + std::string(type_string) );
			UnitLevelTextBlock->Set_Text( "Level: " + std::string(level_string) );
			UnitSpeedTextBlock->Set_Text( "Speed: " + std::string(speed_string) );

			int status_flags = selected_unit->Get_Status_Flags();
			bool move_exhausted = ( ( status_flags & MOVE_EXHAUSTION ) == MOVE_EXHAUSTION );
			bool attack_exhausted = ( ( status_flags & ATTACK_EXHAUSTION ) == ATTACK_EXHAUSTION );

			MoveUnitTextButton->Set_Text( move_exhausted ? "-Move Exhausted-" : "Move Unit" );
			MoveUnitTextButton->Set_Enabled( !move_exhausted );

			AttackUnitTextButton->Set_Text( attack_exhausted ? "-Attack Exhausted-" : "Attack Unit" );
			AttackUnitTextButton->Set_Enabled( !attack_exhausted );
		}
		UnitOptionsFrame->Set_Visible( ( selected_unit != NULL ) );
	}
}


void GameMenuDialogHandler::Update( float time_slice )
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

	BattleController::Get_Instance()->Update( time_slice );
}


void GameMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();

	BattleController::Get_Instance()->Render2D();

	//  If we have no font, do not attempt to display the strings
	if ( ChatVisible )
	{
		ENFORCED_IF ( ChatFont != NULL )
		{
			//  Render the last X strings, starting from the bottom (X being the number of strings we can display)
			for ( unsigned int i = 0; i < StringDisplayCount && i < ChatStrings.size();  ++i )
			{
				ChatFont->Render_Text( ChatStrings[ ChatStrings.size() - 1 - i ].c_str(), ChatPosition.first, ChatPosition.second + ChatDimensions.second - ( (4 + ChatFont->Get_Font_Height() ) * ( i + 2 ) ), false, false );
			}
		}
	}
}


void GameMenuDialogHandler::Render3D( void )
{
	BASECLASS::Render3D();

	Camera.ApplyCameraTransform();
	WorldController::Get_Instance()->Render_Map();
	BattleController::Get_Instance()->Render3D();
}


bool GameMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_CHAT_MESSAGE_RESPONSE:
		Process_Battle_Chat_Message_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_CREATED:
		Process_Battle_Object_Created_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_PLAYER_INDEX:
		Process_Battle_Player_Index_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_MOVED:
		Process_Battle_Object_Moved_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_OBJECT_DESTROYED:
		Process_Battle_Object_Destroyed_Message( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_NEXT_TURN:
		Process_Battle_Next_Turn( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_WINNER:
		Process_Battle_Winner( message_buffer_id );
		break;

	default:
		return false;
		break;
	}

	return true;
}


void GameMenuDialogHandler::Load_Hub_Menu( void )
{
	//  Inform the server that we are leaving
	int socket = GameClient::Get_Instance()->Get_Server_Socket();
	ClientConnection::Leave_Battle( socket );

	//  Clear all units from the local battle controller
	BattleController::Get_Instance()->Clear_Units();

	HubMenuDialogHandler::Set_As_Active_Dialog();
}


void GameMenuDialogHandler::Mid_Click( void )
{
	//  MidClick is a boolean flag set to show that we are in
	//  the middle of clicking a menu option, allowing us to
	//  prevent other input that might screw up the gamestate
	MidClick = true;
}


void GameMenuDialogHandler::Set_Move_Mode( void )
{
	BattleController::Get_Instance()->Set_Current_Mode( BattleController::MODE_MOVE );
	UnitOptionsFrame->Set_Visible( false );
}

void GameMenuDialogHandler::Set_Attack_Mode( void )
{
	BattleController::Get_Instance()->Set_Current_Mode( BattleController::MODE_ATTACK );
	UnitOptionsFrame->Set_Visible( false );
}


void GameMenuDialogHandler::End_Current_Turn( void )
{
	ClientConnection::Request_End_Turn_Message( GameClient::Get_Instance()->Get_Server_Socket() );
}


void GameMenuDialogHandler::Request_Add_Chat_Line( void )
{
	FAIL_IF ( ChatInputEditBox == NULL ) { return; }

	const std::string& input_string = ChatInputEditBox->Get_Text();
	if ( input_string.empty() ) { return; }

	const std::string chat_string = HubMenuDialogHandler::Get_Character_Name() + ": " + input_string;

	ClientConnection::Request_Battle_Chat_Message( GameClient::Get_Instance()->Get_Server_Socket(), chat_string.c_str() );
	ChatInputEditBox->Set_Text( "" );
}


void GameMenuDialogHandler::Process_Battle_Chat_Message_Response( int message_buffer_id )
{
	std::string new_text( readstring( message_buffer_id ) );

	Add_Chat_String( new_text.c_str() );
}


void GameMenuDialogHandler::Add_Chat_String( const char* text )
{
	FAIL_IF ( ChatFont == NULL ) { return; }

	if ( ChatFont->Get_Text_Width( text ) > (unsigned int)(ChatDimensions.first - 8) )
	{
		unsigned int cut_spot = ChatFont->Character_Count_Before_Passing_Width( text, ChatDimensions.first, true );
		std::string shortened_string( text );
		while ( shortened_string.length() > cut_spot ) { shortened_string.erase( --shortened_string.end() ); }
		Add_Chat_String( shortened_string.c_str() );
		Add_Chat_String( text + cut_spot );
		return;
	}

	ChatStrings.push_back( std::string( text ) );
	while ( int(ChatStrings.size()) > MAX_BATTLE_CHAT_STRINGS )
	{
		ChatStrings.erase( ChatStrings.begin() );
	}
}


void GameMenuDialogHandler::Process_Battle_Player_Index_Message( int message_buffer_id )
{
	int local_player_id = readint( message_buffer_id );
	FAIL_IF ( local_player_id < 0 || local_player_id > MAX_INSTANCE_PLAYERS ) { return; }

	BattleController* battle_controller = BattleController::Get_Instance();
	battle_controller->Set_Local_Player_ID( local_player_id );

	bool local_turn = ( battle_controller->Get_Current_Turn() == battle_controller->Get_Local_Player_ID() );
	EndTurnTextButton->Set_Visible( local_turn );
	CurrentTurnTextBlock->Set_Visible( !local_turn );
}


void GameMenuDialogHandler::Process_Battle_Object_Created_Message( int message_buffer_id )
{
	BattleController* battle_controller = BattleController::Get_Instance();

	int object_count = readint( message_buffer_id );

	int player_owner = -1;
	std::string name;
	int type;
	int level;
	int experience;
	int max_health;
	int attack;
	int speed;
	int position_x = -1;
	int position_y = -1;

	for ( int i = 0; i < object_count; ++i )
	{
		player_owner = readint( message_buffer_id );
		name = readstring( message_buffer_id );
		type = readint( message_buffer_id );
		level = readint( message_buffer_id );
		experience = readint( message_buffer_id );
		max_health = readint( message_buffer_id );
		attack = readint( message_buffer_id );
		speed = readint( message_buffer_id );
		
		position_x = readint( message_buffer_id );
		position_y = readint( message_buffer_id );

		battle_controller->Add_Unit( player_owner, name, type, level, experience, max_health, attack, speed, position_x, position_y );
	}
}


void GameMenuDialogHandler::Process_Battle_Object_Moved_Message( int message_buffer_id )
{
	int unit_index = readint( message_buffer_id );
	int position_x = readint( message_buffer_id );
	int position_y = readint( message_buffer_id );

	BattleController::Get_Instance()->Move_Unit( unit_index, position_x, position_y );
}


void GameMenuDialogHandler::Process_Battle_Object_Destroyed_Message( int message_buffer_id )
{
	int unit_index = readint( message_buffer_id );

	BattleController::Get_Instance()->Remove_Unit( unit_index );
}


void GameMenuDialogHandler::Process_Battle_Next_Turn( int message_buffer_id )
{
	int new_turn = readint( message_buffer_id );

	BattleController* battle_controller = BattleController::Get_Instance();
	battle_controller->Set_Current_Turn( new_turn );

	bool local_turn = ( new_turn == battle_controller->Get_Local_Player_ID() );
	EndTurnTextButton->Set_Visible( local_turn );
	UnitOptionsFrame->Set_Visible( local_turn && battle_controller->Get_Selected_Unit() != NULL );

	CurrentTurnTextBlock->Set_Visible( !local_turn );
	if ( local_turn )
	{

		std::string current_turn_text( ""/*battle_controller->Get_Player_Name( new_turn )*/ );
		current_turn_text += "'s turn";
		CurrentTurnTextBlock->Set_Text( current_turn_text );
	}
}


void GameMenuDialogHandler::Process_Battle_Winner( int message_buffer_id )
{
	int winner = readint( message_buffer_id );

	//  Have text to say whether the local player is the winner or loser
	bool local_winner = ( BattleController::Get_Instance()->Get_Local_Player_ID() == winner );
	std::string winner_or_loser( local_winner ? "You Win!" : "You Lose!" );
	WinnerOrLoserTextBlock->Set_Text( winner_or_loser.c_str() );

	//  Show the scoreboard and Hub menu button
	ScoreboardFrame->Set_Visible( true );
	HubMenuButton->Set_Visible( true );

	//  Hide any GUI elements that are not able to be used now that the battle is over	
	if ( ChatVisible ) { Toggle_Chat_Visible(); }
	ToggleChatVisibleButton->Set_Visible( false );
	UnitOptionsFrame->Set_Visible( false );
	EndTurnTextButton->Set_Visible( false );

	//  Disable any battle input and remove menu elements related to the battle being active
	BattleInputEnabled = false;
}


void GameMenuDialogHandler::Toggle_Chat_Visible( void )
{
	ChatVisible = !ChatVisible;
	ChatBoxFrame->Set_Visible( ChatVisible );
	ToggleChatVisibleButton->Set_Text( ChatVisible ? "Hide Chat" : "Show Chat" );
}


void GameMenuDialogHandler::Realign_Chat( void )
{
	ChatPosition.first = ChatBoxFrame->Get_X() + 10;
	ChatPosition.second = ChatBoxFrame->Get_Y() + 20;
	ChatDimensions.first = ChatBoxFrame->Get_Width() - 30;
	ChatDimensions.second = ChatBoxFrame->Get_Height() - 50;
}