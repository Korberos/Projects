#include "MainMenuDialogHandler.h"

#include "GameClient.h"
#include "ClientConnection.h"
#include "CharacterMenuDialogHandler.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"
#include "DialogSystem/Elements/MenuElement_EditBox.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"

void MainMenuDialogHandler::Activate( void )
{
	BASECLASS::Activate();

	//  Initialize the MenuDialog for this handler
	Menu = MenuGUI::Load_Menu_GUI_File( "Assets/Menus/ClientMainMenu.xml" );
	FAIL_IF ( !Menu ) { return; }

	//  Set up a handle to the VersionText
	VersionText = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element("VersionText"));
	ENFORCED_IF ( VersionText != NULL )
	{
		VersionText->Set_Text( CLIENT_VERSION );
	}

	//  Set up a handle to the RequestNewsFeedButton
	RequestNewsFeedButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("RequestNewsFeedButton"));
	ENFORCED_IF ( RequestNewsFeedButton != NULL )
	{
		Register_Event_Handler( RequestNewsFeedButton, IT_UNCLICK, &MainMenuDialogHandler::Request_News_Feed );
	}

	//  Apply the return and iteration callbacks to the edit boxes
	UsernameEditBox = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("UsernameEditBox"));
	ENFORCED_IF ( UsernameEditBox )
	{
		Register_Event_Handler( UsernameEditBox, IT_CONFIRM, &MainMenuDialogHandler::Login_Account );
		Register_Event_Handler( UsernameEditBox, IT_ITERATE, &MainMenuDialogHandler::Focus_On_Password_Box );
	}

	PasswordEditBox = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("PasswordEditBox"));
	ENFORCED_IF ( PasswordEditBox )
	{
		Register_Event_Handler( PasswordEditBox, IT_CONFIRM, &MainMenuDialogHandler::Login_Account );
		Register_Event_Handler( PasswordEditBox, IT_ITERATE, &MainMenuDialogHandler::Focus_On_Username_Box );
	}

	Focus_On_Username_Box();

	//  Set up a handle to the RegisterButton
	RegisterButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("RegisterButton"));
	ENFORCED_IF ( RegisterButton != NULL )
	{
		Register_Event_Handler( RegisterButton, IT_UNCLICK, &MainMenuDialogHandler::Register_Account );
	}

	//  Set up a handle to the LoginButton
	LoginButton = dynamic_cast<MenuGUIElement_TextButton*>(Menu->Get_Element("LoginButton"));
	ENFORCED_IF ( LoginButton != NULL )
	{
		Register_Event_Handler( LoginButton, IT_UNCLICK, &MainMenuDialogHandler::Login_Account );
	}

	char text_block_name[64];
	for ( int i = 0; i < NEWS_ENTRIES_REQUESTED; ++i )
	{
		sprintf_s( text_block_name, "NewsFeed%d_Headline", i + 1 );
		MenuGUIElement_TextBlock* headline_text_block = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element(text_block_name));
		Register_Event_Handler( headline_text_block, IT_CLICK, &MainMenuDialogHandler::Visit_Main_Site );
	}

	DebugConsole::Get_Instance()->Add_String( "Main Menu Initialized" );
	Request_News_Feed();
}


void MainMenuDialogHandler::Deactivate( void )
{
	BASECLASS::Deactivate();
}


void MainMenuDialogHandler::Input( void )
{
	BASECLASS::Input();
}


void MainMenuDialogHandler::Update( float time_slice )
{
	BASECLASS::Update( time_slice );
}


void MainMenuDialogHandler::Render2D( void )
{
	BASECLASS::Render2D();
}


bool MainMenuDialogHandler::Escalate_Message( int message_id, int message_buffer_id )
{
	switch ( message_id )
	{
	case SERVER_TO_CLIENT_MESSAGE_ID_NEWS_RESPONSE:
		Process_News_Feed( message_buffer_id );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_LOGIN_RESPONSE:
		Process_Login_Request_Response( readchar( message_buffer_id ) );
		break;

	case SERVER_TO_CLIENT_MESSAGE_ID_REGISTER_RESPONSE:
		Process_Registration_Request_Response( readchar( message_buffer_id ) );
		break;

	default:
		return false;
		break;
	}

	return true;
}


void MainMenuDialogHandler::Request_News_Feed( void )
{
	ClientConnection::Request_News_Feed( GameClient::Get_Instance()->Get_Server_Socket() );
}


