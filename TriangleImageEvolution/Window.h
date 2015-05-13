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
//	Last Modified:	December 7th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_WINDOW_
#define _DB_WINDOW_

#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "sdlmain.lib")

#include "OpenGL/GLee.h"
#include <SDL.h>
#include <SDL_opengl.h>

#include <string>
#include <iostream>

using std::string;

class CWindow
{
	private:
		int				m_nWidth;
		int				m_nHeight;
		int				m_nBPP;
		bool			m_bFullscreen;
		char*			m_szTitle;
		SDL_Surface*	m_Screen;
		
	public:
		CWindow();
		~CWindow();

		
		bool	DefineWindow(const int Width, const int Height, const int BPP, const bool FullScreen, char* Title);
		void	Undefine(void);
		void	SetSize(int Width, int Height);
		
		inline	int				GetHeight()		{	return m_nHeight;	}
		inline	int				GetWidth()		{	return m_nWidth;	}
		inline	SDL_Surface*	GetScreen()		{	return m_Screen;	}
};

CWindow::CWindow()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_bFullscreen = false;
}
	
CWindow::~CWindow()
{
	SDL_Quit();
}
	
bool CWindow::DefineWindow(const int Width, const int Height, const int BPP, const bool FullScreen, char* Title)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)  return false;
	
	//Copy the values in case we need them
	m_nHeight		= Height;
	m_nWidth		= Width;
	m_szTitle		= Title;
	m_bFullscreen	= FullScreen;
	m_nBPP			= BPP;
	
	//all values are "at least"!	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Set the title
	SDL_WM_SetCaption(Title, Title);

	// Flags tell SDL about the type of window we are creating
	int nFlags = SDL_DOUBLEBUF | SDL_SWSURFACE;
	
	if(FullScreen == true) nFlags |= SDL_FULLSCREEN;

	// Create the window
	m_Screen = SDL_SetVideoMode(Width, Height, BPP, nFlags);
	
	if(m_Screen == 0) return false;
	
	//SDL doesn't trigger off a ResizeEvent at startup, but as we need this for OpenGL, we do this ourself
	SDL_Event evResizeEvent;
	evResizeEvent.type		= SDL_VIDEORESIZE;
	evResizeEvent.resize.w	= Width;
	evResizeEvent.resize.h	= Height;
	
	SDL_PushEvent(&evResizeEvent);

	return true;
}

void CWindow::Undefine(void)
{
	SDL_FreeSurface(m_Screen);
}

void CWindow::SetSize(int Width, int Height)
{
	m_nHeight	= Height;
	m_nWidth	= Width;
}


#endif