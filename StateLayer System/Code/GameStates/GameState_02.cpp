#include "GameState_02.h"

#include "../Utilities/OpenGL/OGL_Functions.h"

GameState_02::GameState_02(void)
{
	m_nStateType = GAMESTATE_02;
	LoadAndBindBMP("./Assets/Textures/SquareTexture_02.bmp", (GLuint*)&m_nSquareTexture);
}

GameState_02::~GameState_02(void)
{
	glDeleteTextures(1, (GLuint*)&m_nSquareTexture);
}

void GameState_02::Initialize(void)
{
}

void GameState_02::Input(int nMouseX, int nMouseY, bool* KeyArray)
{
}

int GameState_02::Update(float fTimeSlice, int nMouseX, int nMouseY)
{
	return 1;
}

void GameState_02::Render3D(void)
{
}

void GameState_02::Render2D(void)
{
	glPushMatrix();
	{
		glBindTexture(GL_TEXTURE_2D, (GLuint)m_nSquareTexture);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(10.0f, 70.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(210.0f, 70.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(210.0f, 120.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(10.0f, 120.0f);
		glEnd();
	}
	glPopMatrix();
}