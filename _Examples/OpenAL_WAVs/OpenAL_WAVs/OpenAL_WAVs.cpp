#include "SimpleAudioLib\CoreSystem.h"

#pragma comment(lib, "OpenAL\\OpenAL32.lib")

#include <conio.h>
#include <time.h>
time_t currTime;
time_t lastTime;
const time_t delayBetweenCoins = 1;

int main()
{
	//  Grab the first time variable and seed the random number generator (for selecting coin sounds)
	lastTime = time(0);
	srand((unsigned int)(lastTime));

	// before you can use this library, you have to initialize it
	SimpleAudioLib::CoreSystem& system = SimpleAudioLib::CoreSystem::getInstance();

	system.initWithDefaultDevice();

	// load audio file in wave format
	SimpleAudioLib::AudioEntity* sound[2];
	sound[0] = system.createAudioEntityFromFile("CoinPickup1.wav");
	sound[1] = system.createAudioEntityFromFile("CoinPickup2.wav");

	while (true)
	{
		if (_kbhit() == 0) continue;
		char keyboardHit = _getch();

		if (keyboardHit == 32) // SPACE to play coin sounds every X seconds (delayBetweenCoins)
		{
			fflush(stdin);

			currTime = time(0);
			if (currTime - lastTime >= delayBetweenCoins)
			{
				int rando = rand() % 2;
				sound[rando]->rewind();
				sound[rando]->play();
				lastTime = currTime;
			}
		}
		else if (keyboardHit == 27) break; // ESCAPE to exit
	}

	// clean up
	delete sound[0];
	sound[0] = NULL;
	delete sound[1];
	sound[1] = NULL;

	SimpleAudioLib::CoreSystem::release();

	return 0;
}