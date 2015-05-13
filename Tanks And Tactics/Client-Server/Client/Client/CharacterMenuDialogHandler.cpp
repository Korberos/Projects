#include "CharacterMenuDialogHandler.h"

#include "MainMenuDialogHandler.h"
#include "HubMenuDialogHandler.h"
#include "CharacterCreationMenuDialogHandler.h"
#include "ClientConnection.h"
#include "GameClient.h"
#include "DialogSystem/MenuGUIElement.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_TextureButton.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"


void CharacterMenuDialogHandler::Activate( void )
{
	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientCharacterMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	CommanderButton1 = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("Commander_01"));
	ENFORCED_IF (CommanderButton1 != NULL)
	{
		Register_Event_Handler( CommanderButton1, IT_UNCLICK, &CharacterMenuDialogHandler::Select_Commander_1 );
	}

	CommanderButton2 = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("Commander_02"));
	ENFORCED_IF (CommanderButton2 != NULL)
	{
		Register_Event_Handler( CommanderButton2, IT_UNCLICK, &CharacterMenuDialogHandler::Select_Commander_2 );
	}

	CommanderButton3 = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("Commander_03"));
	ENFORCED_IF (CommanderButton3 != NULL)
	{
		Register_Event_Handler( CommanderButton3, IT_UNCLICK, &CharacterMenuDialogHandler::Select_Commander_3 );
	}

	MenuGUIElement_TextButton* main_menu_button = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("MainMenuButton"));
	ENFORCED_IF (main_menu_button != NULL)
	{
		Register_Event_Handler( main_menu_button, IT_UNCLICK, &CharacterMenuDialogHandler::Load_Main_Menu );
	}

	CommanderSelectionRing = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("Commander_Selection"));
	ENFORCED_IF (CommanderSelectionRing != NULL)
	{
		CommanderSelectionRing->Set_Visible( false );
		Register_Event_Handler( CommanderSelectionRing, IT_ITERATE, &CharacterMenuDialogHandler::Iterate_Commander );
		Register_Event_Handler( CommanderSelectionRing, IT_CONFIRM, &CharacterMenuDialogHandler::Confirm_Commander );
	}

	CreateCommanderButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CreateCommanderButton"));
	ENFORCED_IF ( CreateCommanderButton != NULL )
	{
		CreateCommanderButton->Set_Visible( false );
		Register_Event_Handler( CreateCommanderButton, IT_UNCLICK, &CharacterMenuDialogHandler::Create_Commander_Menu );
	}

	DeleteCommanderButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("DeleteCommanderButton"));
	ENFORCED_IF ( DeleteCommanderButton != NULL )
	{
		DeleteCommanderButton->Set_Visible( false );
		Register_Event_Handler( DeleteCommanderButton, IT_UNCLICK, &CharacterMenuDialogHandler::Delete_Commander_Menu );
	}

	PlayCommanderButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("PlayCommanderButton"));
	ENFORCED_IF ( PlayCommanderButton != NULL )
	{
		PlayCommanderButton->Set_Visible( false );
		Register_Event_Handler( PlayCommanderButton, IT_UNCLICK, &CharacterMenuDialogHandler::Load_Hub_Menu );
	}

	ConfirmingCharacterDeletionFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("ConfirmingCharacterDeletionFrame"));
	ENFORCED_IF ( ConfirmingCharacterDeletionFrame != NULL )
	{
		ConfirmingCharacterDeletionFrame->Set_Visible( false );
	}

	CancelCharacterDeletionButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CancelCharacterDeletionButton"));
	ENFORCED_IF ( CancelCharacterDeletionButton != NULL )
	{
		Register_Event_Handler( CancelCharacterDeletionButton, IT_UNCLICK, &CharacterMenuDialogHandler::Cancel_Character_Deletion );
	}

	CommanderName1 = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("Commander_01_Name"));
	ASSERT ( CommanderName1 != NULL );

	CommanderName2 = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("Commander_02_Name"));
	ASSERT ( CommanderName2 != NULL );

	CommanderName3 = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("Commander_03_Name"));
	ASSERT ( CommanderName3 != NULL );

	CharacterDeletionText = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("CharacterDeletionText"));
	ASSERT ( CharacterDeletionText != NULL );

	DebugConsole::Get_Instance()->Add_String( "Character Menu Initialized" );
	Request_Character_Data();
}


void CharacterMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
}


void CharacterMenuDialogHandler::Input( void )
{
	BASECLASS::Input();
}


void CharacterMenuDialogHandler::Update( float time_slice )
{
	BASECLASS::Update( time_slice );

	if ( CharacterDeletion.TimeToDeletion > 0.0f )
	{
		CharacterDeletion.TimeToDeletion -= time_slice;
		if ( CharacterDeletion.TimeToDeletion < 0 )
		{
			ClientConnection::Request_Character_Deletion( CommanderData[CommanderSelection - 1].ID, GameClient::Get_Instance()->Get_Server_Socket() );
			Cancel_Character_Deletion();
		}
		else
		{
			ENFORCED_IF ( CharacterDeletionText )
			{
				char deletion_text[256];
				sprintf_s( deletion_text, "Character will be deleted in %d seconds. If you change your mind, click Cancel.", int(CharacterDeletion.TimeToDeletion) );
				CharacterDeletionText->Set_Text( std::string( deletion_text ) );
			}
		}
	}
}


void CharacterMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();
}


bool CharacterMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_DATA_RESPONSE:
		Process_Character_Data( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_DELETION_RESPONSE:
		Process_Character_Deletion( message_buffer_id );
		break;

	default:
		return false;
		break;
	}

	return true;
}


void CharacterMenuDialogHandler::Select_Commander_1( void )
{
	if ( CommanderSelection == 1 ) return;

	CommanderSelection = 1;
	ENFORCED_IF ( CommanderSelectionRing )
	{
		CommanderSelectionRing->Set_Visible( true );
		CommanderSelectionRing->Set_X( CommanderButton1->Get_X() );
		CommanderSelectionRing->Set_Focused( true );
	}

	bool commander_selected = CommanderData[0].ID != -1;
	ENFORCED_IF ( CreateCommanderButton )
	{
		CreateCommanderButton->Set_Visible( !commander_selected );
		CreateCommanderButton->Set_X( CommanderButton1->Get_X() );
	}
	ENFORCED_IF ( DeleteCommanderButton ) { DeleteCommanderButton->Set_Visible( commander_selected ); }
	ENFORCED_IF ( PlayCommanderButton ) { PlayCommanderButton->Set_Visible( commander_selected ); }
}


void CharacterMenuDialogHandler::Select_Commander_2( void )
{
	if ( CommanderSelection == 2 ) return;

	CommanderSelection = 2;
	ENFORCED_IF ( CommanderSelectionRing )
	{
		CommanderSelectionRing->Set_Visible( true );
		CommanderSelectionRing->Set_X( CommanderButton2->Get_X() );
		CommanderSelectionRing->Set_Focused( true );
	}

	bool commander_selected = CommanderData[1].ID != -1;
	ENFORCED_IF ( CreateCommanderButton )
	{
		CreateCommanderButton->Set_Visible( !commander_selected );
		CreateCommanderButton->Set_X( CommanderButton2->Get_X() );
	}
	ENFORCED_IF ( DeleteCommanderButton ) { DeleteCommanderButton->Set_Visible( commander_selected ); }
	ENFORCED_IF ( PlayCommanderButton ) { PlayCommanderButton->Set_Visible( commander_selected ); }
}


void CharacterMenuDialogHandler::Select_Commander_3( void )
{
	if ( CommanderSelection == 3 ) return;

	CommanderSelection = 3;
	ENFORCED_IF ( CommanderSelectionRing )
	{
		CommanderSelectionRing->Set_Visible( true );
		CommanderSelectionRing->Set_X( CommanderButton3->Get_X() );
		CommanderSelectionRing->Set_Focused( true );
	}

	bool commander_selected = CommanderData[2].ID != -1;
	ENFORCED_IF ( CreateCommanderButton && CommanderButton3 )
	{
		CreateCommanderButton->Set_Visible( !commander_selected );
		CreateCommanderButton->Set_X( CommanderButton3->Get_X() );
	}
	ENFORCED_IF ( DeleteCommanderButton ) { DeleteCommanderButton->Set_Visible( commander_selected ); }
	ENFORCED_IF ( PlayCommanderButton ) { PlayCommanderButton->Set_Visible( commander_selected ); }
}


void CharacterMenuDialogHandler::Iterate_Commander( void )
{
	switch ( CommanderSelection )
	{
	case 1:
		Select_Commander_2();
		break;
		
	case 2:
		Select_Commander_3();
		break;
		
	case 3:
		Select_Commander_1();
		break;
	}
}


void CharacterMenuDialogHandler::Confirm_Commander( void )
{
	if ( CommanderSelection <= 0 ) { return; }

	if ( CommanderData[CommanderSelection - 1].ID != -1 )
	{
		Load_Hub_Menu();
	}
	else
	{
		Create_Commander_Menu();
	}
}


void CharacterMenuDialogHandler::Load_Hub_Menu( void )
{
	FAIL_IF ( !Menu ) { return; }

	if ( CommanderSelection == -1 || CommanderData[CommanderSelection - 1].ID == -1 )
	{
		DebugConsole::Get_Instance()->Add_String( "You must select a commander to continue." );
		return;
	}

	ClientConnection::Request_Character_Choice( GameClient::Get_Instance()->Get_Server_Socket(), CommanderData[CommanderSelection - 1].ID );

	HubMenuDialogHandler::Set_Character_Name( CommanderData[CommanderSelection - 1].Name );
	HubMenuDialogHandler::Set_As_Active_Dialog();
}


void CharacterMenuDialogHandler::Load_Main_Menu( void )
{
	MainMenuDialogHandler::Set_As_Active_Dialog();
}


void CharacterMenuDialogHandler::Request_Character_Data( void )
{
	ClientConnection::Request_Character_Data( GameClient::Get_Instance()->Get_Server_Socket() );
}