void MainMenuDialogHandler::Process_News_Feed( int message_buffer_id )
{
	FAIL_IF ( !Menu ) { return; }

	char text_block_name[64];
	for ( int i = 0; i < NEWS_ENTRIES_REQUESTED; ++i )
	{
		sprintf_s( text_block_name, "NewsFeed%d_Headline", i + 1 );
		MenuGUIElement_TextBlock* headline_text_block = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element(text_block_name));
		sprintf_s( text_block_name, "NewsFeed%d_Body", i + 1 );
		MenuGUIElement_TextBlock* body_text_block = dynamic_cast<MenuGUIElement_TextBlock*>(Menu->Get_Element(text_block_name));

		//  If either menu element did not load, assert and ensure that we don't try to use them
		FAIL_IF ( !headline_text_block || !body_text_block ) { continue; }

		char timestamp[64];
		strcpy_s( timestamp, 64, readstring( message_buffer_id ) );

		char headline[64];
		strcpy_s( headline, 64, readstring( message_buffer_id ) );
		
		char headline_string[128];
		sprintf_s( headline_string, 128, "%s -- %s", headline, timestamp );

		headline_text_block->Set_Text( headline_string );

		char body_string[256];
		strcpy_s( body_string, 256, readstring( message_buffer_id ) );

		body_text_block->Set_Text( body_string );
	}
}


void MainMenuDialogHandler::Login_Account( void )
{
	FAIL_IF ( !Menu ) { return; }

	MenuGUIElement_EditBox* username_edit_box = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("UsernameEditBox"));
	if ( username_edit_box == NULL || username_edit_box->Get_Text().empty() ) return;
	MenuGUIElement_EditBox* password_edit_box = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("PasswordEditBox"));
	if ( password_edit_box == NULL || password_edit_box->Get_Text().empty() ) return;

	ClientConnection::Request_Login( GameClient::Get_Instance()->Get_Server_Socket(), username_edit_box->Get_Text().c_str(), password_edit_box->Get_Text().c_str() );

	username_edit_box->Set_Text( "" );
	password_edit_box->Set_Text( "" );
}


void MainMenuDialogHandler::Process_Login_Request_Response( char response_id )
{
	switch ( response_id )
	{
	case LOGIN_RESPONSE_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Unknown failure when logging in. Please contact administration." );
		break;

	case LOGIN_RESPONSE_ACCOUNT_DOES_NOT_EXIST:
		DebugConsole::Get_Instance()->Add_String( "Account does not exist. Did you spell it wrong?" );
		break;

	case LOGIN_RESPONSE_PASSWORD_INCORRECT:
		DebugConsole::Get_Instance()->Add_String( "Username or password incorrect. Please try again." );
		break;

	case LOGIN_RESPONSE_SUCCESS:
		CharacterMenuDialogHandler::Set_As_Active_Dialog();
		DebugConsole::Get_Instance()->Add_String( "Login successful! Taking you to the game..." );
		break;
	}
}


void MainMenuDialogHandler::Register_Account( void )
{
	FAIL_IF ( !Menu ) { return; }

	MenuGUIElement_EditBox* username_edit_box = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("UsernameEditBox"));
	if ( username_edit_box == NULL || username_edit_box->Get_Text().empty() ) return;
	MenuGUIElement_EditBox* password_edit_box = dynamic_cast<MenuGUIElement_EditBox*>(Menu->Get_Element("PasswordEditBox"));
	if ( password_edit_box == NULL || password_edit_box->Get_Text().empty() ) return;

	ClientConnection::Request_Register( GameClient::Get_Instance()->Get_Server_Socket(), username_edit_box->Get_Text().c_str(), password_edit_box->Get_Text().c_str() );

	username_edit_box->Set_Text( "" );
	password_edit_box->Set_Text( "" );
}


void MainMenuDialogHandler::Process_Registration_Request_Response( char response_id )
{
	switch ( response_id )
	{
	case REGISTRATION_RESPONSE_UNKNOWN_FAILURE:
		DebugConsole::Get_Instance()->Add_String( "Unknown failure when registering. Please contact administration." );
		break;

	case REGISTRATION_RESPONSE_ACCOUNT_TAKEN:
		DebugConsole::Get_Instance()->Add_String( "Account already taken. Please try another name." );
		break;

	case REGISTRATION_RESPONSE_SUCCESS:
		DebugConsole::Get_Instance()->Add_String( "Registration successful! You may now log in." );
		break;
	}
}


void MainMenuDialogHandler::Visit_Main_Site( void )
{
	ShellExecute(NULL, L"open", L"http://www.drewbanyai.com", NULL, NULL, SW_SHOWNORMAL);
}


void MainMenuDialogHandler::Focus_On_Username_Box( void )
{
	UsernameEditBox->Set_Clicked( true );
	PasswordEditBox->Set_Clicked( false );
}


void MainMenuDialogHandler::Focus_On_Password_Box( void )
{
	PasswordEditBox->Set_Clicked( true );
	UsernameEditBox->Set_Clicked( false );
}