#ifndef _BATTLE_CONTROLLER_H_
#define _BATTLE_CONTROLLER_H_

#include <algorithm>
using namespace std;

#include "WorldController.h"
#include "OpenGLCamera.h"
#include "./../../ClientServerCommon.h"
#include "RenderablePrimitives.h"

#define RAY_PICK_DEGBUGGING 0

class Renderable;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

	virtual void Render( void ) const = 0;

	//  Accessors
	const Vector3<float>& Get_Position( void )		const { return Position; }
	const Renderable* Get_Renderable( void )			const { return RenderableObject; }

	//  Modifiers
	void Set_Position( Vector3<float>& position )				{ Position = position; }
	void Set_Renderable( Renderable* renderable )				{ RenderableObject = renderable; }

protected:
	Renderable* RenderableObject;
	Vector3<float> Position;
};

class BattleUnit : public GameObject
{
public:
	BattleUnit( const int owner_player, const std::string name, const int type, const int level, const int experience, const int health, const int attack, const int speed, int x, int y );
	~BattleUnit();

	void Render( void ) const;

	bool Ray_Collision( const Vector3<float>& start, const Vector3<float>& end ) const;
	void Recalculate_Position ( void );

	//  Accessors
	const int Get_Owner_Player( void )					const	{ return OwnerPlayer; }
	const std::string& Get_Unit_Name( void )			const	{ return UnitName; }
	const int Get_Unit_Type( void )						const { return UnitType; }
	const int Get_Unit_Level( void )						const { return UnitLevel; }
	const int Get_Unit_Experience( void )				const { return UnitExperience; }
	const int Get_Unit_Max_Health( void )				const { return UnitMaxHealth; }
	const int Get_Unit_Attack( void )					const { return UnitAttack; }
	const int Get_Unit_Speed( void )						const { return UnitSpeed; }

	const int Get_Unit_Index( void )						const { return UnitIndex; }
	const int Get_X( void )									const	{ return BattlePosition.first; }
	const int Get_Y( void )									const	{ return BattlePosition.second; }
	bool Get_Status( int flag )							const { return ( ( StatusFlags & flag ) != 0 ); }
	int Get_Status_Flags( void )							const { return StatusFlags; }
	const int Get_Unit_Health( void )					const { return UnitHealth; }

	//  Modifiers
	void Set_Owner_Player( const int owner_player )			{ OwnerPlayer = owner_player; }
	void Set_Unit_Name( const std::string name )				{ UnitName = name; }
	void Set_Unit_Type( const int type )						{ UnitType = type; }
	void Set_Unit_Level( const int level )						{ UnitLevel = level; }
	void Set_Unit_Experience( const int experience )		{ UnitExperience = experience; }
	void Set_Unit_Max_Health( const int max_health )		{ UnitMaxHealth = max_health; }
	void Set_Unit_Attack( const int attack )					{ UnitAttack = attack; }
	void Set_Unit_Speed( const int speed )						{ UnitSpeed = speed; }

	void Set_Unit_Index( const int index )						{ UnitIndex = index; }
	void Set_X( const int x )										{ BattlePosition.first = x; }
	void Set_Y( const int y )										{ BattlePosition.second = y; }
	void Set_Status( int flag, bool value )					{ if ( value ) { StatusFlags |= flag; } else { StatusFlags &= ~flag; } }
	void Set_Unit_Health( const int health )					{ UnitHealth = min( max( health, 0 ), UnitMaxHealth ); }


private:
	int OwnerPlayer;
	std::string UnitName;
	int UnitType;
	int UnitLevel;
	int UnitExperience;
	int UnitMaxHealth;
	int UnitAttack;
	int UnitSpeed;

	int UnitIndex;
	std::pair< int, int > BattlePosition;
	int StatusFlags;
	int UnitHealth;
};

class BattlePlayer
{
public:
	BattlePlayer( int player_id ) : 
		PlayerID( player_id ),
		PlayerName( "UNDEFINED NAME" )
	{}

	int Get_Player_ID( void )							const { return PlayerID; }
	const std::string& Get_Player_Name( void )	const { return PlayerName; }
	void Set_Player_Name( std::string& name )				{ PlayerName = name; }

private:
	int PlayerID;
	std::string PlayerName;
};

