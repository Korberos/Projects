#include "BattleController.h"

#include <SDL.h>
#include "RandomNumbers.h"
#include "DebugTools.h"
#include "InputSystem.h"
#include "Collision/LineToTriangle.h"
#include "ClientConnection.h"
#include "GameClient.h"
#include "RenderablePrimitives.h"


int BattleController::LocalPlayerID = -1;
int BattleController::SelectedUnitIndex = -1;


GameObject::GameObject() : 
	RenderableObject( NULL )
{}


BattleUnit::BattleUnit( const int owner_player, const std::string name, const int type, const int level, const int experience, const int health, const int attack, const int speed, int x, int y ) : 
	OwnerPlayer( owner_player ),
	UnitName( name ),
	UnitType( type ),
	UnitLevel( level ),
	UnitExperience( experience ),
	UnitMaxHealth( health ),
	UnitAttack( attack ),
	UnitSpeed( speed ),
	BattlePosition( x, y ),
	StatusFlags( 0 ),
	UnitHealth( UnitMaxHealth )
{}


BattleUnit::~BattleUnit()
{
	if ( RenderableObject != NULL )
	{
		delete RenderableObject;
		RenderableObject = NULL;
	}
}


void BattleUnit::Render( void ) const
{
	bool local = ( Get_Owner_Player() == BattleController::Get_Local_Player_ID() );
	bool selected = ( Get_Unit_Index() == BattleController::Get_Selected_Unit_Index() );

	static Vector3<float> colors[3] =
	{ 
		Vector3<float>( 0.25f, 1.00f, 0.25f ),
		Vector3<float>( 0.25f, 0.25f, 1.00f ), 
		Vector3<float>( 1.00f, 0.25f, 0.25f )
	};

	int color_index = 0;
	if ( local )		{ color_index = 1; }
	if ( selected )	{ color_index = 2; }

	//((RenderableDiamond*)RenderableObject)->Set_Color( colors[color_index] );
	((RenderableQuad3D*)RenderableObject)->Set_Color( colors[color_index] );
	RenderableObject->Render();
}


bool BattleUnit::Ray_Collision( const Vector3<float>& start, const Vector3<float>& end ) const
{
	Vector3<float> intersection;
	if ( LineToTriangle( intersection, BasicTriangle( Vector3<float>(Position.x, Position.y, Position.z), Vector3<float>(Position.x + 0.1f, Position.y + 0.3f, Position.z), Vector3<float>(Position.x, Position.y + 0.6f, Position.z) ), start, end ) ) { return true; }
	if ( LineToTriangle( intersection, BasicTriangle( Vector3<float>(Position.x - 0.1f, Position.y + 0.3f, Position.z), Vector3<float>(Position.x, Position.y, Position.z), Vector3<float>(Position.x + 0.1f, Position.y + 0.3f, Position.z) ), start, end ) ) { return true; }

	if ( LineToTriangle( intersection, BasicTriangle( Vector3<float>(Position.x, Position.y, Position.z), Vector3<float>(Position.x, Position.y + 0.3f, Position.z + 0.1f), Vector3<float>(Position.x, Position.y + 0.6f, Position.z) ), start, end ) ) { return true; }
	if ( LineToTriangle( intersection, BasicTriangle( Vector3<float>(Position.x, Position.y + 0.3f, Position.z - 0.1f), Vector3<float>(Position.x, Position.y, Position.z), Vector3<float>(Position.x, Position.y + 0.3f, Position.z + 0.1f) ), start, end ) ) { return true; }

	return false;
}


void BattleUnit::Recalculate_Position( void )
{
	float height = BattleController::Get_Instance()->Get_Height( BattlePosition.first, BattlePosition.second );
	Vector3<float> new_position = Vector3<float>( float(SPACE_BETWEEN_POINTS_Z / 2.0f + BattlePosition.first * SPACE_BETWEEN_POINTS_Z), height, float(SPACE_BETWEEN_POINTS_X / 2.0f + BattlePosition.second * SPACE_BETWEEN_POINTS_X) );
	Set_Position( new_position );
	RenderableObject->Set_Position( new_position );
}


void BattleController::New_Battle( void )
{
	UnitList.clear();
	UnitCount = 0;
	SelectedUnitIndex = -1;

	//for ( unsigned int i = 0; i < PlayerCount; ++i )
	//{
	//	Add_Player( int(i) );
	//}

	int socket = GameClient::Get_Instance()->Get_Server_Socket();
	ClientConnection::Request_Battle_Entry_Confirmation( socket );
}


