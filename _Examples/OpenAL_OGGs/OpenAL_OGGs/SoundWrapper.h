/*
* Copyright (c) 2006, Creative Labs Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided
* that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and
* 	     the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
* 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
* 	     promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "OpenAL\Framework.h"
#include "OpenAL\Vorbis\vorbisfile.h"

#pragma comment(lib, "OpenAL\\OpenAL32.lib")

#include <map>

#define NUMBUFFERS              (4)
#define	SERVICE_UPDATE_PERIOD	(20)

// Ogg Voribis DLL Handle
HINSTANCE g_hVorbisFileDLL = NULL;

// Background Functions (at bottom of file)
unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
void Swap(short &s1, short &s2);
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
int ov_close_func(void *datasource);
long ov_tell_func(void *datasource);

// Function pointers
typedef int(*LPOVCLEAR)(OggVorbis_File *vf);
typedef long(*LPOVREAD)(OggVorbis_File *vf, char *buffer, int length, int bigendianp, int word, int sgned, int *bitstream);
typedef ogg_int64_t(*LPOVPCMTOTAL)(OggVorbis_File *vf, int i);
typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf, int link);
typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf, int link);
typedef int(*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf, char *initial, long ibytes, ov_callbacks callbacks);

// Variables
ov_callbacks		soundCallbacks;
LPOVCLEAR			fn_ov_clear = NULL;
LPOVREAD			fn_ov_read = NULL;
LPOVPCMTOTAL		fn_ov_pcm_total = NULL;
LPOVINFO			fn_ov_info = NULL;
LPOVCOMMENT			fn_ov_comment = NULL;
LPOVOPENCALLBACKS	fn_ov_open_callbacks = NULL;

class SoundWrapper
{
public:
	static SoundWrapper& GetInstance() { static SoundWrapper INSTANCE; return INSTANCE; }

	bool Initialize();
	int loadSoundFile(const char* soundFileName);
	bool playSoundFile(int soundIndex);
	bool pauseSoundFile(int soundIndex);
	bool stopSoundFile(int soundIndex);
	bool isPlaying();
	void Update();
	void Shutdown();

private:
	struct SoundData
	{
	public:
		enum SoundState
		{
			SOUNDSTATE_INVALID,
			SOUNDSTATE_UNPLAYED,
			SOUNDSTATE_LOADED,
			SOUNDSTATE_PLAYING
		};

		FILE*			m_SoundFile;
		OggVorbis_File	m_OggFile;
		vorbis_info*	m_VorbisInfo;
		unsigned long	m_Frequency;
		unsigned long	m_Channels;
		unsigned long	m_BufferSize;
		unsigned long	m_Format;
		char*			m_DecodeBuffer;
		ALuint		    m_SoundBuffers[NUMBUFFERS];
		ALuint		    m_SoundSource;
		ALuint			m_BufferID;
		ALint			iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
		unsigned long	ulBytesWritten;
		SoundState		m_SoundState;

		SoundData(FILE* soundFile) :
			m_SoundFile(soundFile),
			m_Frequency(0),
			m_Channels(0),
			m_BufferSize(0),
			m_Format(0),
			m_SoundState(SOUNDSTATE_INVALID)
		{}
	};

	SoundWrapper();
	~SoundWrapper();

	void continueAllSounds();
	void unloadAllSoundFiles();
	void determineFirstIndex();

	bool m_bVorbisInitialized;
	std::map<int, SoundData*> soundDataList;
	int m_SoundPlayingCount;
	int m_FirstIndex;
};

bool SoundWrapper::Initialize()
{
	// Initialize OpenAL Framework
	ALFWInit();

	// Initialize OggVorbis libary, loading Vorbis DLLs (VorbisFile.dll will load ogg.dll and vorbis.dll)
	if (!m_bVorbisInitialized)
	{
		g_hVorbisFileDLL = LoadLibrary("vorbisfile.dll");
		if (g_hVorbisFileDLL)
		{
			fn_ov_clear = (LPOVCLEAR)GetProcAddress(g_hVorbisFileDLL, "ov_clear");
			fn_ov_read = (LPOVREAD)GetProcAddress(g_hVorbisFileDLL, "ov_read");
			fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(g_hVorbisFileDLL, "ov_pcm_total");
			fn_ov_info = (LPOVINFO)GetProcAddress(g_hVorbisFileDLL, "ov_info");
			fn_ov_comment = (LPOVCOMMENT)GetProcAddress(g_hVorbisFileDLL, "ov_comment");
			fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(g_hVorbisFileDLL, "ov_open_callbacks");

			if (fn_ov_clear && fn_ov_read && fn_ov_pcm_total && fn_ov_info && fn_ov_comment && fn_ov_open_callbacks)
			{
				m_bVorbisInitialized = true;
			}
		}
	}

	//  If we failed to initialize Vorbis, shut down OpenAL and return a failure
	if (!m_bVorbisInitialized)
	{
		ALFWprintf("Failed to find OggVorbis DLLs (vorbisfile.dll, ogg.dll, or vorbis.dll)\n");
		ALFWShutdown();
		return false;
	}

	// Initialise OpenAL
	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		return false;
	}

	//  Set the callback interaction functions
	soundCallbacks.read_func = ov_read_func;
	soundCallbacks.seek_func = ov_seek_func;
	soundCallbacks.close_func = ov_close_func;
	soundCallbacks.tell_func = ov_tell_func;

	return true;
}

int SoundWrapper::loadSoundFile(const char* soundFileName)
{
	// Open the OggVorbis file
	FILE *pOggVorbisFile;
	fopen_s(&pOggVorbisFile, soundFileName, "rb");
	if (!pOggVorbisFile)
	{
		ALFWprintf("Could not find %s\n", soundFileName);
		return -1;
	}

	//  Add the new SoundData to the sound data list, then determine a new first index
	SoundData* newSoundData = new SoundData(pOggVorbisFile);
	soundDataList[m_FirstIndex] = newSoundData;
	determineFirstIndex();

	if (fn_ov_open_callbacks(newSoundData->m_SoundFile, &newSoundData->m_OggFile, NULL, 0, soundCallbacks) == 0)
	{
		// Get some information about the file (Channels, Format, and Frequency)
		newSoundData->m_VorbisInfo = fn_ov_info(&newSoundData->m_OggFile, -1);
		if (newSoundData->m_VorbisInfo)
		{
			newSoundData->m_Frequency = newSoundData->m_VorbisInfo->rate;
			newSoundData->m_Channels = newSoundData->m_VorbisInfo->channels;

			if (newSoundData->m_VorbisInfo->channels == 1)
			{
				newSoundData->m_Format = AL_FORMAT_MONO16;
				// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
				newSoundData->m_BufferSize = newSoundData->m_Frequency >> 1;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				newSoundData->m_BufferSize -= (newSoundData->m_BufferSize % 2);
			}
			else if (newSoundData->m_VorbisInfo->channels == 2)
			{
				newSoundData->m_Format = AL_FORMAT_STEREO16;
				// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
				newSoundData->m_BufferSize = newSoundData->m_Frequency;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				newSoundData->m_BufferSize -= (newSoundData->m_BufferSize % 4);
			}
			else if (newSoundData->m_VorbisInfo->channels == 4)
			{
				newSoundData->m_Format = alGetEnumValue("AL_FORMAT_QUAD16");
				// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
				newSoundData->m_BufferSize = newSoundData->m_Frequency * 2;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				newSoundData->m_BufferSize -= (newSoundData->m_BufferSize % 8);
			}
			else if (newSoundData->m_VorbisInfo->channels == 6)
			{
				newSoundData->m_Format = alGetEnumValue("AL_FORMAT_51CHN16");
				// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
				newSoundData->m_BufferSize = newSoundData->m_Frequency * 3;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				newSoundData->m_BufferSize -= (newSoundData->m_BufferSize % 12);
			}
		}
	}

	//  Return the index of the new sound data entry
	return int(soundDataList.size() - 1);
}

bool SoundWrapper::playSoundFile(int soundIndex)
{
	//  Ensure the sound index is valid before continuing;
	if (soundIndex < 0) return false;
	if (soundIndex >= int(soundDataList.size())) return false;

	SoundData* soundData = soundDataList[soundIndex];
	if (soundData->m_SoundState == SoundData::SOUNDSTATE_LOADED)
	{
		alSourceRewind(soundData->m_SoundSource);
		alSourcePlay(soundData->m_SoundSource);

		soundData->iTotalBuffersProcessed = 0;
	}
	else if (soundData->m_Format != 0)
	{
		// Allocate a buffer to be used to store decoded data for all Buffers
		soundData->m_DecodeBuffer = (char*)malloc(soundData->m_BufferSize);
		if (!soundData->m_DecodeBuffer)
		{
			ALFWprintf("Failed to allocate memory for decoded OggVorbis data\n");
			fn_ov_clear(&soundData->m_OggFile);
			return false;
		}

		// Generate some AL Buffers for streaming
		alGenBuffers(NUMBUFFERS, soundData->m_SoundBuffers);

		// Generate a Source to playback the Buffers
		alGenSources(1, &soundData->m_SoundSource);

		// Fill all the Buffers with decoded audio data from the OggVorbis file
		for (int i = 0; i < NUMBUFFERS; i++)
		{
			soundData->ulBytesWritten = DecodeOggVorbis(&soundData->m_OggFile, soundData->m_DecodeBuffer, soundData->m_BufferSize, soundData->m_Channels);
			if (soundData->ulBytesWritten)
			{
				alBufferData(soundData->m_SoundBuffers[i], soundData->m_Format, soundData->m_DecodeBuffer, soundData->ulBytesWritten, soundData->m_Frequency);
				alSourceQueueBuffers(soundData->m_SoundSource, 1, &soundData->m_SoundBuffers[i]);
			}
		}

		// Start playing source
		alSourcePlay(soundData->m_SoundSource);

		soundData->iTotalBuffersProcessed = 0;
	}
	else
	{
		ALFWprintf("Failed to find format information, or unsupported format\n");
		return false;
	}

	m_SoundPlayingCount++;
	soundData->m_SoundState = SoundData::SOUNDSTATE_PLAYING;

	return true;
}

bool SoundWrapper::pauseSoundFile(int soundIndex)
{
	return true;
}

bool SoundWrapper::stopSoundFile(int soundIndex)
{
	SoundData* soundData = soundDataList[soundIndex];

	//  If the sound data is not playing, return a failure. Otherwise, mark it unplayed for the future.
	if (soundData->m_SoundState != SoundData::SOUNDSTATE_PLAYING) return false;
	soundData->m_SoundState = SoundData::SOUNDSTATE_LOADED;

	// Stop the Source and clear the Queue
	alSourceStop(soundData->m_SoundSource);
	alSourcei(soundData->m_SoundSource, AL_BUFFER, 0);

	if (soundData->m_DecodeBuffer)
	{
		free(soundData->m_DecodeBuffer);
		soundData->m_DecodeBuffer = NULL;
	}

	// Clean up buffers and sources
	alDeleteSources(1, &soundData->m_SoundSource);
	alDeleteBuffers(NUMBUFFERS, soundData->m_SoundBuffers);

	//  Clear away the file data
	fn_ov_clear(&soundData->m_OggFile);

	//  Decrement the sound playing count and return a success
	m_SoundPlayingCount--;
	soundDataList.erase(soundIndex);
	determineFirstIndex();
	return true;
}


bool SoundWrapper::isPlaying()
{
	return (m_SoundPlayingCount > 0);
}

void SoundWrapper::Update()
{
	continueAllSounds();
}

void SoundWrapper::Shutdown()
{
	//  Unload all sound files that are currently still loaded
	unloadAllSoundFiles();

	//  Disconnect from the Vorbis DLL(s) and set us to de-initialized
	if (g_hVorbisFileDLL)
	{
		FreeLibrary(g_hVorbisFileDLL);
		g_hVorbisFileDLL = NULL;
	}
	m_bVorbisInitialized = false;

	// Shutdown AL
	ALFWShutdownOpenAL();

	// Shutdown Framework
	ALFWShutdown();
}

SoundWrapper::SoundWrapper() :
	m_bVorbisInitialized(false),
	m_SoundPlayingCount(0),
	m_FirstIndex(0)
{

}

SoundWrapper::~SoundWrapper()
{
	//  Call the shutdown which will free all memory and set us back to a default state
	Shutdown();
}

void SoundWrapper::continueAllSounds()
{
	for (int i = 0; i < int(soundDataList.size()); ++i)
	{
		SoundData* soundData = soundDataList[i];
		if (soundData->m_SoundState != SoundData::SOUNDSTATE_PLAYING) continue;

		//Sleep(SERVICE_UPDATE_PERIOD);

		// Request the number of OpenAL Buffers have been processed (played) on the Source
		soundData->iBuffersProcessed = 0;
		alGetSourcei(soundData->m_SoundSource, AL_BUFFERS_PROCESSED, &soundData->iBuffersProcessed);

		// Keep a running count of number of buffers processed (for logging purposes only)
		soundData->iTotalBuffersProcessed += soundData->iBuffersProcessed;
		//ALFWprintf("Buffers Processed %d\r", iTotalBuffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (soundData->iBuffersProcessed)
		{
			// Remove the Buffer from the Queue.  (soundData->m_BufferID contains the Buffer ID for the unqueued Buffer)
			soundData->m_BufferID = 0;
			alSourceUnqueueBuffers(soundData->m_SoundSource, 1, &soundData->m_BufferID);

			// Read more audio data (if there is any)
			soundData->ulBytesWritten = DecodeOggVorbis(&soundData->m_OggFile, soundData->m_DecodeBuffer, soundData->m_BufferSize, soundData->m_Channels);
			if (soundData->ulBytesWritten)
			{
				alBufferData(soundData->m_BufferID, soundData->m_Format, soundData->m_DecodeBuffer, soundData->ulBytesWritten, soundData->m_Frequency);
				alSourceQueueBuffers(soundData->m_SoundSource, 1, &soundData->m_BufferID);
			}

			soundData->iBuffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		ALint currentState;
		alGetSourcei(soundData->m_SoundSource, AL_SOURCE_STATE, &currentState);
		if (currentState != AL_PLAYING)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(soundData->m_SoundSource, AL_BUFFERS_QUEUED, &soundData->iQueuedBuffers);
			if (soundData->iQueuedBuffers)
			{
				alSourcePlay(soundData->m_SoundSource);
			}
			else
			{
				// Finished playing
				stopSoundFile(i);
				continue;
			}
		}
	}
}

void SoundWrapper::unloadAllSoundFiles()
{
	//  TO DO: Stop all sound files
	for (int i = 0; i < int(soundDataList.size()); ++i)
	{
		stopSoundFile(i);
	}

	//  TO DO: Delete and free the memory of all sound files
}

void SoundWrapper::determineFirstIndex()
{
	std::map<int, SoundData*>::const_iterator iter;
	for (int i = 0; ; ++i)
	{
		iter = soundDataList.find(i);
		if (iter == soundDataList.end())
		{
			m_FirstIndex = i;
			return;
		}
	}
}

//  Background Functions
unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
	int current_section;
	long lDecodeSize;
	unsigned long ulSamples;
	short *pSamples;

	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = fn_ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (ulChannels == 6)
	{
		pSamples = (short*)pDecodeBuffer;
		for (ulSamples = 0; ulSamples < (ulBufferSize >> 1); ulSamples += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			Swap(pSamples[ulSamples + 1], pSamples[ulSamples + 2]);
			Swap(pSamples[ulSamples + 3], pSamples[ulSamples + 5]);
			Swap(pSamples[ulSamples + 4], pSamples[ulSamples + 5]);
		}
	}

	return ulBytesDone;
}

void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

int ov_close_func(void *datasource)
{
	return fclose((FILE*)datasource);
}

long ov_tell_func(void *datasource)
{
	return ftell((FILE*)datasource);
}