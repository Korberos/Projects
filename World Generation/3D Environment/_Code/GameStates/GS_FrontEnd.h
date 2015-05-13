#pragma once

#include "StateLayerSystem.h"
#include "../Systems/Texture Manager/Texture Manager.h"
#include "../Systems/Menu System/Menu System.h"

class GS_FrontEnd : public GameState
{
private:
	int					m_nImages[1];

	TextureManager*		m_pTextures;
	MenuSystem			m_Menu;

	float*				m_pTime;


public:
	GS_FrontEnd(void);
	~GS_FrontEnd(void);

	void	Initialize(float* fTime);
	bool	Input(int nMouseX, int nMouseY, char* KeyArray);
	int		Update(int nMouseX, int nMouseY);
	void	Render3D(void);
	void	Render2D(void);
};