bool BattleController::Input( void )
{
	if ( Is_In_Mode( MODE_UPDATE ) ) { return false; }
	if ( CurrentTurn != Get_Local_Player_ID() ) { return false; }

	InputSystem* input_system = InputSystem::Get_Instance();

	//  If the user hits TAB, cycle through selection of all active units. (SHIFT-TAB moves backwards)
	if ( input_system->Get_Key( SDLK_TAB ) == 1 )
	{
		bool backwards = ( input_system->Get_Key( SDLK_LSHIFT ) == 1 );
		SelectedUnitIndex += backwards ? -1 : 1;
		if ( SelectedUnitIndex >= UnitCount || SelectedUnitIndex < 0 )
		{
			SelectedUnitIndex = backwards ? UnitCount - 1 : 0;
		}
		return true;
	}

	if ( input_system->Get_Mouse_Button( 0 ) == 1 )
	{
		bool action_success = false;
		switch (CurrentMode)
		{
		case MODE_SELECT:
		{
			Select_Unit( SelectedUnitIndex, true );
			break;
		}

		case MODE_MOVE:
			Move_Unit();
			Deselect_Unit();
			break;

		case MODE_ATTACK:
			Attack_Unit();
			Deselect_Unit();
			break;
		}
	
		Set_Current_Mode( MODE_UPDATE );
		return true;
	}

	if ( input_system->Get_Mouse_Button( 1 ) == 1 )
	{
		Deselect_Unit();
		return true;
	}

	return false;
}


bool BattleController::Update( float time_slice )
{
	if ( Get_Current_Mode() == MODE_UPDATE )
	{
		Set_Current_Mode( MODE_SELECT );
		return true;
	}

	return false;
}


void BattleController::Render2D( void ) const 
{
	#if RAY_PICK_DEGBUGGING
		glDisable( GL_TEXTURE_2D );
		glColor3f( 0.2f, 0.2f, 1.0f );
		glPointSize( 3.0f );
		glBegin( GL_POINTS );
			glVertex2i( Selection2D[0], Selection2D[1] );
		glEnd();
	#endif
}


void BattleController::Render3D( void ) const
{
	int selection_iter = 0;
	for ( UnitListType::const_iterator unit_iter = UnitList.begin(); unit_iter != UnitList.end(); ++unit_iter, ++selection_iter )
	{
		(*unit_iter)->Render();
	}

	#if RAY_PICK_DEGBUGGING
		glColor3f( 1.0f, 0.0f, 0.0f );
		glLineWidth( 3.0f );
		glBegin( GL_LINES );
			glVertex3f( SelectionRayStart.x, SelectionRayStart.y, SelectionRayStart.z );
			glVertex3f( SelectionRayEnd.x, SelectionRayEnd.y, SelectionRayEnd.z );
		glEnd();

		glPointSize( 5.0f );
		glBegin( GL_POINTS );
			glVertex3f( SelectionRayEnd.x, SelectionRayEnd.y, SelectionRayEnd.z );
		glEnd();
	#endif
}


void BattleController::Load_World_Data( MapDataStruct& map_data )
{
	MapRows = map_data.Width;
	MapColumns = map_data.Height;

	if ( HeightList != NULL )
	{
		delete [] HeightList;
		HeightList = NULL;
	}

	HeightList = new float[ MapRows * MapColumns ];
	memcpy_s( HeightList, sizeof( float ) * MapRows * MapColumns, map_data.HeightList, sizeof( float ) * MapRows * MapColumns );
}


float BattleController::Get_Height( unsigned int row, unsigned int column )
{
	//  TODO:  Ensure that the unit messages that call this are not sent to the player while they are in the Lobby.
	//  Until that is fixed, just cancel if the HeightList is NULL, and then recalculate when the HeightList is loaded.
	if ( HeightList == NULL ) { return 0.0f; }

	FAIL_IF ( HeightList == NULL ) { return 0.0f; }
	FAIL_IF ( MapRows <= row ) { return 0.0f; }
	FAIL_IF ( MapColumns <= column ) { return 0.0f; }

	return HeightList[MapRows * column + row];
}


void BattleController::Add_Player( int player_id )
{
	//PlayerList.push_back( BattlePlayer( player_id ) );
}


