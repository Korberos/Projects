////////////////////////////////////////////////////////////////////////////////
//
//	Name:			SDL_Window.h
//
//	Description:	This file is the basic class for creating
//					a window within SDL. It also controls the
//					specifics of the window (size, ect.) after
//					the creation, and then closes it at the end.
//
//	Created:		December 7th, 2008
//
//	Last Modified:	April 18th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _SDL_WINDOW_
#define _SDL_WINDOW_

#include <SDL.h>
#include <SDL_opengl.h>

class SDL_Window
{
	//~  Member Functions
	private:
		
	public:
		SDL_Window();
		~SDL_Window();
		
		bool	Define_Window( int width, int height, int bpp, bool full_screen, const char* title );
		
		// Accessors
		inline	int		Get_Height( void )		const	{	return m_nHeight;	}
		inline	int		Get_Width( void )		const	{	return m_nWidth;	}

		
	//~ Member Variables
	private:
		int		m_nWidth;
		int		m_nHeight;
		int		m_nBPP;
		bool	m_bFullScreen;
		char	m_szTitle[64];

		SDL_Surface* m_pScreen;

	public:

};

SDL_Window::SDL_Window()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_bFullScreen = false;
}

SDL_Window::~SDL_Window()
{
	SDL_Quit();
}

bool SDL_Window::Define_Window( int width, int height, int bpp, bool full_screen, const char* title )
{
	//  If you can not initialize SDL to open a video window, return false
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;
	
	//  Copy over the member variables for future use
	m_nHeight		= height;
	m_nWidth		= width;
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
	
	//  Set the title of the current window
	SDL_WM_SetCaption( title, title );

	//  Create the flags to tell SDL the type of window to create
	int nFlags = 0;
	nFlags |= SDL_OPENGL;
	if( full_screen == true ) nFlags |= SDL_FULLSCREEN;

	//  Create the window
	m_pScreen = SDL_SetVideoMode( width , height , bpp , nFlags );
	if ( m_pScreen == NULL ) return false;
	
	//  Trigger a resize event for OpenGL
	SDL_Event evResizeEvent;
	evResizeEvent.type		= SDL_VIDEORESIZE;
	evResizeEvent.resize.w	= width;
	evResizeEvent.resize.h	= height;
	SDL_PushEvent( &evResizeEvent );

	return true;
}

#endif