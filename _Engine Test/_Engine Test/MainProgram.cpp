#include "MainProgram.h"

#include "FontController.h"
#include "InputSystem.h"
#include "DebugTools.h"
#include "TextureController.h"
#include "DebugConsole.h"
#include <fstream>
#include <string>

InputSystem* input_system = InputSystem::Get_Instance();
FontController* font_system = FontController::Get_Instance();
DebugConsole* debug_console = DebugConsole::Get_Instance();

MainProgram::MainProgram()													{}

const bool MainProgram::Initialize()
{
	//  Load all fonts associated with the Engine Test
	FontController::Get_Instance()->Set_Font_Folder( "Assets/Fonts/" );
	FontController::Get_Instance()->Load_Font( "Arial-12" );
	FontController::Get_Instance()->Load_Font( "Arial-12-White" );
	FontController::Get_Instance()->Load_Font( "DialogueFont" );

	//  Set up the debug console with a font
	debug_console->Set_Bitmap_Font( "Arial-12-White" );

	return true;
}

const bool MainProgram::Input( const float time_slice )
{
	return true;
}

const bool MainProgram::Update( const float time_slice )			{ return true; }
const bool MainProgram::Render3D( void )								{ return true; }

const bool MainProgram::Render2D( void )
{
	return true;
}

const bool MainProgram::Shutdown()
{
	return true;
}