void BattleController::Add_Unit( const int player_owner, const std::string name, const int type, const int level, const int experience, const int health, const int attack, const int speed, int x, int y )
{
	GameObject* new_object = new BattleUnit( player_owner, name, type, level, experience, health, attack, speed, x, y );
	//new_object->Set_Renderable( new RenderableDiamond( new_object->Get_Position(), Vector3<float>( 0.1f, 0.6f, 0.1f ), Vector3<float>( 0.25f, 0.25f, 1.00f ) ) );
	new_object->Set_Renderable( new RenderableQuad3D( new_object->Get_Position(), Vector3<float>( 0.0f, 1.0f, 0.5f ), Vector3<float>( 0.25f, 0.25f, 1.00f ) ) );
	((BattleUnit*)new_object)->Recalculate_Position();
	((BattleUnit*)new_object)->Set_Unit_Index( UnitCount );

	UnitList.push_back( new_object );
	++UnitCount;
}


void BattleController::Move_Unit( int unit_index, int x, int y )
{
	FAIL_IF ( unit_index < 0 ) { return; }
	FAIL_IF ( unit_index >= UnitCount ) { return; }

	BattleUnit* moving_unit = static_cast<BattleUnit*>(UnitList[unit_index]);
	FAIL_IF ( moving_unit == NULL && "Unit casting failed!" ) { return; }

	moving_unit->Set_X( x );
	moving_unit->Set_Y( y );
	moving_unit->Recalculate_Position();
	moving_unit->Set_Status( MOVE_EXHAUSTION, true );
}


void BattleController::Remove_Unit( int unit_index )
{
	FAIL_IF ( unit_index < 0 ) { return; }
	FAIL_IF ( unit_index >= UnitCount ) { return; }

	int i = 0;
	for ( UnitListType::iterator iter = UnitList.begin(); iter != UnitList.end(); ++iter, ++i )
	{
		if ( i == unit_index )
		{
			delete (*iter);
			UnitList.erase( iter );
			--UnitCount;
			return;
		}
	}
}


void BattleController::Clear_Units( void )
{
	//  Delete all objects in the Unit List, then clear the list of pointers
	for ( UnitListType::iterator iter = UnitList.begin(); iter != UnitList.end(); ++iter )
	{
		delete (*iter);
	}
	UnitList.clear();
}


bool BattleController::Select_Unit( int& selection, bool local_only )
{
	InputSystem* input_system = InputSystem::Get_Instance();

	int mouse_x = input_system->Get_Mouse_X();
	int mouse_y = input_system->Get_Mouse_Y();

	//  Request the picking ray from the camera, since it houses the matrix information
	#if RAY_PICK_DEGBUGGING
		Selection2D[0] = mouse_x;
		Selection2D[1] = mouse_y;
		Camera->Determine_Ray( mouse_x, mouse_y, 1024, 768, 60.0f, 1000.0f, SelectionRayStart, SelectionRayEnd );
	#else
		Vector3<float> ray_start;
		Vector3<float> ray_end;
		Camera->Determine_Ray( mouse_x, mouse_y, 1024, 768, 60.0f, 1000.0f, ray_start, ray_end );
	#endif

	std::vector< int > colliding_unit_index_list;
	for ( int i = 0; i < UnitCount; ++i )
	{
		bool collision = false;
		#if RAY_PICK_DEGBUGGING
			collision = ((BattleUnit*)UnitList[i]).Ray_Collision( SelectionRayStart, SelectionRayEnd );
		#else
			collision = ((BattleUnit*)UnitList[i])->Ray_Collision( ray_start, ray_end );
		#endif

		if ( collision )
		{
			colliding_unit_index_list.push_back( i );
		}
	}

	//  If our list is empty, nothing happens.
	if ( colliding_unit_index_list.size() == 0 )
	{
		return false;
	}

	//  If our list has one unit in it, select that unit
	if ( colliding_unit_index_list.size() == 1 ) 
	{
		if ( local_only && ((BattleUnit*)UnitList[colliding_unit_index_list[0]])->Get_Owner_Player() != Get_Local_Player_ID() ) { return false; }
		selection = colliding_unit_index_list[0];
	}
	else
	{
		//  If our list has multiple units, find the closest unit to the camera and eliminate the rest
		for ( int i = 0; i < int(colliding_unit_index_list.size()); ++i )
		{
			if ( local_only && ((BattleUnit*)UnitList[i])->Get_Owner_Player() != Get_Local_Player_ID() ) { continue; }

			if ( selection == -1 )
			{
				selection = i;
				continue;
			}
			else
			{
				if ( ( UnitList[i]->Get_Position() - Camera->GetOrigin() ).Magnitude() < ( UnitList[selection]->Get_Position() - Camera->GetOrigin() ).Magnitude() )
				{
					selection = i;
				}
			}
		}
	}

	//  TODO:  Change anything on the unit that needs to be changed

	return ( selection != -1 );
}


