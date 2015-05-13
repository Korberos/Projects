#include "GameController.h"

int main(int argc, char **argv)
{
	//Create a GameController instance
	CGameController* pGame = new CGameController;

	//If we can initialize the lesson
	if(pGame->Initialize())
	{
		//Then we run it
		pGame->Run();
	}
	else 
	{
		// Output an error message
		std::cout << "Lesson initializiation failed!" << std::endl;
	}
	
	//If we can't initialize, or the lesson has quit we delete the instance
	delete pGame;
		
	return 0;
};