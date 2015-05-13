////////////////////////////////////////////////////////////////////////////////
//
//	Name:			Window.h
//
//	Description:	This file is the basic class for creating
//					a window within SDL. It also controls the
//					specifics of the window (size, ect.) after
//					the creation, and then closes it at the end.
//
//	Created:		December 7th, 2008
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_WINDOW_
#define _DB_WINDOW_

#include <SDL.h>
#include <SDL_opengl.h>

class CWindow
{
	private:
		int		m_nWidth;
		int		m_nHeight;
		int		m_nBPP;
		bool	m_bFullScreen;
		char	m_szTitle[64];

		SDL_Surface* m_pScreen;
		
	public:
		CWindow();
		~CWindow();
		
		bool	DefineWindow(int Width, int Height, int BPP, bool FullScreen, char* Title);
		
		// Accessors
		inline	int		GetHeight()				const	{	return m_nHeight;	}
		inline	int		GetWidth()				const	{	return m_nWidth;	}
};

CWindow::CWindow()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_bFullScreen = false;
}

CWindow::~CWindow()
{
	SDL_Quit();
}

bool CWindow::DefineWindow(int Width, int Height, int BPP, bool FullScreen, char* Title)
{
	//  If you can not initialize SDL to open a video window, return false
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
	
	//  Copy over the member variables for future use
	m_nHeight		= Height;
	m_nWidth		= Width;
	m_nBPP			= BPP;
	m_bFullScreen	= FullScreen;
	strcpy_s(m_szTitle, strlen(Title) + 1, Title);
	
	//  All values are "at least"!	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	//  Set the title of the current window
	SDL_WM_SetCaption(Title, Title);

	//  Create the flags to tell SDL the type of window to create
	int nFlags = 0;
	nFlags |= SDL_OPENGL;
	if(FullScreen == true) nFlags |= SDL_FULLSCREEN;

	//  Create the window
	m_pScreen = SDL_SetVideoMode(Width, Height, BPP, nFlags);
	if (m_pScreen == NULL) return false;
	
	//  Trigger a resize event for OpenGL
	SDL_Event evResizeEvent;
	evResizeEvent.type		= SDL_VIDEORESIZE;
	evResizeEvent.resize.w	= Width;
	evResizeEvent.resize.h	= Height;
	SDL_PushEvent(&evResizeEvent);

	return true;
}

#endif

