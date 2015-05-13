#pragma once

#include "OGL_Includes.h"

struct TextureInfo
{
public:
	int		Width;
	int		Height;
};

bool LoadAndBindPNG(char *Filename, int* Texture, TextureInfo* TexInfo = 0);