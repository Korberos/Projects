#include "OGL_Functions.h"

#include <stdio.h>
#include <fstream>
#include "../DecodePNG.h"

bool LoadAndBindPNG(char *Filename, int* Texture, TextureInfo* TexInfo)
{
	if (!Filename) return false;

	std::vector<unsigned char> Buffer, Image;
	std::ifstream file(Filename, std::ios::in | std::ios::binary | std::ios::ate);

	// Assess the file size
	std::streamsize size = 0;
	if(file.seekg(0, std::ios::end).good()) size = file.tellg();
	if(file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	// Read the contents of the file into the vector
	Buffer.resize(size_t(size));
	if(size > 0) file.read((char*)(&Buffer[0]), size);

	unsigned long Width;
	unsigned long Height;
	if (decodePNG(Image, Width, Height, Buffer.empty() ? 0 : &Buffer[0], (unsigned long)Buffer.size()) != 0) return false;

	if (TexInfo != 0)
	{
		TexInfo->Width	= Width;
		TexInfo->Height	= Height;
	}

	glBindTexture(GL_TEXTURE_2D, (GLuint)*Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(&(Image.front())));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Image.clear();
	Buffer.clear();

	return true;
}