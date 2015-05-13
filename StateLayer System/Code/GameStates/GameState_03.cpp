#include "GameState_03.h"

#include "../Utilities/OpenGL/OGL_Functions.h"

GameState_03::GameState_03(void)
{
	m_nStateType = GAMESTATE_03;
	LoadAndBindBMP("./Assets/Textures/SquareTexture_03.bmp", (GLuint*)&m_nSquareTexture);
}

GameState_03::~GameState_03(void)
{
	glDeleteTextures(1, (GLuint*)&m_nSquareTexture);
}

void GameState_03::Initialize(void)
{
}

void GameState_03::Input(int nMouseX, int nMouseY, bool* KeyArray)
{
}

int GameState_03::Update(float fTimeSlice, int nMouseX, int nMouseY)
{
	return 1;
}

void GameState_03::Render3D(void)
{
}

void GameState_03::Render2D(void)
{
	glPushMatrix();
	{
		glBindTexture(GL_TEXTURE_2D, (GLuint)m_nSquareTexture);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(10.0f, 130.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(210.0f, 130.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(210.0f, 194.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(10.0f, 194.0f);
		glEnd();
	}
	glPopMatrix();
}