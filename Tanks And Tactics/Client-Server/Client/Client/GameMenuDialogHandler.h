#ifndef _GAME_MENU_DIALOG_HANDLER_
#define _GAME_MENU_DIALOG_HANDLER_

#include "DialogController.h"
#include "OpenGLCamera.h"

class MenuGUIElement_Frame;
class MenuGUIElement_EditBox;
class MenuGUIElement_TextButton;
class MenuGUIElement_TextBlock;
struct Font;

class GameMenuDialogHandler : public DialogHandler
{
	enum CameraActions { CAMERA_MOVE_FORWARD = 1, CAMERA_MOVE_BACKWARDS = 2, CAMERA_MOVE_LEFT = 4, CAMERA_MOVE_RIGHT = 8, CAMERA_SPIN_LEFT = 16, CAMERA_SPIN_RIGHT = 32, CAMERA_LOOK_UP = 64, CAMERA_LOOK_DOWN = 128, CAMERA_LOOK_LEFT = 256, CAMERA_LOOK_RIGHT = 512 };
	typedef int CameraActionBitFlag;

	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new GameMenuDialogHandler;
		ChangeActiveDialogScheduledTask* new_task = new ChangeActiveDialogScheduledTask( dialog_handler );
		TaskScheduler::Get_Instance()->Submit_Task( new_task );
	}

	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void Input( void );
	virtual void Update( float time_slice );
	virtual void Render2D( void );
	virtual void Render3D( void );
	virtual bool Escalate_Message( int message_id, int message_buffer_id );

	void Load_Hub_Menu( void );
	void Mid_Click( void );
	void Set_Move_Mode( void );
	void Set_Attack_Mode( void );
	void End_Current_Turn( void );

private:
	GameMenuDialogHandler() : 
		UnitOptionsFrame( NULL ),
		UnitNameTextBlock( NULL ),
		UnitTypeTextBlock( NULL ),
		UnitLevelTextBlock( NULL ),
		UnitSpeedTextBlock( NULL ),
		MoveUnitTextButton( NULL ),
		AttackUnitTextButton( NULL ),
		ChatBoxFrame( NULL ),
		ChatInputEditBox( NULL ),
		EnterTextButton( NULL ),
		ToggleChatVisibleButton( NULL ),
		CurrentTurnTextBlock( NULL ),
		EndTurnTextButton( NULL ),
		ScoreboardFrame( NULL ),
		HubMenuButton( NULL ),
		WinnerOrLoserTextBlock( NULL ),
		CameraActionFlags( 0 ),
		ChatFont( NULL ),
		StringDisplayCount( 0 ),
		ChatPosition( 0, 0 ),
		ChatDimensions( 0, 0 ),
		MidClick( false ),
		ChatVisible( false ),
		BattleInputEnabled( true )
	{}

	virtual ~GameMenuDialogHandler(){}

	void Request_Add_Chat_Line( void );
	void Process_Battle_Chat_Message_Response( int message_buffer_id );
	void Add_Chat_String( const char* text );
	
	void Process_Battle_Player_Index_Message( int message_buffer_id );
	void Process_Battle_Object_Created_Message( int message_buffer_id );
	void Process_Battle_Object_Moved_Message( int message_buffer_id );
	void Process_Battle_Object_Destroyed_Message( int message_buffer_id );
	void Process_Battle_Next_Turn( int message_buffer_id );
	void Process_Battle_Winner( int message_buffer_id );

	void Toggle_Chat_Visible( void );
	void Realign_Chat( void );

	MenuGUIElement_Frame* UnitOptionsFrame;
	MenuGUIElement_TextBlock* UnitNameTextBlock;
	MenuGUIElement_TextBlock* UnitTypeTextBlock;
	MenuGUIElement_TextBlock* UnitLevelTextBlock;
	MenuGUIElement_TextBlock* UnitSpeedTextBlock;
	MenuGUIElement_TextButton* MoveUnitTextButton;
	MenuGUIElement_TextButton* AttackUnitTextButton;
	MenuGUIElement_Frame* ChatBoxFrame;
	MenuGUIElement_EditBox* ChatInputEditBox;
	MenuGUIElement_TextButton* EnterTextButton;
	MenuGUIElement_TextButton* ToggleChatVisibleButton;
	MenuGUIElement_TextBlock* CurrentTurnTextBlock;
	MenuGUIElement_TextButton* EndTurnTextButton;
	MenuGUIElement_Frame* ScoreboardFrame;
	MenuGUIElement_TextButton* HubMenuButton;
	MenuGUIElement_TextBlock* WinnerOrLoserTextBlock;
	
	COpenGLFrame Camera;
	CameraActionBitFlag CameraActionFlags;
	
	Font* ChatFont;
	unsigned int StringDisplayCount;
	std::vector< std::string > ChatStrings;
	std::pair< int, int > ChatPosition;
	std::pair< int, int > ChatDimensions;

	bool MidClick;
	bool ChatVisible;
	bool BattleInputEnabled;
};

#endif // _GAME_MENU_DIALOG_HANDLER_