#include "LobbyMenuDialogHandler.h"

#include "ClientConnection.h"
#include "HubMenuDialogHandler.h"
#include "GameMenuDialogHandler.h"
#include "GameClient.h"
#include "FontController.h"
#include "WorldController.h"
#include "BattleController.h"
#include "DialogSystem/Elements/MenuElement_EditBox.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_ListBox.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"
#include "DialogSystem/Elements/MenuElement_DropDown.h"
#include "DialogSystem/Elements/MenuElement_Texture.h"

const int CHAT_BOX_X = 26;
const int CHAT_BOX_Y = 48;
const int CHAT_BOX_WIDTH = 600;
const int CHAT_BOX_HEIGHT = 540;
const int MAX_LOBBY_CHAT_STRINGS = 80;
const std::string LOBBY_CHAT_FONT("Arial-12");

std::string LobbyMenuDialogHandler::LobbyName = "";
std::string LobbyMenuDialogHandler::MapName = "";
int LobbyMenuDialogHandler::MapIndex = -1;
BattleType LobbyMenuDialogHandler::BattleTypeIndex = BattleType(-1);
PlayersType LobbyMenuDialogHandler::PlayersTypeIndex = PlayersType(-1);
bool LobbyMenuDialogHandler::Custom = false;
LobbyMenuDialogHandler::PlayerType LobbyMenuDialogHandler::PlayerList[MAX_INSTANCE_PLAYERS];

void LobbyMenuDialogHandler::Activate( void )
{
	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientLobbyMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	//  Initialize the Custom Lobby options. Hide them if the custom option is disabled.
	CustomLobbyFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element( "CustomOptionsFrame" ));
	ENFORCED_IF ( CustomLobbyFrame != NULL )
	{
		BattleNameEditBox = dynamic_cast<MenuGUIElement_EditBox*>(CustomLobbyFrame->Get_Contained_Element( "BattleNameEditBox" ));
		BattleTypeDropDown = dynamic_cast<MenuGUIElement_DropDown*>(CustomLobbyFrame->Get_Contained_Element( "BattleTypeDropDown" ));
		MapDropDown = dynamic_cast<MenuGUIElement_DropDown*>(CustomLobbyFrame->Get_Contained_Element( "MapDropDown" ));
		PlayersTypeDropDown = dynamic_cast<MenuGUIElement_DropDown*>(CustomLobbyFrame->Get_Contained_Element( "PlayersTypeDropDown" ));
		ENFORCED_IF ( BattleNameEditBox && BattleTypeDropDown && MapDropDown && PlayersTypeDropDown )
		{
			Request_Map_List();
			Register_Event_Handler( BattleNameEditBox, IT_CONFIRM, &LobbyMenuDialogHandler::Request_Custom_Lobby );
		}

		MenuGUIElement_TextButton* start_lobby_button = dynamic_cast<MenuGUIElement_TextButton*>(CustomLobbyFrame->Get_Contained_Element( "StartLobbyButton" ));
		ENFORCED_IF ( start_lobby_button != NULL )
		{
			Register_Event_Handler( start_lobby_button, IT_UNCLICK, &LobbyMenuDialogHandler::Request_Custom_Lobby );
		}

		MenuGUIElement_TextButton* cancel_lobby_button = dynamic_cast<MenuGUIElement_TextButton*>(CustomLobbyFrame->Get_Contained_Element( "CancelLobbyButton" ));
		ENFORCED_IF ( cancel_lobby_button != NULL )
		{
			Register_Event_Handler( cancel_lobby_button, IT_UNCLICK, &LobbyMenuDialogHandler::Cancel_Custom_Lobby );
		}
	}

	//  Initialize the Lobby Info frame. Hide them if customization is up.
	LobbyInfoFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element( "LobbyInfoFrame" ));
	ENFORCED_IF ( LobbyInfoFrame != NULL )
	{
		LobbyNameTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element( "LobbyNameTextBlock" ));
		MapNameTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element( "MapNameTextBlock" ));
		BattleTypeTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element( "BattleTypeTextBlock" ));
		PlayersTypeTextBlock = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element( "PlayersTypeTextBlock" ));
		CustomizeLobbyButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element( "CustomizeLobbyButton" ));
		ENFORCED_IF ( LobbyNameTextBlock != NULL && MapNameTextBlock != NULL && BattleTypeTextBlock != NULL && PlayersTypeTextBlock != NULL && CustomizeLobbyButton != NULL )
		{
			Register_Event_Handler( CustomizeLobbyButton, IT_UNCLICK, &LobbyMenuDialogHandler::Recustomize_Lobby );
			Fill_Lobby_Information();
		}
	}

	ChatInputEditBox = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element( "ChatInputEditBox" ));
	ASSERT ( ChatInputEditBox != NULL );

	EnterTextButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element( "EnterTextButton" ));
	ENFORCED_IF ( EnterTextButton != NULL )
	{
		Register_Event_Handler( EnterTextButton, IT_UNCLICK, &LobbyMenuDialogHandler::Request_Add_Chat_Line );
	}

	StartGameButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element( "StartGameTextButton" ));
	ENFORCED_IF ( StartGameButton != NULL )
	{
		Register_Event_Handler( StartGameButton, IT_UNCLICK, &LobbyMenuDialogHandler::Request_Start_Game );
		StartGameButton->Set_Visible( false );
	}

	PlayerListBox = dynamic_cast<MenuGUIElement_ListBox*>(Menu->Get_Element( "PlayerListBox" ));
	LobbyLeaderTexture = dynamic_cast<MenuGUIElement_Texture*>(Menu->Get_Element( "LobbyLeaderSymbol" ));
	ENFORCED_IF ( PlayerListBox != NULL && LobbyLeaderTexture != NULL )
	{
		Fill_Player_List();
	}

	Customization_Menu();

	ChatFont = FontController::Get_Instance()->Get_Font( LOBBY_CHAT_FONT.c_str() );
	ENFORCED_IF( ChatFont != NULL )
	{
		StringDisplayCount = CHAT_BOX_HEIGHT / (4 + ChatFont->Get_Font_Height() ) - 1;
	}

	DebugConsole::Get_Instance()->Add_String( "Lobby Menu Initialized" );
}


void LobbyMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
}


void LobbyMenuDialogHandler::Input( void )
{
	BASECLASS::Input();
}


void LobbyMenuDialogHandler::Update( float time_slice )
{
	BASECLASS::Update( time_slice );
}


void LobbyMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();

	//  If we have no font, do not attempt to display the strings
	ENFORCED_IF ( ChatFont != NULL )
	{
		//  Render the last X strings, starting from the bottom (X being the number of strings we can display)
		for ( unsigned int i = 0; i < StringDisplayCount && i < ChatStrings.size();  ++i )
		{
			ChatFont->Render_Text( ChatStrings[ ChatStrings.size() - 1 - i ].c_str(), CHAT_BOX_X, CHAT_BOX_Y + CHAT_BOX_HEIGHT - ( (4 + ChatFont->Get_Font_Height() ) * ( i + 2 ) ), false, false );
		}
	}
}


void LobbyMenuDialogHandler::Render3D( void )
{
	BASECLASS::Render3D();
}


bool LobbyMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_MAP_LIST_RESPONSE:
		Process_Map_List_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_CREATE_LOBBY_RESPONSE:
		Process_Custom_Lobby_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_CHAT_MESSAGE_RESPONSE:
		Process_Lobby_Chat_Message_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_INFORMATION:
		Process_Lobby_Information( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_BATTLE_SUMMONS:
		Process_Battle_Summons( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_JOINED:
		Process_Lobby_Player_Joined_Message( message_buffer_id );
		Fill_Player_List();
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_PLAYER_LIST:
		Process_Lobby_Player_List_Message( message_buffer_id );
		Fill_Player_List();
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_NEW_LEADER:
		Process_Lobby_New_Leader_Message( message_buffer_id );
		Fill_Player_List();
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOBBY_START_BATTLE_RESPONSE:
		Process_Lobby_Start_Battle_Response( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_MAP_DATA_RESPONSE:
		{
			bool map_found = readchar( message_buffer_id ) != 0;
			FAIL_IF ( !map_found ) { break; }
			Process_Map_Data_Response( message_buffer_id );
			Load_Game_Menu();
		}
		break;

	default:
		return false;
		break;
	}

	return true;
}


void LobbyMenuDialogHandler::Set_Lobby_Information( const std::string& lobby_name, const std::string& map_name, int map_index, BattleType battle_type, PlayersType players_index )
{
	LobbyName = lobby_name;
	MapName = map_name;
	MapIndex = map_index;
	BattleTypeIndex = battle_type;
	PlayersTypeIndex = players_index;
}


void LobbyMenuDialogHandler::Process_Lobby_Player_Joined_Message( int message_buffer_id )
{
	int player_index = readint( message_buffer_id );
	PlayerType new_player;
	new_player.first = readchar( message_buffer_id );
	new_player.second = std::string( readstring( message_buffer_id ) );
	if ( new_player.second.length() < 1 ) { return; }

	Add_Lobby_Player( player_index, new_player );
}


void LobbyMenuDialogHandler::Process_Lobby_Player_List_Message( int message_buffer_id )
{
	int player_count = readint( message_buffer_id );

	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		PlayerList[i].first = 0;
		PlayerList[i].second.clear();
	}

	for ( int i = 0; i < player_count; ++i )
	{
		int player_index = readint( message_buffer_id );
		PlayerList[player_index].first = LOBBY_PLAYER_EXISTS;
		PlayerList[player_index].second = std::string( readstring( message_buffer_id ) );
	}

	int lobby_leader = readint( message_buffer_id );
	PlayerList[lobby_leader].first |= LOBBY_PLAYER_LEADER;

	int local_player = readint( message_buffer_id );
	PlayerList[local_player].first |= LOBBY_PLAYER_LOCAL;
}


