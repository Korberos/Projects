////////////////////////////////////////////////////////////////////////////////
//
//	Name:			FontController.h
//
//	Description:	This file contains the CFontController class,
//					which enabled the use of Bitmap Fonts within
//					the program. The controller keeps track of any
//					loaded Bitmap Font and renders the font to
//					screen when called to do so
//
//	Created:		January 21st, 2009
//
//	Last Modified:	January 21st, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_FONTCONTROLLER_
#define _DB_FONTCONTROLLER_

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "./TextureController.h"

class CFontController
{
private:
	CFontController()			{}
	~CFontController()			{}

	struct BitmapFont
	{
	public:
		unsigned int	TextureID;
		unsigned int	Width;
		unsigned int	Height;
		unsigned int	CharWidth;
		unsigned int	CharHeight;
	};

	std::vector<BitmapFont> FontList;

public:
	static CFontController*		GetInstance(void);
	int							LoadBitmapFont(char* szFontFile, unsigned int W, unsigned int H);
	void						RenderText(unsigned int Font, unsigned int X, unsigned int Y, char* Text);
};

#endif