bool BattleController::Move_Unit( void )
{
	//  If you are not the owner, you can't move
	if ( ((BattleUnit*)UnitList[SelectedUnitIndex])->Get_Owner_Player() != Get_Local_Player_ID() ) { return false; }

	//  If the unit is exhausted, don't bother moving
	if ( ((BattleUnit*)UnitList[SelectedUnitIndex])->Get_Status( MOVE_EXHAUSTION ) ) { return false; }

	InputSystem* input_system = InputSystem::Get_Instance();

	int mouse_x = input_system->Get_Mouse_X();
	int mouse_y = input_system->Get_Mouse_Y();

	//  Request the picking ray from the camera, since it houses the matrix information
	#if RAY_PICK_DEGBUGGING
		Selection2D[0] = mouse_x;
		Selection2D[1] = mouse_y;
		Camera->Determine_Ray( mouse_x, mouse_y, 1024, 768, 60.0f, 1000.0f, SelectionRayStart, SelectionRayEnd );
	#else
		Vector3<float> ray_start;
		Vector3<float> ray_end;
		Camera->Determine_Ray( mouse_x, mouse_y, 1024, 768, 60.0f, 1000.0f, ray_start, ray_end );
	#endif

	std::pair< int, int > colliding_battle_position;
	if ( WorldController::Get_Instance()->Colliding_Square( ray_start, ray_end, colliding_battle_position ) == false )
	{
		return false;
	}

	//  If we made it this far, we have a colliding square to move to. Send the request to the server
	int socket = GameClient::Get_Instance()->Get_Server_Socket();

	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_OBJECT_MOVE_REQUEST, BUFFER_PACKET );

	writeint( SelectedUnitIndex, BUFFER_PACKET );
	writeint( colliding_battle_position.first, BUFFER_PACKET );
	writeint( colliding_battle_position.second, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );

	Set_Current_Mode( MODE_UPDATE );
	return true;
}


bool BattleController::Attack_Unit( void )
{
	BattleUnit* selected_unit = static_cast<BattleUnit*>( UnitList[SelectedUnitIndex] );
	FAIL_IF ( selected_unit == NULL && "Selected unit casting failed." ) { return false; }

	//  If you are not the owner, you can't attack
	if ( selected_unit->Get_Owner_Player() != Get_Local_Player_ID() ) { return false; }

	//  If the unit is exhausted, don't bother moving
	if ( selected_unit->Get_Status( ATTACK_EXHAUSTION ) ) { return false; }

	int selected_target = 0;
	Select_Unit( selected_target , false );
	if ( selected_target == -1 ) { return false; }
	const BattleUnit* target_unit = static_cast<const BattleUnit*>( UnitList[selected_target] );
	FAIL_IF ( target_unit == NULL && "Target unit casting failed." ) { return false; }

	//  If you are the owner of the target, you can't attack
	if ( target_unit->Get_Owner_Player() == Get_Local_Player_ID() ) { return false; }

	//  TODO: Check that the selected_target is next to the selected unit. The server will make the same check.
	//  TODO:  Check the distance against the range of the unit once range is a stat
	int x_distance = abs( selected_unit->Get_X() - target_unit->Get_X() );
	int y_distance = abs( selected_unit->Get_Y() - target_unit->Get_Y() );
	if ( x_distance + y_distance > 1 ) { return false; }

	//  TODO: Only do this when a message comes back CONFIRMING an attack
	selected_unit->Set_Status( ATTACK_EXHAUSTION, true );

	//  If we made it this far, we have a colliding object to attack. Send the request to the server
	int socket = GameClient::Get_Instance()->Get_Server_Socket();

	//  Send a request message to the server to attack the selected_target with the selected unit.
	clearbuffer( BUFFER_PACKET );
	writechar( CLIENT_TO_SERVER_MESSAGE_ID_BATTLE_OBJECT_ATTACK_REQUEST, BUFFER_PACKET );

	writeint( SelectedUnitIndex, BUFFER_PACKET );
	writeint( selected_target, BUFFER_PACKET );

	clearbuffer( BUFFER_PACKET_WITH_SIZE );
	writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	sendmessage( socket, SERVER_IP, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );

	return true;
}


void BattleController::Remove_Exhaustion( void )
{
	for ( UnitListType::iterator iter = UnitList.begin(); iter != UnitList.end(); ++iter )
	{
		if ( ((BattleUnit*)(*iter))->Get_Owner_Player() == Get_Current_Turn() )
		{
			for ( int i = 0; i < EXHAUSTION_TYPE_COUNT; ++i )
			{
				((BattleUnit*)(*iter))->Set_Status( i, false );
			}
		}
	}
}