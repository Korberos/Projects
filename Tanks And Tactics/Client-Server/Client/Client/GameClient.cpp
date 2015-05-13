#include "GameClient.h"
#include "ClientConnection.h"
#include "MainMenuDialogHandler.h"

GameClient* GameClient::Get_Instance( void )
{
	static GameClient INSTANCE;
	return &INSTANCE;
}


bool GameClient::Initialize( void )
{
	//  Attempt to connect to the server
	FAIL_IF ( !ClientConnection::Connect_To_Server( ServerSocket, SERVER_IP, SERVER_PORT ) ) { return false; }

	return true;
}


void GameClient::Input( float time_slice )
{
	for ( FloatFunctionListIter iter = InputFunctions.begin(); iter != InputFunctions.end(); ++iter )
	{
		(*iter)( time_slice );
	}
}


void GameClient::Update( float time_slice )
{
	ReceiveMessages();

	for ( FloatFunctionListIter iter = UpdateFunctions.begin(); iter != UpdateFunctions.end(); ++iter )
	{
		(*iter)( time_slice);
	}
}


void GameClient::Render3D( void )
{
	for ( VoidFunctionListIter iter = Render3DFunctions.begin(); iter != Render3DFunctions.end(); ++iter )
	{
		(*iter)();
	}
}


void GameClient::Render2D( void )
{
	for ( VoidFunctionListIter iter = Render2DFunctions.begin(); iter != Render2DFunctions.end(); ++iter )
	{
		(*iter)();
	}
}


void GameClient::ReceiveMessages( void )
{
	if ( ServerSocket == -1 ) return;

	int buffer_size = buffsize( MessageBuffer );
	int length = buffer_size > 4 ? readint( MessageBuffer, true ) - buffer_size : 0;	
	int bytes_received = receivemessage( ServerSocket, 0, BUFFER_RECEIVE, length );
	if ( bytes_received == 0 )
	{
		closesocket( ServerSocket );
		ServerSocket = -1;
		return;
	}
	else if ( bytes_received < 0 )
	{
		return;
	}

	//  If we've gotten this far, we have received data
	copybuffer( MessageBuffer, BUFFER_RECEIVE );

	//  If we haven't received enough data to even get the size, ignore the buffer for now
	if ( buffsize( MessageBuffer ) < sizeof(int) )
	{
		return;
	}

	int packet_size = readint( MessageBuffer, true );
	if ( buffsize( MessageBuffer ) < packet_size )
	{
		return;
	}
	setpos( getpos( true, MessageBuffer ) + sizeof(packet_size), MessageBuffer );

	char message_id = readchar( MessageBuffer );
	DialogHandler* active_dialog = DialogController::Get_Instance()->Get_Active_Dialog();
	ENFORCED_IF ( active_dialog && "Improper message id received. No dialog to escalate to." )
	{
		bool escalated = active_dialog->Escalate_Message( message_id, MessageBuffer ) && "Improper message id received.";
		ASSERT( escalated );
	}

	clearbuffer( MessageBuffer );
}


void GameClient::Remove_Input_Function( FloatFunction void_function )
{
	for ( FloatFunctionListIter iter = InputFunctions.begin(); iter != InputFunctions.end(); ++iter )
	{
		if ( (*iter) == void_function )
		{
			InputFunctions.erase( iter );
			return;
		}
	}
}

void GameClient::Remove_Update_Function( FloatFunction float_function )
{
	for ( FloatFunctionListIter iter = UpdateFunctions.begin(); iter != UpdateFunctions.end(); ++iter )
	{
		if ( (*iter) == float_function )
		{
			UpdateFunctions.erase( iter );
			return;
		}
	}
}

void GameClient::Remove_Render_3D_Function( VoidFunction void_function )
{
	for ( VoidFunctionListIter iter = Render3DFunctions.begin(); iter != Render3DFunctions.end(); ++iter )
	{
		if ( (*iter) == void_function )
		{
			Render3DFunctions.erase( iter );
			return;
		}
	}
}

void GameClient::Remove_Render_2D_Function( VoidFunction void_function )
{
	for ( VoidFunctionListIter iter = Render2DFunctions.begin(); iter != Render2DFunctions.end(); ++iter )
	{
		if ( (*iter) == void_function )
		{
			Render2DFunctions.erase( iter );
			return;
		}
	}
}