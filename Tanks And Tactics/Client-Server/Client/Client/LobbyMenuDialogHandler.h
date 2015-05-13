#ifndef _LOBBY_MENU_DIALOG_HANDLER_
#define _LOBBY_MENU_DIALOG_HANDLER_

#include "DialogController.h"

#include "../../ClientServerCommon.h"

class MenuGUIElement_TextBlock;
class MenuGUIElement_EditBox;
class MenuGUIElement_ListBox;
class MenuGUIElement_TextButton;
class MenuGUIElement_Frame;
class MenuGUIElement_DropDown;
class MenuGUIElement_Texture;
struct Font;

class LobbyMenuDialogHandler : public DialogHandler
{
	typedef DialogHandler BASECLASS;
public:
	static void Set_As_Active_Dialog( void )
	{
		DialogHandler* dialog_handler = new LobbyMenuDialogHandler;
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

	static void Set_Lobby_Information( const std::string& lobby_name, const std::string& map_name, int map_index, BattleType battle_type, PlayersType players_index );
	static void Process_Lobby_Player_Joined_Message( int message_buffer_id );
	static void Process_Lobby_Player_List_Message( int message_buffer_id );
	static void Process_Lobby_New_Leader_Message( int message_buffer_id );
	
	static void Set_Custom( bool custom ) { Custom = custom; }
	
	typedef std::pair< unsigned char, std::string > PlayerType;
	

private:
	LobbyMenuDialogHandler() :
		CustomLobbyFrame( NULL ),
		BattleNameEditBox( NULL ),
		BattleTypeDropDown( NULL ),
		MapDropDown( NULL ),
		PlayersTypeDropDown( NULL ),
		LobbyInfoFrame( NULL ),
		LobbyNameTextBlock( NULL ),
		MapNameTextBlock( NULL ),
		BattleTypeTextBlock( NULL ),
		PlayersTypeTextBlock( NULL ),
		CustomizeLobbyButton( NULL ),
		PlayerListBox( NULL ),
		LobbyLeaderTexture( NULL ),
		ChatInputEditBox( NULL ),
		EnterTextButton( NULL ),
		StartGameButton( NULL ),
		ChatFont( NULL ),
		StringDisplayCount( 0 )
	{}

	virtual ~LobbyMenuDialogHandler() {}

	void Load_Hub_Menu( void );
	void Load_Game_Menu( void );

	void Clear_Lobby( void );

	void Customization_Menu( void );
	void Fill_Lobby_Information( void );
	void Fill_Player_List( void );
	void Recustomize_Lobby( void );

	void Request_Map_List( void );
	void Process_Map_List_Response( int message_buffer_id );
	void Request_Custom_Lobby( void );
	void Process_Custom_Lobby_Response( int message_buffer_id );
	void Cancel_Custom_Lobby( void );

	void Request_Add_Chat_Line( void );
	void Request_Start_Game( void );
	void Process_Lobby_Chat_Message_Response( int message_buffer_id );
	void Process_Lobby_Information( int message_buffer_id );
	void Process_Battle_Summons( int message_buffer_id );
	void Process_Lobby_Start_Battle_Response( int message_buffer_id );
	void Process_Map_Data_Response( int message_buffer_id );
	void Add_Chat_String( const char* text );
	static void Add_Lobby_Player( const int player_index, const PlayerType player );

	MenuGUIElement_Frame* CustomLobbyFrame;
	MenuGUIElement_EditBox* BattleNameEditBox;
	MenuGUIElement_DropDown* BattleTypeDropDown;
	MenuGUIElement_DropDown* MapDropDown;
	MenuGUIElement_DropDown* PlayersTypeDropDown;

	MenuGUIElement_Frame* LobbyInfoFrame;
	MenuGUIElement_TextBlock* LobbyNameTextBlock;
	MenuGUIElement_TextBlock* MapNameTextBlock;
	MenuGUIElement_TextBlock* BattleTypeTextBlock;
	MenuGUIElement_TextBlock* PlayersTypeTextBlock;
	MenuGUIElement_TextButton* CustomizeLobbyButton;

	MenuGUIElement_ListBox* PlayerListBox;
	MenuGUIElement_Texture* LobbyLeaderTexture;
	MenuGUIElement_EditBox* ChatInputEditBox;
	MenuGUIElement_TextButton* EnterTextButton;
	MenuGUIElement_TextButton* StartGameButton;
	
	Font* ChatFont;
	unsigned int StringDisplayCount;
	std::vector< std::string > ChatStrings;

	// Lobby Information
	static std::string LobbyName;
	static std::string MapName;
	static int MapIndex;
	static BattleType BattleTypeIndex;
	static PlayersType PlayersTypeIndex;

	static bool Custom;
	static PlayerType PlayerList[MAX_INSTANCE_PLAYERS];
};

#endif // _LOBBY_MENU_DIALOG_HANDLER_