////////////////////////////////////////////////////
// File Name:			StateLayerSystem.h
// Author:				Drew Banyai
// Date Created:		July 14th, 2008
// Date Modified:		July 14th, 2008
// Description:			Controls the processing and rendering of different states
// Additional Notes:	Includes the main Game State template class
////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <vector>
using std::vector;

#include "../Utilities/OpenGL/OGL_Functions.h"

#define		BIT_INPUT		1
#define		BIT_UPDATE		2
#define		BIT_RENDER3D	4
#define		BIT_RENDER2D	8
#define		BIT_ALLBITS		15

#define		GAMESTATE_01	1
#define		GAMESTATE_02	2


class GameState
{
private:
	float*	m_pTime;

public:
	GameState()					{	m_pTime = 0;	}
	virtual ~GameState()		{}

	int		m_nStateID;
	char	BITFLAG;
	
	virtual void	Initialize(float* fTime)									{}
	virtual bool	Input(int nMouseX, int nMouseY, char* KeyArray)				{	return false;		}
	virtual int		Update(int nMouseX, int nMouseY)							{	return 1;			}
	virtual void	Render3D(void)												{}
	virtual void	Render2D(void)												{}

	inline void	SetBitFlag(char BitFlag)										{	BITFLAG = BitFlag;	}
};



class StateLayerSystem
{
private:
	StateLayerSystem();
	~StateLayerSystem();

	vector<GameState*>			GameStateLayers;
	vector<GameState*>			LayersToRemove;
	vector<GameState*>			LayersToAdd;

	HWND						Window_Handle;
	HINSTANCE					App_Instance;

	float						m_fTimeSlice;
	int							Window_Width;
	int							Window_Height;

	// Basic OpenGL Camera
	GLFrame						m_Camera;

	// Key Array
	char*						Key_Array;

	// Timing (FPS)
	bool						m_bResetTimeTrigger;
	__int64						m_n64PrevTimeStamp;
	float						m_nSecsPerCnt;

	// Mouse Position (Keep it local to the state system)
	int							m_nMouseX;
	int							m_nMouseY;


public:
	static StateLayerSystem* GetInstance(void);

	void	Initialize(HWND Window_Handle, HINSTANCE App_Instance, int WindowWidth = 1024, int WindowHeight = 768, char* KeyArray = 0);
	bool	MainProcess(void);

	inline void			ResetTimeSlice(void)			{	m_bResetTimeTrigger = true; m_fTimeSlice = 0.0f;		}
	inline float*		GetTimeSliceAddress(void)		{	return &m_fTimeSlice;									}

	static GLFrame*		GetCamera(void)					{	return &(StateLayerSystem::GetInstance()->m_Camera);	}
};