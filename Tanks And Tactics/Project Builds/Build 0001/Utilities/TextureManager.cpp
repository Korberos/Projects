////////////////////////////////////////////////////////////////////////////////
//
//	Name:			TextureManager.cpp
//
//	Description:	This file contains a structure that keeps 
//					track of and allows access to texture 
//					allocations. It acts as a single point of 
//					access to all textures and texture functions
//
//	Created:		January 2nd, 2009
//
//	Last Modified:	January 2nd, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "TextureManager.h"

#include "./../SDL_Image/SDL_image.h"
#include <SDL/SDL_OpenGL.h>
#include "SDL_Surface_to_OpenGL.h"

#define INDEX_NOIMAGE 9999

CTextureManager::CTextureManager(void)
{
}

CTextureManager::~CTextureManager(void)
{
	while (TextureList.size()) ReleaseTexture(0);
}

CTextureManager* CTextureManager::GetInstance(void)
{
	static CTextureManager INSTANCE;
	return &INSTANCE;
}

unsigned int CTextureManager::LoadTexture(char* TextureFile)
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

	// If you can find a texture that has been cleared, 
	for (unsigned int i = 0; i < (unsigned int)TextureList.size(); i++)
	{
		if (TextureList[i].References == 0)
		{
			TextureList[i].Image = Image;
			for (unsigned int j = 0; TextureFile[j] != 0; j++) TextureList[i].Filename[j] = TextureFile[j];
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

bool CTextureManager::ReleaseTexture(unsigned int TextureIndex)
{
	if (TextureIndex > (unsigned int)TextureList.size() - 1) return false;

	Texture* BACK = &TextureList[TextureList.size() - 1];
	Texture* GONE = &TextureList[TextureIndex];

	memcpy(GONE, BACK, sizeof(Texture));
	TextureList.pop_back();
	return true;
}

void CTextureManager::DrawTexture(unsigned int TextureID, unsigned int X, unsigned int Y, unsigned int W, unsigned int H)
{
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