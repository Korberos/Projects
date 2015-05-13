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

#include "./Global.h"
#include "./Window.h"
#include "./Controllers/GameController.h"

int main(int argc, char *argv[])
{
	//  Define the window. If the window definition fails, return a failure.
	CWindow Window;
	if (!Window.DefineWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Tanks and Tactics - Tech Demo (build 0004)")) return -1;

	//  Create a GameController instance and allow it to run
	CGameController pGame;
	pGame.SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	return pGame.Run();
}