////////////////////////////////////////////////////////////////////////////////
//
//	Name:			GameController.h
//
//	Description:	This file is the control for the main game
//					or program, and controls all further action
//					after the window is created and this class
//					is initialize.
//
//	Created:		December 7th, 2008
//
//	Last Modified:	December 7th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_GAMECONTROLLER_
#define _DB_GAMECONTROLLER_

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "sdlmain.lib")

#include "Window.h"
#include "GL/GLFrame.h"
#include "MapController.h"
#include "CharacterUnit.h"
#include "SimpleModel.h"
#include "3D_Model.h"
#include "Utilities/TextureManager.h"

class CGameController
{
private:
	// All members needed for basic functions
	CWindow			m_Window;			// The program window
	unsigned int	m_nKeys[SDLK_LAST];	// An array of keys placed up or down
	int				m_nElapsedTime[2];	// The two integers used for the timer
	float			m_fTimeSlice;		// The float used to represent the timeslice of the step

	GLFrame			m_Camera;			// The 3D Camera instance
	CMapController*	m_MapController;	// The map controller instance
	CharacterUnit	m_TankUnitTest;		// A unit (Tank) for testing purposes
	unsigned int	Image;

	void	Input(void);
	void	Update(void);
	void	Render(void);

	void	Resize(int X, int Y);
	bool	ProcessEvents();

public:
	CGameController();
	~CGameController();

	bool	Initialize(void);
	void	Run(void);
};

#endif