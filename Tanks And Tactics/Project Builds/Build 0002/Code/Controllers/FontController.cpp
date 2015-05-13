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
#include "FontController.h"

CFontController* CFontController::GetInstance(void)
{
	static CFontController INSTANCE;
	return &INSTANCE;
}

unsigned int CFontController::LoadBitmapFont(char *szFontFile, unsigned int W, unsigned int H)
{
	unsigned int Texture = CTextureController::GetInstance()->LoadTexture(szFontFile);
	if (Texture == INDEX_NOIMAGE) return Texture;

	BitmapFont BF;
	BF.TextureID	= Texture;
	BF.Width		= W;
	BF.Height		= H;
	BF.CharWidth	= BF.Width / 16;
	BF.CharHeight	= BF.Height / 16;
	FontList.push_back(BF);

	return (unsigned int)(FontList.size() - 1);
}

void CFontController::RenderText(unsigned int Font, unsigned int X, unsigned int Y, char* Text)
{
	if (Font > (unsigned int)(FontList.size() - 1)) return;

	static GLdouble Percent_Width	= 1.0 / 16.0;
	static GLdouble Percent_Height	= 1.0 / 16.0;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, CTextureController::GetInstance()->GetTexture(FontList[Font].TextureID));

	glBegin(GL_QUADS);
		for (unsigned int i = 0; i < (unsigned int)strlen(Text); ++i)
		{
			// Top Left
			glTexCoord2d((GLdouble)(Text[i] % 16) * Percent_Width, (GLdouble)(Text[i] / 16) * Percent_Height);
			glVertex2i(X + (i * FontList[Font].CharWidth), Y);

			// Top Right
			glTexCoord2d((GLdouble)(Text[i] % 16) * Percent_Width + Percent_Width, (GLdouble)(Text[i] / 16) * Percent_Height);
			glVertex2i(X + (i * FontList[Font].CharWidth) + FontList[Font].CharWidth, Y);

			// Bottom Right
			glTexCoord2d((GLdouble)(Text[i] % 16) * Percent_Width + Percent_Width, (GLdouble)(Text[i] / 16) * Percent_Height + Percent_Height);
			glVertex2i(X + (i * FontList[Font].CharWidth) + FontList[Font].CharWidth, Y + FontList[Font].CharHeight);

			// Bottom Left
			glTexCoord2d((GLdouble)(Text[i] % 16) * Percent_Width, (GLdouble)(Text[i] / 16) * Percent_Height + Percent_Height);
			glVertex2i(X + (i * FontList[Font].CharWidth), Y + FontList[Font].CharHeight);
		}
	glEnd();
}