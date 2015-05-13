#ifndef _MAIN_MENU_DIALOG_HANDLER_
#define _MAIN_MENU_DIALOG_HANDLER_

#include "DialogController.h"

class MenuGUIElement_TextBlock;
class MenuGUIElement_TextButton;
class MenuGUIElement_EditBox;

class MainMenuDialogHandler : public DialogHandler
{
	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new MainMenuDialogHandler;
		ChangeActiveDialogScheduledTask* new_task = new ChangeActiveDialogScheduledTask( dialog_handler );
		TaskScheduler::Get_Instance()->Submit_Task( new_task );
	}

	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void Input( void );
	virtual void Update( float time_slice );
	virtual void Render2D( void );
	virtual bool Escalate_Message( int message_id, int message_buffer_id );

	void Request_News_Feed( void );
	void Process_News_Feed( int message_buffer_id );
	void Login_Account( void );
	void Process_Login_Request_Response( char response_id );
	void Register_Account( void );
	void Process_Registration_Request_Response( char response_id );
	void Visit_Main_Site( void );
	void Focus_On_Username_Box( void );
	void Focus_On_Password_Box( void );

private:
	MainMenuDialogHandler() : 
		VersionText( NULL ),
		RequestNewsFeedButton( NULL ),
		RegisterButton( NULL ),
		LoginButton( NULL )
	{}
	virtual ~MainMenuDialogHandler(){}

	MenuGUIElement_TextBlock*	VersionText;
	MenuGUIElement_TextButton*	RequestNewsFeedButton;
	MenuGUIElement_EditBox*		UsernameEditBox;
	MenuGUIElement_EditBox*		PasswordEditBox;
	MenuGUIElement_TextButton*	RegisterButton;
	MenuGUIElement_TextButton* LoginButton;
};

#endif // _MAIN_MENU_DIALOG_HANDLER_