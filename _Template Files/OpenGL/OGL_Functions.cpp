#include "OGL_Functions.h"

#include <stdio.h>
#include <gl\glaux.h>

void InitializeOpenGL(void)
{
	// Lighting
	GLfloat Float4_00_00_00_00[]	= { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat Float4_01_01_01_10[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat Float4_02_02_02_10[]	= { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat Float4_04_04_04_10[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat Float4_06_06_06_10[]	= { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat Float4_08_08_08_10[]	= { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat Float4_10_10_10_10[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat Light0_Pos[]			= { -2.0f, 10.0f, 0.0f, 0.0f };

	// Light0: Diffuse from above
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT,	Float4_00_00_00_00);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,	Float4_08_08_08_10);
	glLightfv(GL_LIGHT0, GL_SPECULAR,	Float4_08_08_08_10);
	glLightfv(GL_LIGHT0, GL_POSITION,	Light0_Pos);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_FILL);

	// Material initialization
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Float4_06_06_06_10);
	glMateriali(GL_FRONT, GL_SHININESS, 100);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Float4_08_08_08_10);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
}

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
		unsigned char* ucSwap = new unsigned char[Image->sizeX * 3];
		for (int i = 0; i < Image->sizeY / 2; i += 1)
		{
			memcpy(ucSwap, &Image->data[i * Image->sizeX * 3], Image->sizeX * 3);
			memcpy(&Image->data[i * Image->sizeX * 3], &Image->data[(Image->sizeY - 1 - i) * Image->sizeX * 3], Image->sizeX * 3);
			memcpy(&Image->data[(Image->sizeY - 1 - i) * Image->sizeX * 3], ucSwap, Image->sizeX * 3);
		}
		delete [] ucSwap;

		glGenTextures(1, Texture);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Image->sizeX, Image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (Image)
	{
		if (Image->data) free(Image->data);
		free(Image);
	}

	return true;
}