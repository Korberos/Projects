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
/*  File:       SDL_Window.h                                     */
/*                                                               */
/*  Purpose:    This file contains a basic class for opening     */
/*              and managing a window within SDL. The specifics  */
/*              of the window, such as width and height, can be  */
/*              easily altered, and shutdown is made simple.     */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#ifndef _SDL_WINDOW_
#define _SDL_WINDOW_

#include <SDL.h>
#include <SDL_opengl.h>

class SDL_Window_Manager
{
	//~  Member Functions
	private:
		
	public:
		SDL_Window_Manager();
		~SDL_Window_Manager();
		
		bool	Define_Window( int width, int height, int bpp, bool full_screen, const char* title );
		
		// Accessors
		inline	int			Get_Height( void )		const	{	return m_nHeight;	}
		inline	int			Get_Width( void )		const	{	return m_nWidth;	}
		inline	SDL_Window*	Get_Window( void )				{	return m_pScreen;	}

		
	//~ Member Variables
	private:
		int		m_nWidth;
		int		m_nHeight;
		int		m_nBPP;
		bool	m_bFullScreen;
		char	m_szTitle[64];

		SDL_Window* m_pScreen;

	public:

};

SDL_Window_Manager::SDL_Window_Manager()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_bFullScreen = false;
}

SDL_Window_Manager::~SDL_Window_Manager()
{
	SDL_Quit();
}

bool SDL_Window_Manager::Define_Window( int width, int height, int bpp, bool full_screen, const char* title )
{
	//  If you can not initialize SDL to open a video window, return false
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;
	
	//  Copy over the member variables for future use
	m_nWidth		= width;
	m_nHeight		= height;
	m_nBPP			= bpp;
	m_bFullScreen	= full_screen;
	strcpy_s( m_szTitle , strlen( title ) + 1 , title );
	
	//  All values are "at least"!	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE , 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE , 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE , 5 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE , 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE , 32 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER , 1 );

	//  Create the flags to tell SDL the type of window to create
	int nFlags = 0;
	if( full_screen == true ) nFlags |= SDL_WINDOW_FULLSCREEN;
	nFlags |= SDL_WINDOW_OPENGL;

	m_pScreen = SDL_CreateWindow(m_szTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_nWidth, m_nHeight, nFlags);
	
	//  Trigger a resize event for OpenGL
	SDL_Event evResizeEvent;
	evResizeEvent.type				= SDL_WINDOWEVENT_RESIZED;
	evResizeEvent.window.data1		= width;
	evResizeEvent.window.data2		= height;
	SDL_PushEvent( &evResizeEvent );

	return true;
}

#endif