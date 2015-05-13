////////////////////////////////////////////////////////////////////////////////
//
//	Name:			TextureController.h
//
//	Description:	This file contains a class that keeps 
//					track of and allows access to texture 
//					allocations. It acts as a single point 
//					of access to textures and their functions
//
//	Created:		January 2nd, 2009
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_TEXTURECONTROLLER_
#define _DB_TEXTURECONTROLLER_

#define INDEX_NOIMAGE 9999

#include <SDL/SDL.h>
#include <vector>

class CTextureController
{
private:
	struct Texture
	{
		unsigned int	Image;
		char			Filename[64];
		unsigned int	References;
	};

	CTextureController();
	~CTextureController();

	std::vector<Texture>	TextureList;

public:
	static CTextureController* GetInstance(void);

	unsigned int	LoadTexture(char* TextureFile);
	bool			ReleaseTexture(unsigned int TextureIndex);
	void			DrawTexture(unsigned int TextureID, unsigned int X, unsigned int Y, unsigned int W, unsigned int H);

	inline	unsigned int	GetTexture(unsigned int TextureID)	{	return  (TextureID == INDEX_NOIMAGE) ? INDEX_NOIMAGE : TextureList[TextureID].Image;	}
};

#endif