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

#include <string>
#include <iostream>

#include "GL/GLee.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

using std::string;

class CWindow
{
	private:
		int		m_nWidth;
		int		m_nHeight;
		int		m_nBPP;
		bool	m_bFullscreen;
		char	m_szTitle[64];
		
	public:
		CWindow();
		~CWindow();

		SDL_Surface* m_Screen;
		
		bool	DefineWindow(int Width, int Height, int BPP, bool FullScreen, char* Title);
		void	SetSize(int Width, int Height);
		
		inline	int		GetHeight()		{	return m_nHeight;	}
		inline	int		GetWidth()		{	return m_nWidth;	}
};

#endif

