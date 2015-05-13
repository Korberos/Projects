#include "CharacterCreationMenuDialogHandler.h"

#include "CharacterMenuDialogHandler.h"
#include "ClientConnection.h"
#include "GameClient.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_TextureButton.h"
#include "DialogSystem/Elements/MenuElement_EditBox.h"

int CharacterCreationMenuDialogHandler::CommanderSlot = -1;

void CharacterCreationMenuDialogHandler::Activate( void )
{
	FAIL_IF ( CommanderSlot == -1 && "No commander slot is selected." )
	{
		Load_Character_Menu();
		return;
	}

	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientCharacterCreationMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	MenuGUIElement_TextButton* character_menu_button = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CharacterMenuButton"));
	ENFORCED_IF (character_menu_button != NULL)
	{
		Register_Event_Handler( character_menu_button, IT_UNCLICK, &CharacterCreationMenuDialogHandler::Load_Character_Menu );
	}

	CommanderType1Button = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("CommanderType1Button"));
	ENFORCED_IF (CommanderType1Button != NULL)
	{
		Register_Event_Handler( CommanderType1Button, IT_UNCLICK, &CharacterCreationMenuDialogHandler::Select_Commander_Type_1 );
	}

	CommanderType2Button = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("CommanderType2Button"));
	ENFORCED_IF (CommanderType2Button != NULL)
	{
		Register_Event_Handler( CommanderType2Button, IT_UNCLICK, &CharacterCreationMenuDialogHandler::Select_Commander_Type_2 );
	}

	CommanderType3Button = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("CommanderType3Button"));
	ENFORCED_IF (CommanderType3Button != NULL)
	{
		Register_Event_Handler( CommanderType3Button, IT_UNCLICK, &CharacterCreationMenuDialogHandler::Select_Commander_Type_3 );
	}

	CreateCommanderButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("CreateCommanderButton"));
	ENFORCED_IF (CreateCommanderButton != NULL)
	{
		Register_Event_Handler( CreateCommanderButton, IT_UNCLICK, &CharacterCreationMenuDialogHandler::Create_Commander );
		CreateCommanderButton->Set_Visible( false );
	}

	CharacterNameBox = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("CharacterNameBox"));
	ENFORCED_IF ( CharacterNameBox != NULL )
	{
		Register_Event_Handler( CharacterNameBox, IT_TEXTINPUT, &CharacterCreationMenuDialogHandler::On_State_Changed );
	}

	CommanderTypeSelection = dynamic_cast<MenuGUIElement_TextureButton*>(Menu->Get_Element("CommanderTypeSelection"));
	ENFORCED_IF (CommanderTypeSelection != NULL)
	{
		CommanderTypeSelection->Set_Visible( false );
	}

	CharacterCreationFrame = dynamic_cast<MenuGUIElement_Frame*>(Menu->Get_Element("CharacterCreationFrame"));
	ASSERT ( CharacterCreationFrame != NULL );

	DebugConsole::Get_Instance()->Add_String( "Character Creation Menu Initialized" );
}


void CharacterCreationMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
	CommanderSlot = -1;
}


void CharacterCreationMenuDialogHandler::Input( void )
{
	BASECLASS::Input();
}


void CharacterCreationMenuDialogHandler::Update( float time_slice )
{
	BASECLASS::Update( time_slice );
}


void CharacterCreationMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();
}


bool CharacterCreationMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_CHARACTER_CREATION_RESPONSE:
		Process_Character_Creation_Response( message_buffer_id );
		break;

	default:
		return false;
		break;
	}

	return true;
}


void CharacterCreationMenuDialogHandler::Load_Character_Menu( void )
{
	CharacterMenuDialogHandler::Set_As_Active_Dialog();
}


void CharacterCreationMenuDialogHandler::Select_Commander_Type_1( void )
{
	if ( CommanderType == 1 ) return;

	CommanderType = 1;
	ENFORCED_IF ( CommanderTypeSelection )
	{
		CommanderTypeSelection->Set_Visible( true );
		CommanderTypeSelection->Set_Y( CommanderType1Button->Get_Y() );
	}

	On_State_Changed();
}


void CharacterCreationMenuDialogHandler::Select_Commander_Type_2( void )
{
	if ( CommanderType == 2 ) return;

	CommanderType = 2;
	ENFORCED_IF ( CommanderTypeSelection )
	{
		CommanderTypeSelection->Set_Visible( true );
		CommanderTypeSelection->Set_Y( CommanderType2Button->Get_Y() );
	}

	On_State_Changed();
}


void CharacterCreationMenuDialogHandler::Select_Commander_Type_3( void )
{
	if ( CommanderType == 3 ) return;

	CommanderType = 3;
	ENFORCED_IF ( CommanderTypeSelection )
	{
		CommanderTypeSelection->Set_Visible( true );
		CommanderTypeSelection->Set_Y( CommanderType3Button->Get_Y() );
	}

	On_State_Changed();
}


void CharacterCreationMenuDialogHandler::Create_Commander( void )
{
	FAIL_IF ( All_Information_Valid() == false )
	{
		CreateCommanderButton->Set_Visible( false );
		return;
	}

	ClientConnection::Request_Character_Creation( CommanderType, CharacterNameBox->Get_Text(), GameClient::Get_Instance()->Get_Server_Socket() );
}



void CharacterCreationMenuDialogHandler::On_State_Changed( void )
{
	ENFORCED_IF ( CreateCommanderButton )
	{
		CreateCommanderButton->Set_Visible( All_Information_Valid() );
	}
}


bool CharacterCreationMenuDialogHandler::All_Information_Valid( void )
{
	if ( CommanderType == -1 ) { return false; }
	if ( CharacterNameBox->Get_Text().empty() ) { return false; }

	return true;
}


void CharacterCreationMenuDialogHandler::Process_Character_Creation_Response( int message_buffer_id )
{
	char response = readchar( message_buffer_id );
	switch ( response )
	{
	case CHARACTER_CREATION_RESPONSE_CHARACTER_NAME_IN_USE:
		DebugConsole::Get_Instance()->Add_String( "This character name is already in use. Please try another name." );
		return;
		break;

	case CHARACTER_CREATION_RESPONSE_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Unknown failure when creating character. Please inform administration if this problem persists. " );
		break;

	case CHARACTER_CREATION_RESPONSE_SUCCESS:
		break;
	}

	Load_Character_Menu();
}