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
/*  File:       InputSystem.cpp                                  */
/*                                                               */
/*  Purpose:    This file contains the primary system in place   */
/*              for managing input from the player. The system   */
/*              requires input pertaining to the basic input     */
/*              taken from the system messages.                  */
/*                                                               */
/*  Created:    01/20/2009                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#include "InputSystem.h"

InputSystem::InputSystem()
{
	Keyboard.clear();
	
	for (unsigned int i = 0; i < 3; ++i)
		MouseButton[i] = 0;
}

InputSystem::~InputSystem()
{
}

InputSystem* InputSystem::Get_Instance( void )
{
	static InputSystem INSTANCE;
	return &INSTANCE;
}

void InputSystem::Invalidate_Old_Input( void )
{
	//  Loop through the values in the keyboard array and
	//  change any 1 to a 2, indicating that the key has
	//  been held down and is not in the process of being 
	//  pushed down this frame.
	for (KeyboardType::iterator iter = Keyboard.begin(); iter != Keyboard.end(); ++iter)
	{
		if ((*iter).second == 1) (*iter).second = 2;
	}

	//  Loop through any values in the mouse array and
	//  change any 1 to a 2, indicating that the button has
	//  been held down and is not in the process of being
	//  pushed down this frame
	for (unsigned int i = 0; i < 3; ++i)
		if (MouseButton[i] == 1)
			MouseButton[i] = 2;

	//  Clear the Text String for the next group of keys to
	//  be taken in by the message system
	TextString = "";
}

void InputSystem::Add_Key_To_String( InputKey::Key k )
{
	if ( k == InputKey::Unknown ) return;

	bool preShift = Get_Key(InputKey::LShift) || Get_Key(InputKey::RShift);

	//  If the character is the spacebar symbol, add the symbol accordingly
	if ( k == InputKey::Space ) { TextString += ' '; return; }

	//  If the character is a letter, add the letter accordingly (lowercase unless shift is pressed)
	if ( k >= InputKey::A && k <= InputKey::Z )
	{
		TextString += (preShift ? 'A' : 'a') + (k - InputKey::A);
		return;
	}

	//  If the character is a number, add the number or symbol accordingly (numbers unless shift is pressed)
	if ( k >= InputKey::Num0 && k <= InputKey::Num9 )
	{
		switch (k)
		{
		case InputKey::Num0:		TextString += (preShift ? ')' : '0');	return;
		case InputKey::Num1:		TextString += (preShift ? '!' : '1');	return;
		case InputKey::Num2:		TextString += (preShift ? '@' : '2');	return;
		case InputKey::Num3:		TextString += (preShift ? '#' : '3');	return;
		case InputKey::Num4:		TextString += (preShift ? '$' : '4');	return;
		case InputKey::Num5:		TextString += (preShift ? '%' : '5');	return;
		case InputKey::Num6:		TextString += (preShift ? '^' : '6');	return;
		case InputKey::Num7:		TextString += (preShift ? '&' : '7');	return;
		case InputKey::Num8:		TextString += (preShift ? '*' : '8');	return;
		case InputKey::Num9:		TextString += (preShift ? '(' : '9');	return;
		}
	}

	//  If the character is a symbol, add the symbol accordingly (primary unless shift is pressed)
	switch (k)
	{
	case InputKey::LBracket:		TextString += (preShift ? '{' : '[');	return;
	case InputKey::RBracket:		TextString += (preShift ? '}' : ']');	return;
	case InputKey::SemiColon:		TextString += (preShift ? ':' : ';');	return;
	case InputKey::Comma:			TextString += (preShift ? '<' : ',');	return;
	case InputKey::Period:			TextString += (preShift ? '>' : '.');	return;
	case InputKey::Quote:			TextString += (preShift ? '"' : '\'');	return;
	case InputKey::Slash:			TextString += (preShift ? '?' : '/');	return;
	case InputKey::BackSlash:		TextString += (preShift ? '|' : '\\');	return;
	case InputKey::Tilde:			TextString += (preShift ? '~' : '`');	return;
	case InputKey::Equal:			TextString += (preShift ? '+' : '=');	return;
	case InputKey::Dash:				TextString += (preShift ? '_' : '-');	return;
	}
}