void LobbyMenuDialogHandler::Process_Lobby_New_Leader_Message( int message_buffer_id )
{
	int lobby_leader = readint( message_buffer_id );
	FAIL_IF ( lobby_leader < 0 || lobby_leader >= MAX_INSTANCE_PLAYERS ) { return; }

	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		PlayerList[i].first &= ~LOBBY_PLAYER_LEADER;
	}
	PlayerList[lobby_leader].first |= LOBBY_PLAYER_LEADER;
}


void LobbyMenuDialogHandler::Load_Hub_Menu( void )
{
	Clear_Lobby();

	HubMenuDialogHandler::Set_As_Active_Dialog();
}


void LobbyMenuDialogHandler::Load_Game_Menu( void )
{
	GameMenuDialogHandler::Set_As_Active_Dialog();
}

void LobbyMenuDialogHandler::Clear_Lobby( void )
{
	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		PlayerList[i].first = 0;
		PlayerList[i].second.clear();
	}
}


void LobbyMenuDialogHandler::Customization_Menu( void )
{
	ENFORCED_IF ( CustomLobbyFrame != NULL )
	{
		CustomLobbyFrame->Set_Visible( Custom );
		BattleNameEditBox->Set_Clicked( Custom );
	}

	ENFORCED_IF ( LobbyInfoFrame != NULL )
	{
		LobbyInfoFrame->Set_Visible( !Custom );
	}

	ENFORCED_IF ( PlayerListBox != NULL )
	{
		PlayerListBox->Set_Visible( !Custom );
	}

	ENFORCED_IF ( ChatInputEditBox != NULL )
	{
		ChatInputEditBox->Set_Visible( !Custom );
	}

	ENFORCED_IF ( EnterTextButton != NULL )
	{
		EnterTextButton->Set_Visible( !Custom );
	}

	ENFORCED_IF ( LobbyLeaderTexture != NULL )
	{
		LobbyLeaderTexture->Set_Visible( !Custom );
	}
}


void LobbyMenuDialogHandler::Fill_Lobby_Information( void )
{
	//  Set the lobby name
	LobbyNameTextBlock->Set_Text( "Name: " + LobbyName );

	//  Set the map name
	MapNameTextBlock->Set_Text( "Map: " + MapName );

	//  Go through the battle type index list for the battle type name
	std::string battle_type("");
	switch ( BattleTypeIndex )
	{
	case BATTLE_TYPE_DEATHMATCH:
		battle_type = "Deathmatch";
		break;

	default:
		battle_type = "ERROR";
	}
	BattleTypeTextBlock->Set_Text( "Battle Type: " + battle_type );

	//  Go through the players type index list for the players type name
	std::string players_type("");
	switch ( PlayersTypeIndex )
	{
	case PLAYER_TYPE_1_V_1:
		players_type = "1 vs 1";
		break;
		
	case PLAYER_TYPE_2_V_2:
		players_type = "2 vs 2";
		break;
		
	case PLAYER_TYPE_3_PLAYER_MELEE:
		players_type = "3 Player Melee";
		break;
		
	case PLAYER_TYPE_4_PLAYER_MELEE:
		players_type = "4 Player Melee";
		break;

	default:
		players_type = "ERROR";
	}
	PlayersTypeTextBlock->Set_Text( "Players Type: " + players_type );
}


