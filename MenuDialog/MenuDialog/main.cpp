#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768

#pragma comment(lib, "sdlmain.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "SDL_mixer.lib")

#include "InputSystem.h"
#include "SDL_Window.h"
#include "MenuDialog.h"
#include "SDL_mixer.h"

MenuDialog*		test_dialog;
Mix_Music*		music;

bool SDL_ProcessEvents( InputSystem* Input );
void Resize_View( unsigned int Width, unsigned int Height );

void Set_Text_Block( void );

int main( int argc , char* argv[] )
{
	//  Create access to the InputSystem and an SDL_Window
	InputSystem*	input = InputSystem::Get_Instance();
	SDL_Window		window;

	//  Define the window. If the window definition fails, return a failure flag.
	if ( !window.Define_Window( WINDOW_WIDTH , WINDOW_HEIGHT , 32 , false , "MenuDialog System (build 0001)" ) ) return -1;

	// Initialize SDL_Mixer
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
	{
		char* error = SDL_GetError();
		return -2;
	}
	music = Mix_LoadMUS( "Sounds/Mario - Star.wav" ); 

	//  Create a Menu Dialog. If the dialog creation fails, return a failure flag.
	test_dialog = MenuDialog::CreateMenuDialog( "TestMenuDialog" );
	if (test_dialog == NULL) return -3;

	test_dialog->Load_Bitmap_Font("Arial");

	MenuElement_CheckBox* check_box_test = dynamic_cast<MenuElement_CheckBox*>(test_dialog->Get_Element("CheckBoxTest"));
	if (check_box_test)
	{
		check_box_test->Set_Check_Event(&Set_Text_Block);
	}

	MenuElement_TextureButton* texture_button_test = dynamic_cast<MenuElement_TextureButton*>(test_dialog->Get_Element("TextureButtonTest"));
	if (texture_button_test)
	{
		texture_button_test->Set_Click_Event(&Set_Text_Block);
	}

	MenuElement_TextButton* text_button_test = dynamic_cast<MenuElement_TextButton*>(test_dialog->Get_Element("TextButtonTest"));
	if (text_button_test != NULL)
	{
		text_button_test->Set_Text("The quick red fox jumped over the lazy brown dog. Testing!");
		text_button_test->Set_Click_Event(&Set_Text_Block);
	}

	MenuElement_DropDown* drop_down_test = dynamic_cast<MenuElement_DropDown*>(test_dialog->Get_Element("DropDownTest"));
	if (drop_down_test != NULL)
	{
		drop_down_test->Add_Item("TestString1");
		drop_down_test->Add_Item("TestString2");
		drop_down_test->Add_Item("TestString3");
		drop_down_test->Set_Selection_Event(&Set_Text_Block);
	}
	
	MenuElement_TextBlock* text_block_test = dynamic_cast<MenuElement_TextBlock*>(test_dialog->Get_Element("TextBlockTest"));
	if (text_block_test != NULL)
	{
		text_block_test->Set_Text("This is a test of the string parsing functionality of the text block menu element. These words should parse down to multiple strings that render in lines downward.");
	}

	MenuElement_ListBox* list_box_test = dynamic_cast<MenuElement_ListBox*>(test_dialog->Get_Element("ListBoxTest"));
	if (list_box_test != NULL)
	{
		list_box_test->Add_Item("TestString2");
		list_box_test->Add_Item("TestString3");
		list_box_test->Add_Item("TestString4");
		list_box_test->Add_Item("TestString5");
		list_box_test->Add_Item("TestString6");
		list_box_test->Add_Item("TestString7");
		list_box_test->Add_Item("TestString8");
		list_box_test->Add_Item("TestString9");
		list_box_test->Add_Item("TestString10");
		list_box_test->Set_Selection_Event(&Set_Text_Block);
	}
	
	MenuElement_EditBox* edit_box_test = dynamic_cast<MenuElement_EditBox*>(test_dialog->Get_Element("EditBoxTest"));
	if (edit_box_test != NULL)
	{
		edit_box_test->Set_Text("Testing");
	}


	// Set up the general information for the rendering of the window through OpenGL
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.02f);

	//  Process any events from the Operating System
	while (SDL_ProcessEvents( input ))
	{
		if (input->Get_Key(27)) break;		// 27 : ESCAPE

		// Get the time slice
		static float time_slice;
		static int elapsed_time[2];
		elapsed_time[0] = SDL_GetTicks();
		time_slice = std::max(std::min((float)(elapsed_time[0] - elapsed_time[1]) / 1000.0f, 1.0f), 0.1f);
		elapsed_time[1] = elapsed_time[0];

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Initiate the 2D Rendering Context
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, (GLdouble)WINDOW_WIDTH, (GLdouble)WINDOW_HEIGHT, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		test_dialog->Input( input );
		test_dialog->Update( time_slice );
		test_dialog->Render();

		SDL_GL_SwapBuffers();
		input->Invalidate_Old_Input();
	}

	Mix_FreeMusic( music ); 
	Mix_CloseAudio();

	return 0;
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
	gluPerspective( 60.0 , (GLdouble)width / (GLdouble)height , 1.0 , 100.0 );

	// Set up Model View
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void Set_Text_Block( void )
{
	static int new_value = 0;
	new_value++;
	char value_string[4];
	_itoa_s(new_value, value_string, 4,  10);

	MenuElement_TextBlock* text_block_test = (MenuElement_TextBlock*)test_dialog->Get_Element("TextBlockTest");
	if (text_block_test != NULL)
	{
		text_block_test->Set_Text(std::string(value_string));
	}

	// Simple music playing
	if( Mix_PlayingMusic() == 0 )
	{
		Mix_PlayMusic( music, -1 );
	}
	else
	{
		if (Mix_PausedMusic())
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PauseMusic();
		}
	}
}