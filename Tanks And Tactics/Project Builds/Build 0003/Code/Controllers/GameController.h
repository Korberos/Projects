////////////////////////////////////////////////////////////////////////////////
//
//	Name:			GameController.h
//
//	Description:	This file is the control for the main game
//					or program, and controls all action within
//					itself through the single function call "Run"
//
//	Created:		December 7th, 2008
//
//	Last Modified:	January 31st, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_GAMECONTROLLER_
#define _DB_GAMECONTROLLER_

#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "sdlmain.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <iostream>

#include "./../Global.h"
#include "./../Window.h"
#include "./InputController.h"
#include "./../Game States/State_MainGame.h"
#include "./../Utilities/RandomNumber.h"

class CGameController
{
//  ~Member Functions
public:
	CGameController()		{}
	~CGameController()		{}

	bool				Run(void);


private:
	//  Secondary Functionality
	bool				Initialize(void);
	bool				ProcessEvents(void);
	void				ResizeView(unsigned int Width, unsigned int Height);

	//  Primary Functionality
	bool				Input(void);
	void				Update(void);
	void				Render(void);

//  ~Member Variables
public:


private:
	CWindow				m_Window;
	CInputController*	m_pInput;
	float				m_fTimeSlice;
	CGameState*			m_pGameState;
};

#endif