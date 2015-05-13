////////////////////////////////////////////////////
// File Name:			MainProgram.h
// Author:				Drew Banyai
// Date Created:		July 17th, 2008
// Date Modified:		July 17th, 2008
// Description:			Controls the processing and rendering of different states
// Additional Notes:	~
////////////////////////////////////////////////////
#pragma once

#include <windows.h>

#include "./Utilities/OpenGL/OGL_Functions.h"
#include "Heightfield.h"

class MainProgram
{
private:
	MainProgram();
	~MainProgram();

	HWND						Window_Handle;
	HINSTANCE					App_Instance;

	float						m_fTimeSlice;
	int							Window_Width;
	int							Window_Height;

	// Basic OpenGL Camera
	GLFrame						m_Camera;

	// Key Array
	bool*						Key_Array;

	// Timing (FPS)
	bool						m_bResetTimeTrigger;
	__int64						m_n64PrevTimeStamp;
	float						m_nSecsPerCnt;

	// Mouse Position (Keep it local to the state system)
	int							m_nMouseX;
	int							m_nMouseY;

	SwiftHeightField			m_hHeight;


public:
	static MainProgram* GetInstance(void);

	void	Initialize(HWND Window_Handle, HINSTANCE App_Instance, int WindowWidth = 1024, int WindowHeight = 768, bool* KeyArray = 0);
	bool	MainProcess(void);

	inline void			ResetTimeSlice(void)			{	m_bResetTimeTrigger = true; m_fTimeSlice = 0.0f;		}

	static GLFrame*		GetCamera(void)					{	return &(MainProgram::GetInstance()->m_Camera);	}
};