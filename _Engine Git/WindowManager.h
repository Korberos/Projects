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
/*  File:       Windowmanager.h                                  */
/*                                                               */
/*  Purpose:    This file contains a basic class for opening     */
/*              and managing a window within the engine. The     */
/*              specifics of the window, such as width and       */
/*              height, can be easily altered, and shutdown      */
/*              is made simple.                                  */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#ifndef _WINDOW_MANAGER_H_
#define _WINDOW_MANAGER_H_

#include <SFML\Graphics.hpp>

class WindowManager
{
	//~  Member Functions
	private:
		
	public:
		WindowManager();
		~WindowManager();
		
		bool Define_Window(int width, int height, int bpp, bool full_screen, const char* title);

		bool PollEvent(sf::Event& pollEvent)	{ return m_Window.pollEvent(pollEvent); }
		void Display()									{ m_Window.display(); }
		
		// Accessors
		inline int				Get_Height( void )		const	{	return m_nHeight;	}
		inline int				Get_Width( void )		const	{	return m_nWidth;	}

		
	//~ Member Variables
	private:
		int					m_nWidth;
		int					m_nHeight;
		int					m_nBitsPerPixel;
		bool					m_bFullScreen;
		std::string			m_zTitle;
		sf::RenderWindow	m_Window;

	public:

};

#endif // _WINDOW_MANAGER_H_