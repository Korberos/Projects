#include "SoundWrapper.h"

#include <time.h>
time_t currTime;
time_t lastTime;
const time_t delayBetweenActions = 1; // If you hold SPACE, every 1 second a new sound will play

int main()
{
	//  Grab the first time variable and seed the random number generator (for selecting coin sounds)
	lastTime = time(0);
	srand((unsigned int)(lastTime));

	//  Grab the singleton instance of the sound wrapper
	SoundWrapper& soundWrapper = SoundWrapper::GetInstance();

	//  Initialize the sound wrapper (and fail out if that doesn't succeed)
	if (soundWrapper.Initialize() == false) return 0;

	//  Load a WAV file
	int CoinSound[4];
	time_t CountSoundsLastTime[4] = { lastTime, lastTime, lastTime, lastTime };
	CoinSound[0] = soundWrapper.loadSoundFile("CoinPickup1.ogg", SOUNDFILETYPE_OGG);
	CoinSound[1] = soundWrapper.loadSoundFile("CoinPickup2.ogg", SOUNDFILETYPE_OGG);
	CoinSound[2] = soundWrapper.loadSoundFile("CoinPickup1.wav", SOUNDFILETYPE_WAV);
	CoinSound[3] = soundWrapper.loadSoundFile("CoinPickup2.wav", SOUNDFILETYPE_WAV);

	//  Load and play background music
	int BackgroundMusic;
	soundWrapper.playSoundFile(BackgroundMusic = soundWrapper.loadSoundFile("BackgroundMusic.ogg", SOUNDFILETYPE_OGG));

	//  Listen for input and play sounds accordingly
	while (true)
	{
		soundWrapper.Update();

		if (_kbhit())
		{
			char keyboardCheck = _getch();
			fflush(stdin);
			if (keyboardCheck >= '1' && keyboardCheck <= '4')
			{
				currTime = time(0);
				int index = keyboardCheck - '1';
				if (currTime - CountSoundsLastTime[index] >= delayBetweenActions)
				{
					soundWrapper.playSoundFile(CoinSound[index]);
					CountSoundsLastTime[index] = time(0);
				}
			}
		}
	}

	// Shutdown the sound wrapper (unloading all sounds)
	soundWrapper.Shutdown();

	return 0;
}