#ifndef _CHARACTER_MENU_DIALOG_HANDLER_
#define _CHARACTER_MENU_DIALOG_HANDLER_

#include "DialogController.h"
#include "../../ClientServerCommon.h"

class MenuGUIElement_TextureButton;
class MenuGUIElement_TextButton;
class MenuGUIElement_TextBlock;
class MenuGUIElement_Frame;

class CharacterDeletionRequest
{
public:
	CharacterDeletionRequest() : 
		CharacterSlot( -1 ),
		TimeToDeletion( -1.0f )
	{}

	int CharacterSlot;
	float TimeToDeletion;
};

class CharacterMenuDialogHandler : public DialogHandler
{
	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new CharacterMenuDialogHandler;
		ChangeActiveDialogScheduledTask* new_task = new ChangeActiveDialogScheduledTask( dialog_handler );
		TaskScheduler::Get_Instance()->Submit_Task( new_task );
	}

	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void Input( void );
	virtual void Update( float time_slice );
	virtual void Render2D( void );
	virtual bool Escalate_Message( int message_id, int message_buffer_id );

private:
	CharacterMenuDialogHandler() : 
		CommanderButton1( NULL ),
		CommanderButton2( NULL ),
		CommanderButton3( NULL ),
		CommanderSelectionRing( NULL ),
		CreateCommanderButton( NULL ),
		DeleteCommanderButton( NULL ),
		PlayCommanderButton( NULL ),
		CommanderName1( NULL ),
		CommanderName2( NULL ),
		CommanderName3( NULL ),
		ConfirmingCharacterDeletionFrame( NULL ),
		CancelCharacterDeletionButton( NULL ),
		CharacterDeletionText( NULL ),
		CommanderSelection( 1 )
	{}
	virtual ~CharacterMenuDialogHandler(){}

	void Select_Commander_1( void );
	void Select_Commander_2( void );
	void Select_Commander_3( void );
	void Iterate_Commander( void );
	void Confirm_Commander( void );
	void Load_Hub_Menu( void );
	void Load_Main_Menu( void );
	void Request_Character_Data( void );
	void Process_Character_Data( int message_buffer_id );
	void Process_Character_Deletion( int message_buffer_id );
	void Create_Commander_Menu( void );
	void Delete_Commander_Menu( void );
	void Cancel_Character_Deletion( void );

	void Rebrand_Character_Slots( void );

	MenuGUIElement_TextureButton* CommanderButton1;
	MenuGUIElement_TextureButton* CommanderButton2;
	MenuGUIElement_TextureButton* CommanderButton3;
	MenuGUIElement_TextureButton* CommanderSelectionRing;
	MenuGUIElement_TextButton* CreateCommanderButton;
	MenuGUIElement_TextButton* DeleteCommanderButton;
	MenuGUIElement_TextButton* PlayCommanderButton;
	MenuGUIElement_TextBlock* CommanderName1;
	MenuGUIElement_TextBlock* CommanderName2;
	MenuGUIElement_TextBlock* CommanderName3;
	MenuGUIElement_Frame* ConfirmingCharacterDeletionFrame;
	MenuGUIElement_TextButton* CancelCharacterDeletionButton;
	MenuGUIElement_TextBlock* CharacterDeletionText;

	int CommanderSelection;
	CharacterData CommanderData[3];

	CharacterDeletionRequest CharacterDeletion;
};

#endif // _CHARACTER_MENU_DIALOG_HANDLER_