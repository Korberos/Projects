#pragma once

#include "StateLayerSystem.h"

class GameState_01 : public GameState
{
private:
	unsigned int		m_nSquareTexture;


public:
	GameState_01(void);
	~GameState_01(void);

	void	Initialize(void);
	void	Input(int nMouseX, int nMouseY, bool* KeyArray);
	int		Update(float fTimeSlice, int nMouseX, int nMouseY);
	void	Render3D(void);
	void	Render2D(void);
};