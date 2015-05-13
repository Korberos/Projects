#pragma once

#include <vector>
using namespace std;

#include "../../Utilities/OpenGL/OGL_Functions.h"

#define	REVERSE_RATIO	1024.0f / 768.0f

struct Texture
{
	int		Identifier;
	char	Filename[256];
	int		Width;
	int		Height;
	int		References;
};

class TextureManager
{
private:
	unsigned int		m_nCurrentTexture;
	vector<Texture>		m_vTextures;

	TextureManager();
	~TextureManager();

public:
	static TextureManager* GetInstance(void);

	int		LoadTexture(char* Filename);
	void	ReleaseTexture(unsigned int nTextureID);
	bool	DrawTexture(int ID = -1, float X = 0.0f, float Y = 0.0f, float X_Scale = 1.0f, float Y_Scale = 1.0f);
	void	Shutdown(void);

	inline	int	GetTextureWidth(unsigned int nTexture)		{	return ((nTexture < m_vTextures.size()) ? m_vTextures[nTexture].Width: -1);	}
	inline	int	GetTextureHeight(unsigned int nTexture)		{	return ((nTexture < m_vTextures.size()) ? m_vTextures[nTexture].Height: -1);	}
};