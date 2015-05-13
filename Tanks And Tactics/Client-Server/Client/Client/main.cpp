#define WINDOW_WIDTH		1024
#define WINDOW_HEIGHT	768

#pragma comment(lib, "sdlmain.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "InputSystem.h"
#include "SDL_Window.h"
#include "TaskScheduler.h"
#include "MainMenuDialogHandler.h"
#include "GameClient.h"
#include "DebugConsole.h"
#include "FontController.h"
#include "RandomNumbers.h"
#include "DialogSystem/MenuGUI.h"
#include "DialogSystem/Elements/MenuElement_Frame.h"
#include "DialogSystem/Elements/MenuElement_CheckBox.h"
#include "DialogSystem/Elements/MenuElement_TextureButton.h"
#include "DialogSystem/Elements/MenuElement_TextButton.h"
#include "DialogSystem/Elements/MenuElement_DropDown.h"
#include "DialogSystem/Elements/MenuElement_TextBlock.h"
#include "DialogSystem/Elements/MenuElement_ListBox.h"
#include "DialogSystem/Elements/MenuElement_EditBox.h"
#include "DialogSystem/Elements/MenuElement_Texture.h"

void Setup_OpenGL_Defaults( void );
bool SDL_ProcessEvents( InputSystem* Input );
void Resize_View( unsigned int Width, unsigned int Height );

int main( int argc , char* argv[] )
{
	//  Seed the random number generator
	Seed_Random_Number_Generator();

	//  Create access to the InputSystem
	InputSystem* input_system = InputSystem::Get_Instance();

	//  Create access to the DialogController
	DialogController* dialog_controller = DialogController::Get_Instance();
	MainMenuDialogHandler::Set_As_Active_Dialog();

	
	std::string window_name( "Tanks and Tactics - Client" );
#ifdef _DEBUG
	window_name += " - DEBUG";
#else
	window_name += " - RELEASE";
#endif

	//  Define the window. If the window definition fails, return a failure flag.
	SDL_Window window;
	if ( !window.Define_Window( WINDOW_WIDTH , WINDOW_HEIGHT , 32 , false , window_name.c_str() ) )
	{
		assert( "Failed to define the window." && false );
		return 1;
	}

	//  Create the Font Controller and load in basic fonts
	FontController* font_controller = FontController::Get_Instance();
	font_controller->Load_Font( "Arial-12" );
	font_controller->Load_Font( "Arial-12-White" );

	//  Create the Task Scheduler
	TaskScheduler* task_scheduler = TaskScheduler::Get_Instance();

	//  Create the Debug Console
	DebugConsole* debug_console = DebugConsole::Get_Instance();
	debug_console->Resize( WINDOW_WIDTH / 3 * 2, WINDOW_HEIGHT / 5 * 2 );
	debug_console->Recolor( 0.125f, 0.125f, 0.125f, 0.95f );
	debug_console->Set_Bitmap_Font( "Arial-12-White" );

	//  Load in menu element types
	MenuGUI::Add_Element_Type( "Frame", &MenuGUIElementType_Frame::Instance );
	MenuGUI::Add_Element_Type( "CheckBox", &MenuGUIElementType_CheckBox::Instance );
	MenuGUI::Add_Element_Type( "TextureButton", &MenuGUIElementType_TextureButton::Instance );
	MenuGUI::Add_Element_Type( "TextButton", &MenuGUIElementType_TextButton::Instance );
	MenuGUI::Add_Element_Type( "DropDown", &MenuGUIElementType_DropDown::Instance );
	MenuGUI::Add_Element_Type( "TextBlock", &MenuGUIElementType_TextBlock::Instance );
	MenuGUI::Add_Element_Type( "ListBox", &MenuGUIElementType_ListBox::Instance );
	MenuGUI::Add_Element_Type( "EditBox", &MenuGUIElementType_EditBox::Instance );
	MenuGUI::Add_Element_Type( "Texture", &MenuGUIElementType_Texture::Instance );

	//  Initialize the Winsock Wrapper
	Winsock_Init( BUFFER_TYPE_COUNT );

	// Set up the general information for the rendering of the window through OpenGL
	Setup_OpenGL_Defaults();

	//  Create the GameClient instance and initialize it
	GameClient* game_client = GameClient::Get_Instance();
	if ( !game_client->Initialize() )
	{
		assert( "Failed to initialize the game client." && false );
		return 1;
	}

	//  Process any events from the Operating System
	while ( SDL_ProcessEvents( input_system ) )
	{
		if (input_system->Get_Key(27)) break;		// 27 : ESCAPE

		// Get the time slice
		static float time_slice;
		static int elapsed_time[2];
		elapsed_time[0] = SDL_GetTicks();
		time_slice = std::max( std::min( float(elapsed_time[0] - elapsed_time[1]) / 1000.0f, 1.0f ), 0.0f );
		elapsed_time[1] = elapsed_time[0];

		//  INPUT
		game_client->Input( time_slice );
		dialog_controller->Input();
		debug_console->Input();

		// UPDATE
		game_client->Update( time_slice );
		dialog_controller->Update( time_slice );
		task_scheduler->Update( time_slice );

		// RENDER
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable( GL_DEPTH_TEST );

		//  Initiate the 3D Rendering Context
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 60.0, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 1.0, 1000.0 );

		// RENDER 3D
		game_client->Render3D();
		dialog_controller->Render3D();

		//  Initiate the 2D Rendering Context
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, (GLdouble)WINDOW_WIDTH, (GLdouble)WINDOW_HEIGHT, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable( GL_DEPTH_TEST );

		// RENDER 2D
		game_client->Render2D();
		dialog_controller->Render2D();
		debug_console->Render();

		//  Invalidate any input from this frame
		input_system->Invalidate_Old_Input();

		//  Swap the buffers to display rendered data
		SDL_GL_SwapBuffers();
	}

	//  Shut down systems
	font_controller->Shutdown();
	dialog_controller->Shutdown();

	return 0;
}

void Setup_OpenGL_Defaults( void )
{
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.02f);
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
	gluPerspective( 60.0, (GLdouble)width / (GLdouble)height , 1.0 , 100.0 );

	// Set up Model View
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}