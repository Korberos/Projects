#include "Engine.h"

#include <SFML/OpenGL.hpp>

static InputSystem* input_system = InputSystem::Get_Instance();
static TaskScheduler* task_scheduler = TaskScheduler::Get_Instance();
static DialogController* dialog_controller = DialogController::Get_Instance();
static DebugConsole* debug_console = DebugConsole::Get_Instance();
static FontController* font_controller = FontController::Get_Instance();
static TextureController* texture_controller = TextureController::Get_Instance();

static WindowManager window;

static int window_width = 1;
static int window_height = 1;

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
	gluPerspective( 60.0, (GLdouble)width / (GLdouble)height, 1.0, 100.0 );

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
	sf::Event new_event;
	while ( window.PollEvent(new_event) )
	{
		switch ( new_event.type )
		{
		case sf::Event::Closed:
			{
				//  Return false for the quit event
				return false;
			}

			case sf::Event::KeyPressed:
			{	
				// Set the designated key to 1
				input->Set_Key( new_event.key.code , 1 );
				input->Add_Key_To_String( new_event.key.code );
				break;
			}

			case sf::Event::KeyReleased:
			{
				// Set the designated key to 0
				input->Set_Key( new_event.key.code, 0 );
				break;
			}

			case sf::Event::MouseMoved:
			{
				input->Set_Mouse_X( new_event.mouseMove.x );
				input->Set_Mouse_Y( new_event.mouseMove.y );
				break;
			}

			case sf::Event::MouseButtonPressed:
			{
				switch ( new_event.mouseButton.button )
				{
				case sf::Mouse::Left:
					input->Set_Mouse_Button( 0 , 1 );
					break;
				case sf::Mouse::Right:
					input->Set_Mouse_Button( 1 , 1 );
					break;
				case sf::Mouse::Middle:
					input->Set_Mouse_Button( 2 , 1 );
					break;
				}
				break;
			}

			case sf::Event::MouseButtonReleased:
			{
				switch ( new_event.mouseButton.button )
				{
				case sf::Mouse::Left:
					input->Set_Mouse_Button( 0 , 0 );
					break;
				case sf::Mouse::Right:
					input->Set_Mouse_Button( 1 , 0 );
					break;
				case sf::Mouse::Middle:
					input->Set_Mouse_Button( 2 , 0 );
					break;
				default:
					break;
				}
				break;
			}

			case sf::Event::Resized:
			{
				Resize_View( new_event.size.width, new_event.size.height );
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

int InitializeEngine( const int width, const int height, std::string title )
{
	window_width = width;
	window_height = height;

	//  Seed the random number generator
	Seed_Random_Number_Generator();

#ifdef _DEBUG
	title +=  "- DEBUG"; 
#else
	title +=  "- RELEASE"; 
#endif

	//  Define the window. If the window definition fails, return a failure flag.
	if ( !window.Define_Window( window_width , window_height , 32 , false , title.c_str() ) )
	{
		assert( "Failed to define the window." && false );
		return 1;
	}

	// Set up the general information for the rendering of the window through OpenGL
	Setup_OpenGL_Defaults();

	//  Create the Debug Console
	debug_console->Resize( window_width / 3 * 2, window_height / 5 * 2 );
	debug_console->Recolor( 0.125f, 0.125f, 0.125f, 0.95f );

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
	Winsock_Init( 1 );

	// Return success
	return 0;
}

bool ProcessEventsEngine()
{
	if (SDL_ProcessEvents( input_system ) == false) { return false; }
	if (input_system->Get_Key(27)) { return false; }		// 27 : ESCAPE
	
	return true;
}

void InputEngine()
{
	dialog_controller->Input();
	debug_console->Input();
}

void UpdateEngine( const float time_slice )
{
	dialog_controller->Update( time_slice );
	task_scheduler->Update( time_slice );
}

void PreRender3DEngine()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable( GL_DEPTH_TEST );

	//  Initiate the 3D Rendering Context
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, (float)(window_width) / (float)(window_height), 1.0, 1000.0 );
}

void Render3DEngine()
{
	dialog_controller->Render3D();
}

void PreRender2DEngine()
{
	//  Initiate the 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (GLdouble)window_width, (GLdouble)window_height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable( GL_DEPTH_TEST );
}

void Render2DEngine()
{
	dialog_controller->Render2D();
	debug_console->Render();
}

void PostLoopEngine()
{
	//  Invalidate any input from this frame
	input_system->Invalidate_Old_Input();

	//  Swap the buffers to display rendered data
	window.Display();
}

void ShutdownEngine()
{
	font_controller->Shutdown();
	dialog_controller->Shutdown();
}