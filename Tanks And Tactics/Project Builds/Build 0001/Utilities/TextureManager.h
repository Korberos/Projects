////////////////////////////////////////////////////////////////////////////////
//
//	Name:			TextureManager.h
//
//	Description:	This file contains a class that keeps 
//					track of and allows access to texture 
//					allocations. It acts as a single point 
//					of access to textures and their functions
//
//	Created:		January 2nd, 2009
//
//	Last Modified:	January 2nd, 2009
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <SDL/SDL.h>
#include <vector>

class CTextureManager
{
private:
	struct Texture
	{
		unsigned int	Image;
		char			Filename[64];
		unsigned int	References;
	};

	CTextureManager();
	~CTextureManager();

	std::vector<Texture>	TextureList;

public:
	static CTextureManager* GetInstance(void);

	unsigned int	LoadTexture(char* TextureFile);
	bool			ReleaseTexture(unsigned int TextureIndex);
	void			DrawTexture(unsigned int TextureID, unsigned int X, unsigned int Y, unsigned int W, unsigned int H);
};