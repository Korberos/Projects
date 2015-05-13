#include "GameState_01.h"

#include "../Utilities/OpenGL/OGL_Functions.h"

GameState_01::GameState_01(void)
{
	m_nStateType = GAMESTATE_01;
	LoadAndBindBMP("./Assets/Textures/SquareTexture_01.bmp", (GLuint*)&m_nSquareTexture);
}

GameState_01::~GameState_01(void)
{
	glDeleteTextures(1, (GLuint*)&m_nSquareTexture);
}

void GameState_01::Initialize(void)
{
}

void GameState_01::Input(int nMouseX, int nMouseY, bool* KeyArray)
{
	if (KeyArray[87])	// W
	{
		StateLayerSystem::GetCamera()->MoveForward(0.01f);
	}
	if (KeyArray[65])	// A
	{
		StateLayerSystem::GetCamera()->MoveRight(0.01f);
	}
	if (KeyArray[83])	// S
	{
		StateLayerSystem::GetCamera()->MoveForward(-0.01f);
	}
	if (KeyArray[68])	// D
	{
		StateLayerSystem::GetCamera()->MoveRight(-0.01f);
	}
	if (KeyArray[38])	// Up
	{
		StateLayerSystem::GetCamera()->RotateLocalX(-0.01f);
	}
	if (KeyArray[40])	// Down
	{
		StateLayerSystem::GetCamera()->RotateLocalX(0.01f);
	}
	if (KeyArray[37])	// Left
	{
		StateLayerSystem::GetCamera()->RotateWorld(0.01f, 0.00f, 0.01f, 0.0f);
	}
	if (KeyArray[39])	// Right
	{
		StateLayerSystem::GetCamera()->RotateWorld(-0.01f, 0.0f, 0.01f, 0.0f);
	}
	if (KeyArray[81])	// Q
	{
		StateLayerSystem::GetCamera()->MoveUp(0.01f);
	}
	if (KeyArray[69])	// E
	{
		StateLayerSystem::GetCamera()->MoveUp(-0.01f);
	}
}

int GameState_01::Update(float fTimeSlice, int nMouseX, int nMouseY)
{
	return 1;
}

void GameState_01::Render3D(void)
{
	// NOTE: This is just a test for rendering
	glPushMatrix();
	{
		glDisable(GL_LIGHTING);
		
		GLfloat FloorHeight		= -0.1f;
		GLfloat GridLength		= 10.0f;
		GLfloat GridSeparation	= 0.25f;

		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 0.7f, 0.7f);
		glLineWidth(2);

		glBegin(GL_LINES);
		for (GLfloat X = -GridLength; X <= GridLength; X += GridSeparation)
		{
			glVertex3f(X, FloorHeight, GridLength);
			glVertex3f(X, FloorHeight, -GridLength);
			
			glVertex3f(GridLength, FloorHeight, X);
			glVertex3f(-GridLength, FloorHeight, X);
		}
		glEnd();

		glLineWidth(1);
		glEnable(GL_TEXTURE_2D);
	}
	glPopMatrix();
}

void GameState_01::Render2D(void)
{
	glPushMatrix();
	{
		glBindTexture(GL_TEXTURE_2D, (GLuint)m_nSquareTexture);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(10.0f, 10.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(210.0f, 10.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(210.0f, 60.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(10.0f, 60.0f);
		glEnd();
	}
	glPopMatrix();
}