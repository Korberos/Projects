////////////////////////////////////////////////////////////////////////////////
//
//	Name:			TextureController.cpp
//
//	Description:	This file contains a structure that keeps 
//					track of and allows access to texture 
//					allocations. It acts as a single point of 
//					access to all textures and texture functions
//
//	Created:		January 2nd, 2009
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "TextureController.h"

#include "./../Utilities/SDL_Image/SDL_image.h"
#include <SDL_OpenGL.h>
#include "./../Utilities/SDL_Surface_to_OpenGL.h"

CTextureController::CTextureController(void)
{
}

CTextureController::~CTextureController(void)
{
	while (TextureList.size()) ReleaseTexture(0);
}

CTextureController* CTextureController::GetInstance(void)
{
	static CTextureController INSTANCE;
	return &INSTANCE;
}

unsigned int CTextureController::LoadTexture(char* TextureFile)
{
	// Attempt to load the texture file. If it can not be loaded, return.
	SDL_Surface* NewSurface = IMG_Load(TextureFile);
	if (NewSurface == NULL)
	{
		SDL_FreeSurface(NewSurface);
		return INDEX_NOIMAGE;
	}
	unsigned int Image = convert_surface(NewSurface);
	SDL_FreeSurface(NewSurface);

	// If you can find a texture that has the same filename, return the ID
	for (unsigned int i = 0; i < (unsigned int)TextureList.size(); ++i)
		if (strcmp(TextureList[i].Filename, TextureFile) == 0)
			return i;

	// If you can find a texture that has been cleared, define it and return the ID
	for (unsigned int i = 0; i < (unsigned int)TextureList.size(); ++i)
	{
		if (TextureList[i].References == 0)
		{
			TextureList[i].Image = Image;
			strcpy_s(TextureList[i].Filename, strlen(TextureFile) + 1, TextureFile);
			TextureList[i].References = 1;
			return i;
		}
	}

	// If no cleared texture has been found, create a new one
	Texture T;
	T.Image = Image;
	strcpy_s(T.Filename, strlen(TextureFile) + 1, TextureFile);
	T.References = 1;
	TextureList.push_back(T);
	return (unsigned int)(TextureList.size() - 1);
}

bool CTextureController::ReleaseTexture(unsigned int TextureIndex)
{
	if (TextureIndex > (unsigned int)TextureList.size() - 1) return false;

	Texture* BACK = &TextureList[TextureList.size() - 1];
	Texture* GONE = &TextureList[TextureIndex];

	memcpy(GONE, BACK, sizeof(Texture));
	TextureList.pop_back();
	return true;
}

void CTextureController::DrawTexture(unsigned int TextureID, unsigned int X, unsigned int Y, unsigned int W, unsigned int H)
{
	if (TextureID < 0)					return;
	if (TextureID > TextureList.size())	return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureList[TextureID].Image);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(X, Y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(X + W, Y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(X + W, Y + H);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(X, Y + H);
	glEnd();
}