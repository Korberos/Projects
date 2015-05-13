////////////////////////////////////////////////////////////////////////////////
//
//	Name:			GameController.cpp
//
//	Description:	This file is the control for the main game
//					or program, and controls all further action
//					after the window is created and this class
//					is initialize.
//
//	Created:		December 7th, 2008
//
//	Last Modified:	January 1st, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "GameController.h"

CGameController::CGameController()
{
	m_MapController = CMapController::GetInstance();
	m_nElapsedTime[0] = m_nElapsedTime[1] = SDL_GetTicks();
	memset(m_nKeys, 0, sizeof(unsigned int) * SDLK_LAST);

	m_TankUnitTest.Initialize("Character.char");
}

CGameController::~CGameController()
{
}

void CGameController::Input(void)
{
	if (m_nKeys[SDLK_a])		m_Camera.MoveRight(0.01f * m_fTimeSlice);
	if (m_nKeys[SDLK_d])		m_Camera.MoveRight(-0.01f * m_fTimeSlice);
	if (m_nKeys[SDLK_w])		m_Camera.MoveForward(0.03f * m_fTimeSlice);
	if (m_nKeys[SDLK_s])		m_Camera.MoveForward(-0.03f * m_fTimeSlice);
	if (m_nKeys[SDLK_q])		m_Camera.MoveUp(0.025f * m_fTimeSlice);
	if (m_nKeys[SDLK_e])		m_Camera.MoveUp(-0.025f * m_fTimeSlice);
	if (m_nKeys[SDLK_UP])		m_Camera.RotateLocalX(-0.01f * m_fTimeSlice);
	if (m_nKeys[SDLK_DOWN])		m_Camera.RotateLocalX(0.01f * m_fTimeSlice);
	if (m_nKeys[SDLK_LEFT])		m_Camera.RotateWorld(0.013f * m_fTimeSlice, 0.0f, 1.0f, 0.0f);
	if (m_nKeys[SDLK_RIGHT])	m_Camera.RotateWorld(-0.013f * m_fTimeSlice, 0.0f, 1.0f, 0.0f);
}

void CGameController::Update(void)
{
	// Update the Time Slice to how much time has gone by since the last update
	m_nElapsedTime[0] = SDL_GetTicks();
	m_fTimeSlice = max(min((float)(m_nElapsedTime[0] - m_nElapsedTime[1]) / 1000.0f, 0.1f), 1.0f);
	m_nElapsedTime[1] = m_nElapsedTime[0];
	
	// Update the tank test unit
	m_TankUnitTest.Update(m_fTimeSlice);
}

void CGameController::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)m_Window.GetHeight() / (GLfloat)m_Window.GetWidth(), 0.1f, 100.0f);
	m_Camera.ApplyCameraTransform();

	// Render all 3D Geometry
	m_MapController->RenderAll();

	// Render the tank test unit
	m_TankUnitTest.Render();

	// 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (GLdouble)m_Window.GetWidth(), (GLdouble)m_Window.GetHeight(), 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render all 2D Aspects
	CTextureManager::GetInstance()->DrawTexture(Image, 100, 100, 50, 50);
}

void CGameController::Resize(int X, int Y)
{
	std::cout << "Resizing Window to " << X << "x" << Y << std::endl;
	
	if (Y <= 0)
	{
		Y = 1;
	}

	glViewport(0, 0, X, Y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)X / (GLfloat)Y, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool CGameController::ProcessEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) // Loop through all events
	{
		switch (event.type)
		{
			// Quit event
			case SDL_QUIT:
			{
				// Return false because we are quitting
				return false;
			}

			case SDL_KEYDOWN:
			{
				SDLKey symbol = event.key.keysym.sym;

				if(symbol == SDLK_ESCAPE) //Quit if escape was pressed
				{
					return false;
				}

				m_nKeys[symbol] = 1;
				break;
			}

			case SDL_KEYUP:
			{
				SDLKey symbol = event.key.keysym.sym;
				m_nKeys[symbol] = 0;
				break;
			}

			case SDL_VIDEORESIZE:
			{
				//the window has been resized so we need to set up our viewport and projection according to the new size
				Resize(event.resize.w, event.resize.h);
				break;
			}
			// Default case
			default:
			{
				break;
			}
		}
	}

	return true;
}

bool CGameController::Initialize()
{
	if(!m_Window.DefineWindow(800, 600, 32, false, "Tanks and Tactics Tech Demo"))
	{
		return false;
	}

	m_Camera.SetOrigin(-0.04f, 1.25f, 4.23f);

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.1f);

	m_MapController->LoadMap("TestMapFile.ttm");
	Image = CTextureManager::GetInstance()->LoadTexture("TestImage.png");

	return true;
}

void CGameController::Run()
{
	while (ProcessEvents())
	{
		Input();
		Update();
		Render();
		SDL_GL_SwapBuffers();
	}
}