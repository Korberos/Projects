////////////////////////////////////////////////////////////////////////////////
//
//	Name:			GameController.cpp
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
#include "GameController.h"

bool CGameController::Run(void)
{
	if (!Initialize()) return false;

	while (ProcessEvents())
	{
		if (!Input()) break;
		Update();
		Render();
		SDL_GL_SwapBuffers();
	}

	return true;
}

bool CGameController::Initialize(void)
{
	//! This function will initialize all information into the
	//! class and define all starting variables, including the
	//! other controllers working within the GameController.

	//! The function will return true if all is successful, and
	//! false if the function fails to initialize any required
	//! functionality.
	
	// Initialize (seed) the random number generator
	SeedNumberGenerator();

	// Set up the general information for the rendering of the window through OpenGL
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.02f);

	// Gather all necessary controllers
	m_pInput = CInputController::GetInstance();

	// Create the starting game state (in this case, CState_Battle)
	m_pGameState = new CState_Battle(&m_fTimeSlice);

	return true;
}

bool CGameController::ProcessEvents(void)
{
	//! This function will take in information from SDL's OS
	//! message retrieval system to get things such as input
	//! and important system messages relating to the window.

	//! It will return true if all messages are able to be
	//! handled and false if any message indicates a shutdown
	//! of the program for any reason.

	//  Loop through all events
	SDL_Event E;
	while (SDL_PollEvent(&E))
	{
		switch (E.type)
		{
			case SDL_QUIT:
			{
				//  Return false for the quit event
				return false;
			}

			case SDL_KEYDOWN:
			{	
				// Set the designated key to 1
				m_pInput->SetKey(E.key.keysym.sym, 1);
				break;
			}

			case SDL_KEYUP:
			{
				// Set the designated key to 0
				m_pInput->SetKey(E.key.keysym.sym, 0);
				break;
			}

			case SDL_MOUSEMOTION:
			{
				m_pInput->SetMouseX(E.motion.x);
				m_pInput->SetMouseY(E.motion.y);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				switch (E.button.button)
				{
				case SDL_BUTTON_LEFT:
					m_pInput->SetMouseButton(0, 1);
					break;
				case SDL_BUTTON_RIGHT:
					m_pInput->SetMouseButton(1, 1);
					break;
				case SDL_BUTTON_MIDDLE:
					m_pInput->SetMouseButton(2, 1);
					break;
				}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				switch (E.button.button)
				{
				case SDL_BUTTON_LEFT:
					m_pInput->SetMouseButton(0, 0);
					break;
				case SDL_BUTTON_RIGHT:
					m_pInput->SetMouseButton(1, 0);
					break;
				case SDL_BUTTON_MIDDLE:
					m_pInput->SetMouseButton(2, 0);
					break;
				default:
					break;
				}
				break;
			}

			case SDL_VIDEORESIZE:
			{
				ResizeView(E.resize.w, E.resize.h);
				break;
			}

			default:
			{
				break;
			}
		}
	}

	return true;
}

void CGameController::ResizeView(unsigned int Width, unsigned int Height)
{
	//! This function will take in a Width and Height, and alter
	//! the viewport of the screen as well as the window size.

	//  Ensure that Height is a positive number
	if (Height <= 0) Height = 1;

	// Set up the Viewport
	glViewport(0, 0, Width, Height);

	// Set up Projection View
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)Width / (GLdouble)Height, 1.0, 100.0);

	// Set up Model View
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool CGameController::Input(void)
{
	//! This function will process the input retrieved from the
	//! OS and will operate the program based on it.

	// Apply any and all input to the current GameState
	if (!m_pGameState->Input()) return false;

	// Once all input is taken, invalidate the keys just pressed
	// down to indicate that they are held down for future calls.
	m_pInput->InvalidateOldInput();

	return true;
}

void CGameController::Update(void)
{
	//! This function will take the amount of time that has passed
	//! since the last frame and update all game objects that need
	//! to be updated before rendering.

	// Update the Elapsed Time since the last call to Update
	static int m_nElapsedTime[2];
	m_nElapsedTime[0] = SDL_GetTicks();
	m_fTimeSlice = std::max(std::min((float)(m_nElapsedTime[0] - m_nElapsedTime[1]) / 1000.0f, 1.0f), 0.1f);
	m_nElapsedTime[1] = m_nElapsedTime[0];

	// Update the current GameState
	m_pGameState->Update();
}

void CGameController::Render(void)
{
	//! This function will call the rendering functionality of all
	//! necessary game objects, such as menu systems or game unit
	//! controllers, and will alter the screen to take in both 3D
	//! and 2D graphics.
	
	// Clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Initiate the 3D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, m_dHeightByWidth, 0.1, 100.0);

	// Call the 3D Rendering on the current Game State
	m_pGameState->Render3D();

	// Initiate the 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_dWidth, m_dHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Call the 2D Rendering on the current Game State
	m_pGameState->Render2D();
}