class BattleController
{
public:
	static BattleController* Get_Instance( void )
	{
		static BattleController INSTANCE;
		return &INSTANCE;
	}

	enum CurrentModeType { MODE_SELECT, MODE_MOVE, MODE_ATTACK, MODE_UPDATE, MODE_TYPE_COUNT  };

	void New_Battle( void );

	bool Input( void );
	bool Update( float time_slice );
	void Render2D( void ) const;
	void Render3D( void ) const;

	void Load_World_Data( MapDataStruct& map_data );
	float Get_Height( unsigned int row, unsigned int column );

	CurrentModeType Get_Current_Mode( void )						const { return CurrentMode; }
	bool Is_In_Mode( CurrentModeType mode )						const { return ( CurrentMode == mode ); }

	const GameObject* Get_Selected_Unit( void )					const { return ( SelectedUnitIndex == -1 ) ? NULL : UnitList[SelectedUnitIndex]; }
	void Deselect_Unit( void )													{ SelectedUnitIndex = -1; }
	void Set_Current_Mode( CurrentModeType current_mode )				{ CurrentMode = current_mode; }
	void Set_Camera( COpenGLFrame* camera )								{ Camera = camera; }
	
	static const int Get_Selected_Unit_Index( void )					{ return SelectedUnitIndex; }

	static void Set_Local_Player_ID( int local_player_id )			{ LocalPlayerID = local_player_id; }
	static int Get_Local_Player_ID( void )									{ return LocalPlayerID; }
	
	//void Set_Player_Name( int index, std::string& name )				{ PlayerList[index].Set_Player_Name( name ); }

	void Set_Current_Turn( int turn )										{ CurrentTurn = turn; Remove_Exhaustion(); }
	int Get_Current_Turn( void )										const { return CurrentTurn; }

	unsigned int Get_Player_Count( void )							const { return PlayerCount; }
	//const std::string& Get_Player_Name( int index )				const { return PlayerList[index].Get_Player_Name(); }

	void Add_Player( int player_id );
	void Add_Unit( const int player_owner, const std::string name, const int type = 0, const int level = 0, const int experience = 0, const int health = 0, const int attack = 0, const int speed = 0, int x = -1, int y = -1 );
	void Move_Unit( int unit_index, int x, int y );
	void Remove_Unit( int unit_index );
	void Clear_Units( void );

private:
	BattleController() : 
	   PlayerCount( 2 ),
	   PlayerTurn( 1 ),
	   UnitCount( -1 ),
	   MapRows( 0 ),
	   MapColumns( 0 ),
		HeightList( NULL ),
		CurrentTurn( 0 ),
		Camera( NULL ),
		CurrentMode( MODE_SELECT )
	{
		#if RAY_PICK_DEGBUGGING
			SelectionRayStart = Vector3<float>( 0.0f, 0.0f, 0.0f );
			SelectionRayEnd = Vector3<float>( 0.0f, 0.0f, 0.0f );
			Selection2D[0] = 0;
			Selection2D[1] = 0;
		#endif
	}
	~BattleController()
	{
		Clear_Units();

		if ( HeightList != NULL )
		{
			delete [] HeightList;
			HeightList = NULL;
		}
	}

	bool Select_Unit( int& selection, bool local_only );
	bool Move_Unit( void );
	bool Attack_Unit( void );
	void Remove_Exhaustion( void );

	unsigned int PlayerCount;
	unsigned int PlayerTurn;
	int UnitCount;

	unsigned int MapRows;
	unsigned int MapColumns;
	float* HeightList;

	typedef std::vector< BattlePlayer > PlayerListType;
	//PlayerListType PlayerList;
	int CurrentTurn;

	typedef std::vector< GameObject* > UnitListType;
	UnitListType UnitList;

	static int SelectedUnitIndex;
	static int LocalPlayerID;
	
	COpenGLFrame* Camera;
	
	CurrentModeType CurrentMode;

	#if RAY_PICK_DEGBUGGING
		Vector3<float> SelectionRayStart;
		Vector3<float> SelectionRayEnd;
		int Selection2D[2];
	#endif
};

#endif