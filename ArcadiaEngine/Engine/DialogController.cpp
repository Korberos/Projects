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
/*  File:       DialogController.cpp                             */
/*                                                               */
/*  Purpose:    This file contains the primary system in place   */
/*              for loading and managing dialogs in-game. The    */
/*              basic dialog is included which can be derived    */
/*              from to create customized dialogs.               */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#include "DialogController.h"


void DialogHandler::Activate( void )
{
	Active = true;
}

void DialogHandler::Deactivate( void )
{
	Active = false;

	if ( Menu )
	{
		MenuGUI::Delete_Menu_GUI( Menu );
		Menu = NULL;
	}
}

void DialogHandler::Input( void )
{
	FAIL_IF ( !Active ) { return; }

	if ( Menu )
	{
		//  Gather input for all menus and their elements. Return a list of input responses.
		MenuGUIElement::InputResponseListType input_responses;
		Menu->Input( input_responses );

		//  For each input response, check for any events registered to them and trigger the functions if needed
		for ( MenuGUIElement::InputResponseListType::iterator iter = input_responses.begin(); iter != input_responses.end(); ++iter )
		{
			EventMapType::iterator element_iter = EventMap.find( (*iter).first );
			if ( element_iter == EventMap.end() ) { continue; }

			EventListType::iterator interaction_iter = ((*element_iter).second).find( (*iter).second );
			if ( interaction_iter == (*element_iter).second.end() ) { continue; }

			(*interaction_iter).second->Call( this );
		}
	}
}

void DialogHandler::Update( float time_slice )
{
	FAIL_IF ( !Active ) { return; }

	if ( Menu )
	{
		Menu->Update( time_slice );
	}
}

void DialogHandler::Render2D( void )
{
	FAIL_IF ( !Active ) { return; }

	if ( Menu )
	{
		Menu->Render();
	}
}

void DialogHandler::Render3D( void )
{
	FAIL_IF ( !Active ) { return; }
}