#ifndef _CHARACTER_CREATION_MENU_DIALOG_HANDLER_
#define _CHARACTER_CREATION_MENU_DIALOG_HANDLER_

#include "DialogController.h"

class MenuGUIElement_Frame;
class MenuGUIElement_TextureButton;
class MenuGUIElement_TextButton;
class MenuGUIElement_EditBox;

class CharacterCreationMenuDialogHandler : public DialogHandler
{
	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new CharacterCreationMenuDialogHandler;
		ChangeActiveDialogScheduledTask* new_task = new ChangeActiveDialogScheduledTask( dialog_handler );
		TaskScheduler::Get_Instance()->Submit_Task( new_task );
	}

	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void Input( void );
	virtual void Update( float time_slice );
	virtual void Render2D( void );
	virtual bool Escalate_Message( int message_id, int message_buffer_id );

	static void Set_Commander_Slot( int slot ) { CommanderSlot = slot; }

private:
	CharacterCreationMenuDialogHandler() : 
		CharacterCreationFrame( NULL ),
		CommanderType1Button( NULL ),
		CommanderType2Button( NULL ),
		CommanderType3Button( NULL ),
		CreateCommanderButton( NULL ),
		CommanderType( -1 )
	{}
	virtual ~CharacterCreationMenuDialogHandler() {}

	void Load_Character_Menu( void );
	void Select_Commander_Type_1( void );
	void Select_Commander_Type_2( void );
	void Select_Commander_Type_3( void );
	void Create_Commander( void );
	void On_State_Changed( void );

	bool All_Information_Valid( void );
	void Process_Character_Creation_Response( int message_buffer_id );

	MenuGUIElement_Frame* CharacterCreationFrame;
	MenuGUIElement_TextureButton* CommanderType1Button;
	MenuGUIElement_TextureButton* CommanderType2Button;
	MenuGUIElement_TextureButton* CommanderType3Button;
	MenuGUIElement_TextureButton* CommanderTypeSelection;
	MenuGUIElement_TextButton* CreateCommanderButton;
	MenuGUIElement_EditBox* CharacterNameBox;

	int CommanderType;

	static int CommanderSlot;
};

#endif // _CHARACTER_MENU_DIALOG_HANDLER_