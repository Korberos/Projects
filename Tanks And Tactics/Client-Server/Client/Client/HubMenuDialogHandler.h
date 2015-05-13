#ifndef _HUB_MENU_DIALOG_HANDLER_
#define _HUB_MENU_DIALOG_HANDLER_

#include "DialogController.h"
#include "OpenGLCamera.h"

class MenuGUIElement_Frame;
class MenuGUIElement_TextButton;

class HubMenuDialogHandler : public DialogHandler
{
	enum CameraActions { CAMERA_MOVE_FORWARD = 1, CAMERA_MOVE_BACKWARDS = 2, CAMERA_MOVE_LEFT = 4, CAMERA_MOVE_RIGHT = 8, CAMERA_SPIN_LEFT = 16, CAMERA_SPIN_RIGHT = 32, CAMERA_LOOK_UP = 64, CAMERA_LOOK_DOWN = 128, CAMERA_LOOK_LEFT = 256, CAMERA_LOOK_RIGHT = 512 };
	typedef int CameraActionBitFlag;

	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new HubMenuDialogHandler;
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

	void Load_Character_Menu( void );
	void Load_Custom_Lobby_Menu( void );
	void Load_Game_Menu( void );
	void Load_Barracks_Menu( void );
	void Load_Summoner_Menu( void );
	void Cancel_Dialog( void );
	void Request_Random_Lobby_Join( void );
	void Request_Random_Battle_Join( void );
	void Process_Join_Random_Lobby_Response( int message_buffer_id );
	void Process_Join_Random_Battle_Response( int message_buffer_id );
	void Process_Map_Data_Response( int message_buffer_id );

	static void Set_Character_Name( std::string name )			{ CharacterName = name; }
	static const std::string& Get_Character_Name( void )		{ return CharacterName; }

private:
	HubMenuDialogHandler() : 
		CameraActionFlags( 0 ),
		BarracksDialogFrame( NULL ),
		SummonerDialogFrame( NULL ),
		CancelDialogTextButton( NULL ),
		JoinRandomLobbyTextButton( NULL ),
		JoinRandomBattleTextButton( NULL ),
		CharacterMenuTextButton( NULL ),
		CustomBattleTextButton( NULL )
	{
	}
	virtual ~HubMenuDialogHandler(){}
	
	COpenGLFrame Camera;
	CameraActionBitFlag CameraActionFlags;

	static std::string CharacterName;
	
	MenuGUIElement_Frame* BarracksDialogFrame;
	MenuGUIElement_Frame* SummonerDialogFrame;
	MenuGUIElement_TextButton* CancelDialogTextButton;
	MenuGUIElement_TextButton* JoinRandomLobbyTextButton;
	MenuGUIElement_TextButton* JoinRandomBattleTextButton;
	MenuGUIElement_TextButton* CharacterMenuTextButton;
	MenuGUIElement_TextButton* CustomBattleTextButton;
};

#endif // _HUB_MENU_DIALOG_HANDLER_