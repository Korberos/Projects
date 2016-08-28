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
#include "OpenAL\SimpleAudioLib\CoreSystem.h"

#pragma comment(lib, "OpenAL\\OpenAL32.lib")

#include <map>

#define NUMBUFFERS              4

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

enum SoundFileType
{
	SOUNDFILETYPE_UNKNOWN,
	SOUNDFILETYPE_OGG,
	SOUNDFILETYPE_WAV
};

class SoundWrapper
{
private:
	struct SoundData
	{
	public:
		enum SoundStatus
		{
			SOUNDSTATUS_INVALID,
			SOUNDSTATUS_LOADED,
			SOUNDSTATUS_PLAYING
		};

		struct OggFileData
		{
		public:
			FILE*							m_SoundFile;
			OggVorbis_File					m_OggFile;
			vorbis_info*					m_VorbisInfo;
			unsigned long					m_Frequency;
			unsigned long					m_Channels;
			unsigned long					m_BufferSize;
			unsigned long					m_Format;
			char*							m_DecodeBuffer;
			ALuint							m_SoundBuffers[NUMBUFFERS];
			ALuint							m_SoundSource;
			ALuint							m_BufferID;
			ALint							iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
			unsigned long					ulBytesWritten;

			OggFileData(FILE* soundFile) :
				m_SoundFile(soundFile),
				m_Frequency(0),
				m_Channels(0),
				m_BufferSize(0),
				m_Format(0)
			{}

		};

		SoundStatus						m_SoundStatus;
		SoundFileType					m_SoundFileType;
		std::string						m_SoundFileName;
		OggFileData*					m_OggFileData;
		SimpleAudioLib::AudioEntity*	m_AudioEntity;	//  For WAV Files

		SoundData(const char* fileName, SoundFileType fileType) :
			m_SoundStatus(SOUNDSTATUS_INVALID),
			m_SoundFileType(fileType),
			m_SoundFileName(fileName),
			m_OggFileData(NULL),
			m_AudioEntity(NULL)
		{
		}
	};

public:
	static SoundWrapper& GetInstance() { static SoundWrapper INSTANCE; return INSTANCE; }

	bool Initialize();
	int loadSoundFile(const char* soundFileName, SoundFileType fileType);
	bool playSoundFile(int soundIndex, bool looping = false);
	bool unloadSoundFile(int soundIndex);
	void Update();
	void Shutdown();

	inline const int getSoundPlayingCount() const { return m_nSoundPlayingCount; }

private:
	SoundWrapper();
	~SoundWrapper();

	bool loadSoundData(SoundData*& soundData);
	void unloadAllSoundFiles();
	int determineFirstIndex();

	std::map<int, SoundData*> soundDataList;
	std::map<std::string, SoundData*> soundDataListByName;
	int m_nSoundPlayingCount;

	SimpleAudioLib::CoreSystem& m_SimpleAudioLib;
	bool m_bVorbisInitialized;
	HINSTANCE m_VorbisFileDLL;
};

