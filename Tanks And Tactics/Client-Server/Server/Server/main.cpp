#pragma comment(lib, "sdlmain.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <conio.h>
#include <iostream>
#include "GameServer.h"

void main( void )
{
	//  Create a game server
	GameServer* game_server = GameServer::Get_Instance();

	//  Initialize the server
	if ( !game_server->Initialize() )
	{
		assert("The server did not properly initialize." && false);
		return;
	}

	std::cout << "Server initialized. Press ESCAPE at any time to exit the program." << std::endl << std::endl;

	//  Main loop
	while ( true )
	{
		//  If the ESCAPE key is hit, exit the loop
		if ( _kbhit() && _getch() == 27 ) break;

		game_server->Service();
	}
}