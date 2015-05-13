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
/*  File:       DebugConsole.h                                   */
/*                                                               */
/*  Purpose:    This file contains the system for displaying     */
/*              and managing a self-contained console system     */
/*              which can be used to create commands that        */
/*              produce special functionality in games.          */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#ifndef _DEBUG_CONSOLE_
#define _DEBUG_CONSOLE_

#include <vector>
#include "DialogController.h"

struct Font;

#define MAX_STRINGS 100

class DebugConsole
{
public:
	struct ActionTrigger;

	static DebugConsole* Get_Instance( void )
	{
		static DebugConsole INSTANCE;
		return &INSTANCE;
	}


	void Input( void );
	void Render( void );

	void Resize( int console_width, int console_height );
	void Recolor( float R, float G, float B, float A );
	void Set_Bitmap_Font( const char* font );
	void Execute_String( std::string string );
	void Add_String( const char* text, int wrap_lines = 1 );
	void Add_Action_Trigger( ActionTrigger action_trigger );

	bool Get_Active( void ) { return Active; }
	void Set_Active( bool active ) { Active = active; }

	//  Action Triggers - Commands that link to functions. If the command is used in the console, the remained of the string is passed to the function
	struct ActionTrigger
	{
	public:
		ActionTrigger( std::string command, const char* (*action)( std::string )) :
			Command( command ),
			Action( action )
		{}

		std::string Command;
		const char* (*Action)( std::string );

		const char* Trigger_Action( std::string arguments )
		{
			return Action( arguments );
		}
	};

private:
	DebugConsole() : 
		ConsoleWidth( 0 ),
		ConsoleHeight( 0 ),
		//BackgroundColor(),
		StringDisplayCount( 5 ),
		Active( false ),
		ConsoleFont( NULL ),
		ConsoleStrings(),
		TypingString(""),
		ActionTriggerList()
	{
		BackgroundColor[0] = 0.0f;
		BackgroundColor[1] = 0.0f;
		BackgroundColor[2] = 0.0f;
		BackgroundColor[3] = 1.0f;
	}

	~DebugConsole()
	{
	}
	
	int ConsoleWidth;
	int ConsoleHeight;
	float BackgroundColor[4];
	unsigned int StringDisplayCount;

	bool Active;
	Font* ConsoleFont;
	std::vector< std::string > ConsoleStrings;
	std::string TypingString;

	typedef std::vector<ActionTrigger> ActionTriggerListType;
	ActionTriggerListType ActionTriggerList;
};

#endif