bool SoundWrapper::Initialize()
{
	// Initialize OpenAL Framework
	ALFWInit();

	// Initialize OggVorbis libary, loading Vorbis DLLs (VorbisFile.dll will load ogg.dll and vorbis.dll)
	if (!m_bVorbisInitialized)
	{
		m_VorbisFileDLL = LoadLibrary("vorbisfile.dll");
		if (m_VorbisFileDLL)
		{
			fn_ov_clear = (LPOVCLEAR)GetProcAddress(m_VorbisFileDLL, "ov_clear");
			fn_ov_read = (LPOVREAD)GetProcAddress(m_VorbisFileDLL, "ov_read");
			fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(m_VorbisFileDLL, "ov_pcm_total");
			fn_ov_info = (LPOVINFO)GetProcAddress(m_VorbisFileDLL, "ov_info");
			fn_ov_comment = (LPOVCOMMENT)GetProcAddress(m_VorbisFileDLL, "ov_comment");
			fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(m_VorbisFileDLL, "ov_open_callbacks");

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

	// Initialise OpenAL and get the data out to send into SimpleAudioLib
	ALCdevice* pDevice = NULL;
	ALCcontext* pContext = NULL;
	if (!ALFWInitOpenAL(pDevice, pContext))
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

	//  Send the data in to SimpleAudioLib to be held for use with WAV files
	m_SimpleAudioLib.initWithDefinedDevice(pDevice, pContext);

	return true;
}

int SoundWrapper::loadSoundFile(const char* soundFileName, SoundFileType fileType)
{
	//  Find a new index, create a new SoundData class, and then load the sound data into it
	int newSoundIndex = determineFirstIndex();
	SoundData* newSoundData = new SoundData(soundFileName, fileType);
	soundDataList[newSoundIndex] = newSoundData;
	soundDataListByName[soundFileName] = newSoundData;
	loadSoundData(newSoundData);

	//  Return the index of the new sound data entry
	return newSoundIndex;
}

bool SoundWrapper::loadSoundData(SoundData*& soundData)
{
	if (soundData->m_SoundFileType == SOUNDFILETYPE_OGG && soundData->m_OggFileData == NULL)
	{
		// Open the OggVorbis file
		FILE *pOggVorbisFile;
		fopen_s(&pOggVorbisFile, soundData->m_SoundFileName.c_str(), "rb");
		if (!pOggVorbisFile)
		{
			ALFWprintf("Could not find %s\n", soundData->m_SoundFileName.c_str());
			return false;
		}

		SoundData::OggFileData*& oggFileData = soundData->m_OggFileData = new SoundData::OggFileData(pOggVorbisFile);
		if (fn_ov_open_callbacks(oggFileData->m_SoundFile, &oggFileData->m_OggFile, NULL, 0, soundCallbacks) == 0)
		{
			// Get some information about the file (Channels, Format, and Frequency)
			oggFileData->m_VorbisInfo = fn_ov_info(&oggFileData->m_OggFile, -1);
			if (oggFileData->m_VorbisInfo)
			{
				oggFileData->m_Frequency = oggFileData->m_VorbisInfo->rate;
				oggFileData->m_Channels = oggFileData->m_VorbisInfo->channels;

				if (oggFileData->m_VorbisInfo->channels == 1)
				{
					oggFileData->m_Format = AL_FORMAT_MONO16;
					// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
					oggFileData->m_BufferSize = oggFileData->m_Frequency >> 1;
					// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
					oggFileData->m_BufferSize -= (oggFileData->m_BufferSize % 2);
				}
				else if (oggFileData->m_VorbisInfo->channels == 2)
				{
					oggFileData->m_Format = AL_FORMAT_STEREO16;
					// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
					oggFileData->m_BufferSize = oggFileData->m_Frequency;
					// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
					oggFileData->m_BufferSize -= (oggFileData->m_BufferSize % 4);
				}
				else if (oggFileData->m_VorbisInfo->channels == 4)
				{
					oggFileData->m_Format = alGetEnumValue("AL_FORMAT_QUAD16");
					// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
					oggFileData->m_BufferSize = oggFileData->m_Frequency * 2;
					// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
					oggFileData->m_BufferSize -= (oggFileData->m_BufferSize % 8);
				}
				else if (oggFileData->m_VorbisInfo->channels == 6)
				{
					oggFileData->m_Format = alGetEnumValue("AL_FORMAT_51CHN16");
					// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
					oggFileData->m_BufferSize = oggFileData->m_Frequency * 3;
					// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
					oggFileData->m_BufferSize -= (oggFileData->m_BufferSize % 12);
				}
			}
		}
	}
	else if (soundData->m_SoundFileType == SOUNDFILETYPE_WAV && soundData->m_AudioEntity == NULL)
	{
		SimpleAudioLib::AudioEntity* newAudioEntity;
		newAudioEntity = m_SimpleAudioLib.createAudioEntityFromFile(soundData->m_SoundFileName.c_str());
		if (newAudioEntity == NULL)
		{
			ALFWprintf("Could not create AudioEntity for %s\n", soundData->m_SoundFileName.c_str());
			return false;
		}

		soundData->m_AudioEntity = newAudioEntity;
	}

	//  Set the sound state to loaded and return a success
	soundData->m_SoundStatus = SoundData::SOUNDSTATUS_LOADED;
	return true;
}

bool SoundWrapper::playSoundFile(int soundIndex, bool looping)
{
	//  Ensure the sound index is valid before continuing;
	if (soundIndex < 0) return false;
	if (soundIndex >= int(soundDataList.size())) return false;

	SoundData* soundData = soundDataList[soundIndex];

	//  If we're set to an invalid state, reload the data
	if (soundData->m_SoundStatus == SoundData::SOUNDSTATUS_INVALID)
	{
		loadSoundData(soundData);
	}

	//  Now that we have the correct sound data, play it
	if (soundData->m_SoundFileType == SOUNDFILETYPE_OGG)
	{
		SoundData::OggFileData*& oggFileData = soundData->m_OggFileData;
		if (oggFileData->m_Format != 0)
		{
			// Allocate a buffer to be used to store decoded data for all Buffers
			oggFileData->m_DecodeBuffer = (char*)malloc(oggFileData->m_BufferSize);
			if (!oggFileData->m_DecodeBuffer)
			{
				ALFWprintf("Failed to allocate memory for decoded OggVorbis data\n");
				fn_ov_clear(&oggFileData->m_OggFile);
				return false;
			}

			// Generate some AL Buffers for streaming
			alGenBuffers(NUMBUFFERS, oggFileData->m_SoundBuffers);

			// Generate a Source to playback the Buffers
			alGenSources(1, &oggFileData->m_SoundSource);

			// Fill all the Buffers with decoded audio data from the OggVorbis file
			for (int i = 0; i < NUMBUFFERS; i++)
			{
				oggFileData->ulBytesWritten = DecodeOggVorbis(&oggFileData->m_OggFile, oggFileData->m_DecodeBuffer, oggFileData->m_BufferSize, oggFileData->m_Channels);
				if (oggFileData->ulBytesWritten)
				{
					alBufferData(oggFileData->m_SoundBuffers[i], oggFileData->m_Format, oggFileData->m_DecodeBuffer, oggFileData->ulBytesWritten, oggFileData->m_Frequency);
					alSourceQueueBuffers(oggFileData->m_SoundSource, 1, &oggFileData->m_SoundBuffers[i]);
				}
			}

			// Start playing source
			alSourcePlay(oggFileData->m_SoundSource);
			alSourcei(oggFileData->m_SoundSource, AL_LOOPING, looping);

			oggFileData->iTotalBuffersProcessed = 0;
		}
		else
		{
			ALFWprintf("Failed to find format information, or unsupported format\n");
			return false;
		}
	}
	else if (soundData->m_SoundFileType == SOUNDFILETYPE_WAV)
	{
		soundData->m_AudioEntity->rewind();
		soundData->m_AudioEntity->play();
	}

	soundData->m_SoundStatus = SoundData::SOUNDSTATUS_PLAYING;
	m_nSoundPlayingCount++;

	return true;
}

bool SoundWrapper::unloadSoundFile(int soundIndex)
{
	//  Ensure the sound index is valid before continuing;
	if (soundIndex < 0) return false;
	if (soundIndex >= int(soundDataList.size())) return false;

	SoundData* soundData = soundDataList[soundIndex];

	if (soundData->m_SoundFileType == SOUNDFILETYPE_OGG)
	{
		SoundData::OggFileData*& oggFileData = soundData->m_OggFileData;

		// Stop the Source and clear the Queue
		alSourceStop(oggFileData->m_SoundSource);
		alSourcei(oggFileData->m_SoundSource, AL_BUFFER, 0);

		if (oggFileData->m_DecodeBuffer)
		{
			free(oggFileData->m_DecodeBuffer);
			oggFileData->m_DecodeBuffer = NULL;
		}

		// Clean up buffers and sources
		alDeleteSources(1, &oggFileData->m_SoundSource);
		alDeleteBuffers(NUMBUFFERS, oggFileData->m_SoundBuffers);

		//  Clear away the file data
		fn_ov_clear(&oggFileData->m_OggFile);

		delete soundData->m_OggFileData;
		soundData->m_OggFileData = NULL;
	}
	else if (soundData->m_SoundFileType == SOUNDFILETYPE_WAV)
	{
		delete soundData->m_AudioEntity;
		soundData->m_AudioEntity = NULL;
	}

	//  Decrement the sound playing count and return a success
	soundData->m_SoundStatus = SoundData::SOUNDSTATUS_INVALID;
	m_nSoundPlayingCount--;
	return true;
}

void SoundWrapper::Update()
{
	for (int i = 0; i < int(soundDataList.size()); ++i)
	{
		SoundData* soundData = soundDataList[i];
		if (soundData->m_SoundFileType != SOUNDFILETYPE_OGG) continue;

		SoundData::OggFileData*& oggFileData = soundData->m_OggFileData;
		if (oggFileData == NULL) continue;

		// Request the number of OpenAL Buffers have been processed (played) on the Source
		oggFileData->iBuffersProcessed = 0;
		alGetSourcei(oggFileData->m_SoundSource, AL_BUFFERS_PROCESSED, &oggFileData->iBuffersProcessed);

		// Keep a running count of number of buffers processed (for logging purposes only)
		oggFileData->iTotalBuffersProcessed += oggFileData->iBuffersProcessed;
		//ALFWprintf("Buffers Processed %d\r", iTotalBuffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (oggFileData->iBuffersProcessed)
		{
			// Remove the Buffer from the Queue.  (soundData->m_BufferID contains the Buffer ID for the unqueued Buffer)
			oggFileData->m_BufferID = 0;
			alSourceUnqueueBuffers(oggFileData->m_SoundSource, 1, &oggFileData->m_BufferID);

			// Read more audio data (if there is any)
			oggFileData->ulBytesWritten = DecodeOggVorbis(&oggFileData->m_OggFile, oggFileData->m_DecodeBuffer, oggFileData->m_BufferSize, oggFileData->m_Channels);
			if (oggFileData->ulBytesWritten)
			{
				alBufferData(oggFileData->m_BufferID, oggFileData->m_Format, oggFileData->m_DecodeBuffer, oggFileData->ulBytesWritten, oggFileData->m_Frequency);
				alSourceQueueBuffers(oggFileData->m_SoundSource, 1, &oggFileData->m_BufferID);
			}

			oggFileData->iBuffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		ALint currentState;
		alGetSourcei(oggFileData->m_SoundSource, AL_SOURCE_STATE, &currentState);
		if (currentState != AL_PLAYING)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(oggFileData->m_SoundSource, AL_BUFFERS_QUEUED, &oggFileData->iQueuedBuffers);
			if (oggFileData->iQueuedBuffers)
			{
				alSourcePlay(oggFileData->m_SoundSource);
			}
			else
			{
				// Finished playing
				unloadSoundFile(i);
				continue;
			}
		}
	}
}

void SoundWrapper::Shutdown()
{
	//  Unload all sound files that are currently still loaded
	unloadAllSoundFiles();

	//  Disconnect from the Vorbis DLL(s) and set us to de-initialized
	if (m_VorbisFileDLL)
	{
		FreeLibrary(m_VorbisFileDLL);
		m_VorbisFileDLL = NULL;
	}
	m_bVorbisInitialized = false;

	// Shutdown AL
	ALFWShutdownOpenAL();

	// Shutdown Framework
	ALFWShutdown();
}

SoundWrapper::SoundWrapper() :
	m_nSoundPlayingCount(0),
	m_SimpleAudioLib(SimpleAudioLib::CoreSystem::getInstance()),
	m_bVorbisInitialized(false),
	m_VorbisFileDLL(NULL)
{
}

SoundWrapper::~SoundWrapper()
{
	//  Call the shutdown which will free all memory and set us back to a default state
	Shutdown();

	//  Shut down the SimpleAudioLib core
	SimpleAudioLib::CoreSystem::release();
}

void SoundWrapper::unloadAllSoundFiles()
{
	//  Delete all sound file data
	for (int i = 0; i < int(soundDataList.size()); ++i)
	{
		unloadSoundFile(i);
	}
}

int SoundWrapper::determineFirstIndex()
{
	std::map<int, SoundData*>::const_iterator iter;
	for (int i = 0; ; ++i)
	{
		iter = soundDataList.find(i);
		if (iter == soundDataList.end()) return i;
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