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

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

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

#endif

