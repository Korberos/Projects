#include "SoundWrapper.h"

#include <time.h>
time_t currTime;
time_t lastTime;
const time_t delayBetweenCoins = 1;

int main()
{
	//  Grab the first time variable and seed the random number generator (for selecting coin sounds)
	lastTime = time(0);
	srand((unsigned int)(lastTime));

	//  Grab the singleton instance of the sound wrapper
	SoundWrapper& soundWrapper = SoundWrapper::GetInstance();

	//  Initialize the sound wrapper (and fail out if that doesn't succeed)
	if (soundWrapper.Initialize() == false) return 0;

	//  Load and play background music
	soundWrapper.playSoundFile(soundWrapper.loadSoundFile("BackgroundMusic.ogg"));

	//  Load the two coin sounds
	int coinSound;
	char* coinFiles[2] = { "CoinPickup1.ogg", "CoinPickup2.ogg" };

	while (true)
	{
		soundWrapper.Update();

		if (_kbhit() && (_getch() == 32))
		{
			fflush(stdin);

			currTime = time(0);
			if (currTime - lastTime >= delayBetweenCoins)
			{
				int rando = rand() % 2;
				coinSound = soundWrapper.loadSoundFile(coinFiles[rando]);
				soundWrapper.playSoundFile(coinSound);
				lastTime = currTime;
			}
		}
	}

	// Shutdown VorbisFile Library
	soundWrapper.Shutdown();

	return 0;
}