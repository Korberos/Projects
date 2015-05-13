#include "GS_FrontEnd.h"

#define	REVERSE_RATIO	1024.0f / 768.0f

GS_FrontEnd::GS_FrontEnd(void)
{
	m_nStateID = GAMESTATE_01;

	m_pTextures = TextureManager::GetInstance();
	memset(&m_nImages[0], -1, sizeof(m_nImages[0]) * 1);

	m_Menu.AddMenuWidget("./_Assets/Scripts/Button 01.bin");
	m_Menu.AddMenuWidget("./_Assets/Scripts/Button 02.bin");
	m_Menu.AddMenuWidget("./_Assets/Scripts/Button 03.bin");
	m_Menu.AddMenuWidget("./_Assets/Scripts/Button 04.bin");
}

GS_FrontEnd::~GS_FrontEnd(void)
{
	glDeleteTextures(1, (GLuint*)m_nImages);
}

void GS_FrontEnd::Initialize(float* fTime)
{
	m_nImages[0] = m_pTextures->LoadTexture("./_Assets/Textures/Front End/Background.png");

	m_pTime = fTime;
}

bool GS_FrontEnd::Input(int nMouseX, int nMouseY, char* KeyArray)
{
	m_Menu.Input();
	return true;
}

int GS_FrontEnd::Update(int nMouseX, int nMouseY)
{
	m_Menu.Update();
	return 1;
}

void GS_FrontEnd::Render3D(void)
{
}

void GS_FrontEnd::Render2D(void)
{
	m_pTextures->DrawTexture(m_nImages[0], 0.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_Menu.Render();
}