////////////////////////////////////////////////////////////////////////////////
//
//	Name:			main.cpp
//
//	Description:	This file acts as the program entry point
//
//	Created:		December 7th, 2008
//
//	Last Modified:	January 31st, 2009
//
////////////////////////////////////////////////////////////////////////////////

#include "./Controllers/GameController.h"

int main(int argc, char *argv[])
{
	//  Create a GameController instance and allow it to run
	CGameController pGame;
	return pGame.Run();
}