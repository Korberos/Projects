#pragma once

#include <vector>
using namespace std;

#include "../Texture Manager/Texture Manager.h"

#include "./Menu Widget.h"

class MenuSystem
{
private:
	TextureManager*		m_pTexture;

	vector<MenuWidget*>	m_vWidgets;
	unsigned int		m_nSelected;

public:
	MenuSystem(void);
	~MenuSystem(void);

	void			Clear(void);
	MenuWidget*		AddMenuWidget(char* szBinaryFile);
	int				Input(void);
	int				Update(void);
	void			Render(void);
};