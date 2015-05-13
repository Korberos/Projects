#pragma once

#include "StateLayerSystem.h"
#include "../Systems/Texture Manager/Texture Manager.h"
#include "../Systems/Map Control/Map Control.h"

class GS_World : public GameState
{
private:
	int					m_nImages[1];

	TextureManager*		m_pTextures;
	MapControl*			m_pMapControl;

	float*				m_pTime;


public:
	GS_World(void);
	~GS_World(void);

	void	Initialize(float* fTime);
	bool	Input(int nMouseX, int nMouseY, char* KeyArray);
	int		Update(int nMouseX, int nMouseY);
	void	Render3D(void);
	void	Render2D(void);
};