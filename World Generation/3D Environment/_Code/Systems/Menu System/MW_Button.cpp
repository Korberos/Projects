#include "./MW_Button.h"

#include "../Input Control/Input Control.h"
#include "../Texture Manager/Texture Manager.h"

MW_Button::MW_Button(void)
{
	m_nX		= 0;
	m_nY		= 0;
	m_nTexture	= -1;
	m_nState	= STATE_NONE;
	m_fTimeDown	= 0.0f;
	m_fTime		= 0;
}

MW_Button::~MW_Button(void)
{
}

bool MW_Button::Initialize(void)
{
	return true;
}

int MW_Button::Input(void)
{
	InputControl*	pInput		= InputControl::GetInstance();
	TextureManager*	pTexture	= TextureManager::GetInstance();

	switch (m_nState)
	{
	case STATE_NONE:
		if ((pInput->GetMouseX()) > m_nX)
			if ((pInput->GetMouseX()) < m_nX + pTexture->GetTextureWidth(m_nTexture))
				if ((pInput->GetMouseY()) > m_nY)
					if ((pInput->GetMouseY()) < m_nY + pTexture->GetTextureHeight(m_nTexture))
						m_nState = STATE_MOUSEOVER;
		break;

	case STATE_MOUSEOVER:
		if (((pInput->GetMouseX()) < m_nX) || ((pInput->GetMouseX()) > m_nX + pTexture->GetTextureWidth(m_nTexture)) || ((pInput->GetMouseY()) < m_nY) || ((pInput->GetMouseY()) > m_nY + pTexture->GetTextureHeight(m_nTexture)))
		{
			m_nState = STATE_NONE;
		}
		else
		{
			if (pInput->GetBufferedMouseButton(0) == true)
		{
			ButtonDown();
			m_fTimeDown = 0.2f;
			m_nState = STATE_PUSHDOWN;
		}
		}
		break;

	case STATE_PUSHDOWN:
		if (m_fTimeDown != 0.0f) break;
		if (pInput->GetBufferedMouseButton(0) == false)
		{
			m_nState = STATE_NONE;
			if ((pInput->GetMouseX()) > m_nX)
				if ((pInput->GetMouseX()) < m_nX + pTexture->GetTextureWidth(m_nTexture))
					if ((pInput->GetMouseY()) > m_nY)
						if ((pInput->GetMouseY()) < m_nY + pTexture->GetTextureHeight(m_nTexture))
							m_nState = STATE_MOUSEOVER;
		}
		break;
	}
	return 0;
}

int MW_Button::Update(void)
{
	m_fTimeDown = max(m_fTimeDown - (*m_fTime), 0.0f);
	return 0;
}

void MW_Button::Render(void)
{
	TextureManager* pTexture = TextureManager::GetInstance();

	switch (m_nState)
	{
	case STATE_NONE:
		pTexture->DrawTexture(m_nTexture, (float)m_nX, (float)m_nY);
		break;

	case STATE_MOUSEOVER:
		pTexture->DrawTexture(m_nTexture, (float)m_nX, (float)m_nY, 1.05f, 1.1f);
		break;

	case STATE_PUSHDOWN:
		pTexture->DrawTexture(m_nTexture, (float)m_nX, (float)m_nY, 0.95f, 0.95f);
		break;
	}
}