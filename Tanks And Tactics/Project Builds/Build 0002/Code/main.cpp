#include "./Controllers/GameController.h"

#include <stdlib.h>
#include <ctime>

int main(int argc, char *argv[])
{
	// Seed the random number generator
	srand((unsigned int)time(0));

	//  Create a GameController instance and allow it to run
	CGameController pGame;
	return pGame.Run();
};