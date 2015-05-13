#include "OGL_Functions.h"

#include <stdio.h>
#include <gl\glaux.h>

bool LoadAndBindBMP(char *Filename, GLuint* Texture)
{
	if (!Filename) return false;

	FILE *File;
	fopen_s(&File, Filename, "r");
	if (!File) return false;
	fclose(File);

	AUX_RGBImageRec* Image = NULL;
	if (Image = auxDIBImageLoad(Filename))
	{
		glGenTextures(1, Texture);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Image->sizeX, Image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->data);
	}

	if (Image)
	{
		if (Image->data) free(Image->data);
		free(Image);
	}

	return true;
}