void CharacterMenuDialogHandler::Process_Character_Data( int message_buffer_id )
{
	//  Clear out current known character information
	for ( unsigned int i = 0; i < 3; ++i ) 
	{
		CommanderData[i].ID = -1;
		CommanderData[i].Type = -1;
		CommanderData[i].Name[0] = 0;
	}

	unsigned char response = readchar( message_buffer_id );
	switch ( response )
	{
	case CHARACTER_DATA_RESPONSE_UNKNOWN_FAILURE:
	case CHARACTER_DATA_RESPONSE_CHARACTER_COUNT_OFF:
		DebugConsole::Get_Instance()->Add_String( "Failure when requesting character list. Please contact administration." );
		break;

	case CHARACTER_DATA_RESPONSE_SUCCESS:
		int character_count = readint( message_buffer_id );

		for ( int i = 0; i < character_count; ++i )
		{
			CommanderData[i].ID = readint( message_buffer_id );
			if ( CommanderData[i].ID == -1 )
			{
				CommanderData[i].Type = -1;
				continue;
			}

			strcpy_s( CommanderData[i].Name, 31, readstring( message_buffer_id ) );
			CommanderData[i].Type = readint( message_buffer_id );
			CommanderData[i].Level = readint( message_buffer_id );
			CommanderData[i].Victories = readint( message_buffer_id );
			CommanderData[i].Defeats = readint( message_buffer_id );
		}
		break;
	}

	Rebrand_Character_Slots();
}


void CharacterMenuDialogHandler::Process_Character_Deletion( int message_buffer_id )
{
	unsigned char response = readchar( message_buffer_id );
	switch ( response )
	{
	case CHARACTER_DELETION_RESPONSE_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Failure when requesting character deletion. Please contact administration." );
		break;

	case CHARACTER_DELETION_RESPONSE_SUCCESS:
		Request_Character_Data();
		break;
	}

	Rebrand_Character_Slots();
}


void CharacterMenuDialogHandler::Create_Commander_Menu( void )
{
	CharacterCreationMenuDialogHandler::Set_As_Active_Dialog();
	CharacterCreationMenuDialogHandler::Set_Commander_Slot( CommanderSelection );
}


void CharacterMenuDialogHandler::Delete_Commander_Menu( void )
{
	ENFORCED_IF ( ConfirmingCharacterDeletionFrame != NULL )
	{
		FAIL_IF ( ConfirmingCharacterDeletionFrame->Get_Visible() == true ) { return; }
		ConfirmingCharacterDeletionFrame->Set_Visible( true );

		CharacterDeletion.CharacterSlot = CommanderSelection;
		CharacterDeletion.TimeToDeletion = 10.0f;
	}
}


void CharacterMenuDialogHandler::Cancel_Character_Deletion( void )
{
	ENFORCED_IF ( ConfirmingCharacterDeletionFrame != NULL )
	{
		FAIL_IF ( ConfirmingCharacterDeletionFrame->Get_Visible() == false ) { return; }
		ConfirmingCharacterDeletionFrame->Set_Visible( false );

		CharacterDeletion.CharacterSlot = -1;
		CharacterDeletion.TimeToDeletion = -1.0f;
	}
}


void CharacterMenuDialogHandler::Rebrand_Character_Slots( void )
{
	ENFORCED_IF ( CommanderButton1 != NULL ) 
	{
		char file_name[128];
		sprintf_s( file_name, 127, "Assets/Images/CommanderImage%d.png", CommanderData[0].Type == -1 ? 0 : CommanderData[0].Type );
		CommanderButton1->Set_Texture( file_name );
		ENFORCED_IF ( CommanderName1 != NULL )
		{
			CommanderName1->Set_Text( CommanderData[0].Type == -1 ? "" : CommanderData[0].Name );
		}
	}
	
	ENFORCED_IF ( CommanderButton2 != NULL ) 
	{
		char file_name[128];
		sprintf_s( file_name, 127, "Assets/Images/CommanderImage%d.png", CommanderData[1].Type == -1 ? 0 : CommanderData[1].Type );
		CommanderButton2->Set_Texture( file_name );
		ENFORCED_IF ( CommanderName2 != NULL )
		{
			CommanderName2->Set_Text( CommanderData[1].Type == -1 ? "" : CommanderData[1].Name );
		}
	}
	
	ENFORCED_IF ( CommanderButton3 != NULL ) 
	{
		char file_name[128];
		sprintf_s( file_name, 127, "Assets/Images/CommanderImage%d.png", CommanderData[2].Type == -1 ? 0 : CommanderData[2].Type );
		CommanderButton3->Set_Texture( file_name );
		ENFORCED_IF ( CommanderName3 != NULL )
		{
			CommanderName3->Set_Text( CommanderData[2].Type == -1 ? "" : CommanderData[2].Name );
		}
	}

	int old_selection = CommanderSelection;
	CommanderSelection = -1;
	switch ( old_selection )
	{
	case 1:
		Select_Commander_1();
		break;
		
	case 2:
		Select_Commander_2();
		break;
		
	case 3:
		Select_Commander_3();
		break;
	}
}