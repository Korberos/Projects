/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       DebugConsole.cpp                                 */
/*                                                               */
/*  Purpose:    This file contains the system for displaying     */
/*              and managing a self-contained console system     */
/*              which can be used to create commands that        */
/*              produce special functionality in games.          */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#include "DebugConsole.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include "InputSystem.h"
#include "DebugTools.h"
#include "FontController.h"

#define DEBUG_CONSOLE_TOGGLE_KEY	SDLK_BACKQUOTE
#define MAX_LINES_PER_ADDITION		3

void DebugConsole::Input( void )
{
	InputSystem* input_system = InputSystem::Get_Instance();

	//  If we hit the toggle key, toggle the console
	if ( input_system->Get_Key( DEBUG_CONSOLE_TOGGLE_KEY ) == 1 )
	{
		Set_Active( !Get_Active() );
		return;
	}

	//  If at this point we are not active, do not take any more input
	if ( !Get_Active() ) { return; }

	TypingString += input_system->Get_Text_String();
	if (input_system->Get_Key(SDLK_BACKSPACE) == 1 && !TypingString.empty()) TypingString.erase(--TypingString.end());

	if ( input_system->Get_Key( SDLK_RETURN ) == 1 && !TypingString.empty() )
	{
		Execute_String( TypingString );
		TypingString.clear();
	}
}


void DebugConsole::Render( void )
{
	if ( !Get_Active() ) return;

	//  Display the background square, a dark gray box
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable( GL_TEXTURE_2D );
	glColor4f( BackgroundColor[0], BackgroundColor[1], BackgroundColor[2], BackgroundColor[3] );
	glBegin( GL_QUADS );
		glVertex2i( 0,				0 );
		glVertex2i( ConsoleWidth,	0 );
		glVertex2i( ConsoleWidth,	ConsoleHeight );
		glVertex2i( 0,				ConsoleHeight );
	glEnd();

	//  If we have no font, do not attempt to display the strings
	FAIL_IF ( ConsoleFont == NULL ) { return; }

	//  Render the last X strings, starting from the bottom (X being the number of strings we can display)
	for ( unsigned int i = 0; i < StringDisplayCount && i < ConsoleStrings.size();  ++i )
	{
		ConsoleFont->Render_Text( ConsoleStrings[ ConsoleStrings.size() - 1 - i ].c_str(), 4, ConsoleHeight - ( (4 + ConsoleFont->Get_Font_Height() ) * ( i + 2 ) ), false, false );
	}

	ConsoleFont->Render_Text( TypingString.c_str(), 4, ConsoleHeight - (4 + ConsoleFont->Get_Font_Height() ), false, false );
}


void DebugConsole::Resize( int console_width, int console_height )
{
	FAIL_IF ( console_width < 1 || console_height < 1 ) { return; }	

	ConsoleWidth = console_width;
	ConsoleHeight = console_height;
}


void DebugConsole::Recolor( float R, float G, float B, float A )
{
	BackgroundColor[0] = R;
	BackgroundColor[1] = G;
	BackgroundColor[2] = B;
	BackgroundColor[3] = A;
}


void DebugConsole::Set_Bitmap_Font( const char* font )
{
	ConsoleFont = FontController::Get_Instance()->Get_Font( font );
	ENFORCED_IF( ConsoleFont != NULL )
	{
		StringDisplayCount = ConsoleHeight / (4 + ConsoleFont->Get_Font_Height() ) - 1;
	}
}


void DebugConsole::Execute_String( std::string string )
{
	std::string::size_type first_space = string.find_first_of(" ");
	std::string command( string );
	std::string arguments;

	if ( first_space != (unsigned int)(-1) )
	{
		while ( command.length() > first_space ) command.erase( --command.end() );
		arguments = std::string( string.c_str() + string.find_first_of(" ") + 1 );
	}

	for ( ActionTriggerListType::iterator iter = ActionTriggerList.begin(); iter != ActionTriggerList.end(); ++iter )
	{
		if ( (*iter).Command.compare( command ) == 0 )
		{
			Add_String( (*iter).Trigger_Action( arguments ) );
			return;
		}
	}
}


void DebugConsole::Add_String( const char* text, int wrap_lines )
{
	if ( wrap_lines > MAX_LINES_PER_ADDITION ) return;

	if ( ConsoleFont->Get_Text_Width( text ) > (unsigned int)(ConsoleWidth - 8) )
	{
		unsigned int cut_spot = ConsoleFont->Character_Count_Before_Passing_Width( text, ConsoleWidth, true );
		std::string shortened_string( text );
		while ( shortened_string.length() > cut_spot ) { shortened_string.erase( --shortened_string.end() ); }
		Add_String( shortened_string.c_str() );
		Add_String( text + cut_spot, ++wrap_lines );
		return;
	}

	ConsoleStrings.push_back( std::string( text ) );
	while ( ConsoleStrings.size() > MAX_STRINGS )
	{
		ConsoleStrings.erase( ConsoleStrings.begin() );
	}
}


void DebugConsole::Add_Action_Trigger( ActionTrigger action_trigger )
{
	FAIL_IF ( action_trigger.Command.empty() ) { return; }

	ActionTriggerList.push_back( action_trigger );
}