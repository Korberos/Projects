#include "WorldController.h"
#include "SDL_Window.h"
#include "OpenGLCamera.h"
#include "InputSystem.h"

#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "sdlmain.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


void Resize_View( unsigned int width, unsigned int height )
{
	//! This function will take in a Width and Height, and alter
	//! the viewport of the screen as well as the window size.

	//  Ensure that Height is a positive number
	if ( height <= 0 ) height = 1;

	// Set up the Viewport
	glViewport( 0 , 0 , width , height );

	// Set up Projection View
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0 , (GLdouble)width / (GLdouble)height , 1.0 , 100.0 );

	// Set up Model View
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

bool SDL_ProcessEvents( InputSystem* input )
{
	//! This function will take in information from SDL's OS
	//! message retrieval system to get things such as input
	//! and important system messages relating to the window.

	//! It will return true if all messages are able to be
	//! handled and false if any message indicates a shutdown
	//! of the program for any reason.

	//  Loop through all events
	SDL_Event E;
	while ( SDL_PollEvent(&E) )
	{
		switch ( E.type )
		{
			case SDL_QUIT:
			{
				//  Return false for the quit event
				return false;
			}

			case SDL_KEYDOWN:
			{	
				// Set the designated key to 1
				input->Set_Key( E.key.keysym.sym , 1 );
				input->Add_Key_To_String( (short)E.key.keysym.sym );
				break;
			}

			case SDL_KEYUP:
			{
				// Set the designated key to 0
				input->Set_Key( E.key.keysym.sym , 0 );
				break;
			}

			case SDL_MOUSEMOTION:
			{
				input->Set_Mouse_X( E.motion.x );
				input->Set_Mouse_Y( E.motion.y );
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				switch ( E.button.button )
				{
				case SDL_BUTTON_LEFT:
					input->Set_Mouse_Button( 0 , 1 );
					break;
				case SDL_BUTTON_RIGHT:
					input->Set_Mouse_Button( 1 , 1 );
					break;
				case SDL_BUTTON_MIDDLE:
					input->Set_Mouse_Button( 2 , 1 );
					break;
				}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				switch ( E.button.button )
				{
				case SDL_BUTTON_LEFT:
					input->Set_Mouse_Button( 0 , 0 );
					break;
				case SDL_BUTTON_RIGHT:
					input->Set_Mouse_Button( 1 , 0 );
					break;
				case SDL_BUTTON_MIDDLE:
					input->Set_Mouse_Button( 2 , 0 );
					break;
				default:
					break;
				}
				break;
			}

			case SDL_VIDEORESIZE:
			{
				Resize_View( E.resize.w , E.resize.h );
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


int main( int argc, char* argv[] )
{
	InputSystem* input_system = InputSystem::Get_Instance();

	//  Define the window. If the window definition fails, return a failure.
	SDL_Window Window;
	if (!Window.Define_Window(1024, 768, 32, false, "World Controller test")) return -1;

	//  Grab an instance of the world controller and load the test map
	WorldController* world_controller = WorldController::Get_Instance();
	world_controller->Load_World_Geometry( "test_world.xml" );

	glClearColor(0.7f, 0.7f, 0.8f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.02f);

	COpenGLFrame m_Camera;
	m_Camera.SetOrigin(1.0f, 1.0f, -5.0f);
	m_Camera.SetForward(0.0f, 0.0f, 1.0f);
	m_Camera.Normalize();

	// Render the test map
	while (SDL_ProcessEvents( input_system ))
	{
		if (input_system->Get_Key(27)) break;		// 27 : ESCAPE

		//  Get the amount of time that has passed since the last step
		static float m_fTimeSlice;
		static int m_nElapsedTime[2];
		m_nElapsedTime[0] = SDL_GetTicks();
		m_fTimeSlice = std::max(std::min((float)(m_nElapsedTime[0] - m_nElapsedTime[1]) / 1000.0f, 1.0f), 0.1f);
		m_nElapsedTime[1] = m_nElapsedTime[0];

		//  Move the camera based on the input and the amount of time that has passed
		if (input_system->Get_Key(SDLK_a))		m_Camera.MoveRight(0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_d))		m_Camera.MoveRight(-0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_w))		m_Camera.MoveForward(0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_s))		m_Camera.MoveForward(-0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_q))		m_Camera.MoveUp(0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_e))		m_Camera.MoveUp(-0.5f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_UP))		m_Camera.RotateLocalX(-0.2f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_DOWN))	m_Camera.RotateLocalX(0.2f * (m_fTimeSlice));
		if (input_system->Get_Key(SDLK_LEFT))	m_Camera.RotateWorld(0.2f * (m_fTimeSlice), 0.0f, 1.0f, 0.0f);
		if (input_system->Get_Key(SDLK_RIGHT))	m_Camera.RotateWorld(-0.2f * (m_fTimeSlice), 0.0f, 1.0f, 0.0f);

		//  Begin rendering by clearing the color and depth buffers
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 45.0, 1024.0 / 768.0, 0.1, 100.0 );

		m_Camera.ApplyCameraTransform();

		world_controller->Render_Map();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 1024.0, 768.0, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		SDL_GL_SwapBuffers();

		input_system->Invalidate_Old_Input();
	}

	return 0;
}