void LobbyMenuDialogHandler::Fill_Player_List( void )
{
	FAIL_IF ( PlayerListBox == NULL ) { return; }
	PlayerListBox->Clear_List();

	bool local_leader = false;
	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( ( PlayerList[i].first & LOBBY_PLAYER_EXISTS ) == 0 ) { continue; }
		PlayerListBox->Add_Item( PlayerList[i].second );

		if ( PlayerList[i].first & LOBBY_PLAYER_LEADER )
		{
			//  If the local player is the leader, take note so we can show the start game and customize buttons
			local_leader = ( ( PlayerList[i].first & LOBBY_PLAYER_LOCAL ) != 0 );

			//  Move the lobby leader indicator symbol to the right position
			int x = PlayerListBox->Get_X() - LobbyLeaderTexture->Get_Width() - 6;
			int y = PlayerListBox->Get_First_Entry_Height() + ( PlayerListBox->Get_Entry_Height() * i );
			LobbyLeaderTexture->Set_X( x );
			LobbyLeaderTexture->Set_Y( y );
		}
	}

	ENFORCED_IF ( StartGameButton != NULL )
	{
		StartGameButton->Set_Visible( local_leader );
	}

	ENFORCED_IF ( CustomizeLobbyButton != NULL )
	{
		CustomizeLobbyButton->Set_Visible( local_leader );
	}
}


void LobbyMenuDialogHandler::Recustomize_Lobby( void )
{
	LobbyMenuDialogHandler::Set_Custom( true );
	Customization_Menu();
}


void LobbyMenuDialogHandler::Request_Map_List( void )
{
	ClientConnection::Request_Map_List( GameClient::Get_Instance()->Get_Server_Socket() );
}


void LobbyMenuDialogHandler::Process_Map_List_Response( int message_buffer_id )
{
	FAIL_IF ( !Menu ) { return; }

	unsigned int map_count = readuint( message_buffer_id );
	
	//  Take in all strings. Even if we can't do anything about them, we should cleanly read the message.
	std::vector< std::string > map_list;
	for ( unsigned int i = 0; i < map_count; ++i )
	{
		map_list.push_back( readstring( message_buffer_id ) );
	}

	ENFORCED_IF ( MapDropDown != NULL )
	{
		MapDropDown->Clear();
		for ( std::vector< std::string >::iterator iter = map_list.begin(); iter != map_list.end(); ++iter )
		{
			MapDropDown->Add_Item( (*iter) );
		}
	}
}


void LobbyMenuDialogHandler::Request_Custom_Lobby( void )
{
	FAIL_IF ( BattleNameEditBox == NULL ) { return; }
	FAIL_IF ( BattleNameEditBox->Get_Text().empty() == true ) { return; }

	FAIL_IF ( BattleTypeDropDown == NULL ) { return; }
	FAIL_IF ( BattleTypeDropDown->Get_Selected_Index() == -1 ) { return; }

	FAIL_IF ( MapDropDown == NULL ) { return; }
	FAIL_IF ( MapDropDown->Get_Selected_Index() == -1 ) { return; }

	FAIL_IF ( PlayersTypeDropDown == NULL ) { return; }
	FAIL_IF ( PlayersTypeDropDown->Get_Selected_Index() == -1 ) { return; }

	ClientConnection::Request_Custom_Lobby( GameClient::Get_Instance()->Get_Server_Socket(), BattleNameEditBox->Get_Text(), BattleTypeDropDown->Get_Selected_Index(), MapDropDown->Get_Selected_Index(), PlayersTypeDropDown->Get_Selected_Index() );

	LobbyMenuDialogHandler::Set_Custom( false );
	Customization_Menu();
}


void LobbyMenuDialogHandler::Process_Custom_Lobby_Response( int message_buffer_id )
{
	LobbyMenuDialogHandler::Set_Lobby_Information( BattleNameEditBox->Get_Text(), MapDropDown->Get_Selected_Item(), MapDropDown->Get_Selected_Index(), BattleType(BattleTypeDropDown->Get_Selected_Index()), PlayersType(PlayersTypeDropDown->Get_Selected_Index()) );
	Fill_Lobby_Information();
	
	LobbyMenuDialogHandler::Set_Custom( false );
	Customization_Menu();
}


