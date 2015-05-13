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
/*  File:       SDLSimpleWindow.cpp                                     */
/*                                                               */
/*  Purpose:    This file contains a basic class for opening     */
/*              and managing a window within SDL. The specifics  */
/*              of the window, such as width and height, can be  */
/*              easily altered, and shutdown is made simple.     */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#include "WindowManager.h"

#include <SFML/OpenGL.hpp>
#include "DebugTools.h"

WindowManager::WindowManager()
{
	m_nWidth = 1;
	m_nHeight = 1;
	m_nBitsPerPixel = 0;
	m_bFullScreen = false;
	m_zTitle = "NEW WINDOW";
	m_Window.create(sf::VideoMode(m_nWidth, m_nHeight), m_zTitle);
}

WindowManager::~WindowManager()
{
	m_Window.close();
}

bool WindowManager::Define_Window( int width, int height, int bpp, bool full_screen, const char* title )
{
	// change the size of the window
	m_Window.setSize(sf::Vector2u(width, height));

	// change the title of the window
	m_Window.setTitle(title);
	
	//  Copy over the member variables for future use
	m_nHeight			= height;
	m_nWidth				= width;
	m_nBitsPerPixel	= bpp;
	m_bFullScreen		= full_screen;
	m_zTitle				= std::string(title);

	/*
	//  All values are "at least"!	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE , 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE , 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE , 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE , 8 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE , 32 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE , 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER , 1 );

	//  Create the flags to tell SDL the type of window to create
	int nFlags = 0;
	nFlags |= SDL_WINDOW_RESIZABLE;
	nFlags |= SDL_WINDOW_OPENGL;
	if( full_screen == true ) nFlags |= SDL_WINDOW_FULLSCREEN;

	//  Create the window
	m_Window = SDL_CreateWindow( m_zTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, nFlags );
	FAIL_IF ( m_Window == NULL ) return false;

	SDL_GL_CreateContext(m_Window);
	
	//  Trigger a resize event for OpenGL
	SDL_Event evResizeEvent;
	evResizeEvent.type			= SDL_WINDOWEVENT_RESIZED;
	evResizeEvent.window.data1	= width;
	evResizeEvent.window.data2	= height;
	SDL_PushEvent( &evResizeEvent );
	*/
	return true;
}