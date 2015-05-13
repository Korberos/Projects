#include "GS_World.h"

#include "../Systems/Input Control/Input Control.h"

#define	REVERSE_RATIO	1024.0f / 768.0f

GS_World::GS_World(void)
{
	m_nStateID = GAMESTATE_02;

	m_pMapControl = MapControl::GetInstance();
	m_pTextures = TextureManager::GetInstance();
	memset(&m_nImages[0], -1, sizeof(m_nImages[0]) * 1);
}

GS_World::~GS_World(void)
{
	glDeleteTextures(1, (GLuint*)m_nImages);
	m_pMapControl->Shutdown();
}

void GS_World::Initialize(float* fTime)
{
	m_nImages[0] = m_pTextures->LoadTexture("./_Assets/Textures/World/WorldTexture.png");
	m_pMapControl->LoadMapFromFile("./_Assets/Maps/MapFile.bin");

	m_pTime = fTime;
}

bool GS_World::Input(int nMouseX, int nMouseY, char* KeyArray)
{
	InputControl* pInput = InputControl::GetInstance();

	if (pInput->GetKey(87))	// W
	{
		StateLayerSystem::GetCamera()->MoveForward(0.8f * (*m_pTime));
	}
	if (pInput->GetKey(65))	// A
	{
		StateLayerSystem::GetCamera()->MoveRight(0.8f * (*m_pTime));
	}
	if (pInput->GetKey(83))	// S
	{
		StateLayerSystem::GetCamera()->MoveForward(-0.8f * (*m_pTime));
	}
	if (pInput->GetKey(68))	// D
	{
		StateLayerSystem::GetCamera()->MoveRight(-0.8f * (*m_pTime));
	}
	if (pInput->GetKey(38))	// Up
	{
		StateLayerSystem::GetCamera()->RotateLocalX(-0.8f * (*m_pTime));
	}
	if (pInput->GetKey(40))	// Down
	{
		StateLayerSystem::GetCamera()->RotateLocalX(0.8f * (*m_pTime));
	}
	if (pInput->GetKey(37))	// Left
	{
		StateLayerSystem::GetCamera()->RotateWorld(0.8f * (*m_pTime), 0.0f * (*m_pTime), 0.8f * (*m_pTime), 0.0f * (*m_pTime));
	}
	if (pInput->GetKey(39))	// Right
	{
		StateLayerSystem::GetCamera()->RotateWorld(-0.8f * (*m_pTime), 0.0f * (*m_pTime), 0.8f * (*m_pTime), 0.0f * (*m_pTime));
	}
	if (pInput->GetKey(81))	// Q
	{
		StateLayerSystem::GetCamera()->MoveUp(0.8f * (*m_pTime));
	}
	if (pInput->GetKey(69))	// E
	{
		StateLayerSystem::GetCamera()->MoveUp(-0.8f * (*m_pTime));
	}

	if (pInput->GetKey(27))
	{
		return false;
	}

	return true;
}

int GS_World::Update(int nMouseX, int nMouseY)
{
	return 1;
}

void GS_World::Render3D(void)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_nImages[0]);
	m_pMapControl->Render(m_pMapControl->GetMainVolume());
	//m_pMapControl->RenderVolumes(m_pMapControl->GetMainVolume(), 0.0f, false);
}

void GS_World::Render2D(void)
{
}