void LobbyMenuDialogHandler::Cancel_Custom_Lobby( void )
{
	HubMenuDialogHandler::Set_As_Active_Dialog();
}


void LobbyMenuDialogHandler::Request_Add_Chat_Line( void )
{
	FAIL_IF ( ChatInputEditBox == NULL ) { return; }

	const std::string& input_string = ChatInputEditBox->Get_Text();
	if ( input_string.empty() ) { return; }

	const std::string chat_string = HubMenuDialogHandler::Get_Character_Name() + ": " + input_string;

	ClientConnection::Request_Lobby_Chat_Message( GameClient::Get_Instance()->Get_Server_Socket(), chat_string.c_str() );
	ChatInputEditBox->Set_Text( "" );
}


void LobbyMenuDialogHandler::Request_Start_Game( void )
{
	//  TODO:  When you join a lobby, request lobby info from the server so that we have the info. Otherwise, we have nothing

	FAIL_IF ( LobbyName.length() == 0 ) { return; }
	FAIL_IF ( MapName.length() == 0 ) { return; }
	FAIL_IF ( MapIndex == -1 ) { return; }
	FAIL_IF ( BattleTypeIndex == -1 ) { return; }
	FAIL_IF ( PlayersTypeIndex == -1 ) { return; }

	ClientConnection::Request_Lobby_Start_Battle( GameClient::Get_Instance()->Get_Server_Socket() );
}


void LobbyMenuDialogHandler::Process_Lobby_Chat_Message_Response( int message_buffer_id )
{
	std::string new_text( readstring( message_buffer_id ) );

	Add_Chat_String( new_text.c_str() );
}


void LobbyMenuDialogHandler::Process_Lobby_Information( int message_buffer_id )
{
	std::string battle_name( readstring( message_buffer_id ) );
	std::string map_name( readstring( message_buffer_id ) );
	int map_index = readint( message_buffer_id );
	int battle_type = readint( message_buffer_id );
	int players_type = readint( message_buffer_id );

	LobbyMenuDialogHandler::Set_Lobby_Information( battle_name, map_name, map_index, BattleType(battle_type), PlayersType(players_type) );
	Fill_Lobby_Information();
}


void LobbyMenuDialogHandler::Process_Battle_Summons( int message_buffer_id )
{
	bool valid_map = ( readchar( message_buffer_id ) != 0 );
	if ( valid_map == true )
	{
		Process_Map_Data_Response( message_buffer_id );
		Load_Game_Menu();
	}
}


void LobbyMenuDialogHandler::Process_Lobby_Start_Battle_Response( int message_buffer_id )
{
	char failure_id = readchar( message_buffer_id );
	//  TODO:  Display a message based on the failure ID
}


void LobbyMenuDialogHandler::Process_Map_Data_Response( int message_buffer_id )
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


void LobbyMenuDialogHandler::Add_Chat_String( const char* text )
{
	FAIL_IF ( ChatFont == NULL ) { return; }

	if ( ChatFont->Get_Text_Width( text ) > (unsigned int)(CHAT_BOX_WIDTH - 8) )
	{
		unsigned int cut_spot = ChatFont->Character_Count_Before_Passing_Width( text, CHAT_BOX_WIDTH, true );
		std::string shortened_string( text );
		while ( shortened_string.length() > cut_spot ) { shortened_string.erase( --shortened_string.end() ); }
		Add_Chat_String( shortened_string.c_str() );
		Add_Chat_String( text + cut_spot );
		return;
	}

	ChatStrings.push_back( std::string( text ) );
	while ( int(ChatStrings.size()) > MAX_LOBBY_CHAT_STRINGS )
	{
		ChatStrings.erase( ChatStrings.begin() );
	}
}


void LobbyMenuDialogHandler::Add_Lobby_Player( const int player_index, const PlayerType player )
{
	for ( int i = 0; i < MAX_INSTANCE_PLAYERS; ++i )
	{
		if ( PlayerList[i].second.length() == 0 ) { continue; }
		FAIL_IF ( PlayerList[i].second.compare( player.second ) == 0 ) { return; }
	}

	//  TODO:  Use the bit flag
	PlayerList[player_index] = player;
}