#include "./Menu System.h"

#include <fstream>
using namespace std;

#include "../../GameStates/StateLayerSystem.h"

// Inlcude all Menu Widgets
#include "./MW_Button.h"

MenuSystem::MenuSystem(void)
{
	m_pTexture = TextureManager::GetInstance();
	Clear();
}

MenuSystem::~MenuSystem(void)
{
	Clear();
}

void MenuSystem::Clear(void)
{
	for (unsigned int i = 0; i < m_vWidgets.size(); i += 1) delete m_vWidgets[i];
	m_vWidgets.clear();
	m_nSelected = 0;
}

MenuWidget* MenuSystem::AddMenuWidget(char* szBinaryFile)
{
	fstream Input(szBinaryFile, ios_base::in | ios_base::binary);
	if ((Input.bad()) || (!Input.good()))
	{
		Input.close();
		return 0;
	}

	// Read in the widget type
	char Type;
	Input.read((char*)&Type, sizeof(Type));
	if ((Type < 0) || (Type >= WIDGET_TYPE_COUNT))
	{
		Input.close();
		return 0;
	}

	// Gather the variable types needed for the following switch statement
	MenuWidget*		NewWidget;
	char			Filename[64];
	int				X;
	int				Y;

	switch (Type)
	{
	case MW_BUTTON:
		NewWidget = new MW_Button;

		Input.read((char*)&X, sizeof(X));
		((MW_Button*)NewWidget)->SetX(X);
		
		Input.read((char*)&Y, sizeof(Y));
		((MW_Button*)NewWidget)->SetY(Y);

		Input.read((char*)Filename, 64);
		((MW_Button*)NewWidget)->SetTexture(m_pTexture->LoadTexture(Filename));

		((MW_Button*)NewWidget)->Initialize();
		((MW_Button*)NewWidget)->LinkToTime(StateLayerSystem::GetInstance()->GetTimeSliceAddress());
		break;
	}

	m_vWidgets.push_back(NewWidget);
	return NewWidget;
}

int MenuSystem::Input(void)
{
	int Return;
	for (unsigned int i = 0; i < m_vWidgets.size(); i += 1)
	{
		Return = m_vWidgets[i]->Input();
		if (Return != 0) return Return;
	}
	return 0;
}

int MenuSystem::Update(void)
{
	int Return;
	for (unsigned int i = 0; i < m_vWidgets.size(); i += 1)
	{
		Return = m_vWidgets[i]->Update();
		if (Return != 0) return Return;
	}
	return 0;
}

void MenuSystem::Render(void)
{
	for (unsigned int i = 0; i < m_vWidgets.size(); i += 1)
		m_vWidgets[i]->Render();
}