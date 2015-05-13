////////////////////////////////////////////////////////////////////////////////
//
//	Name:			State_MainGame.h
//
//	Description:	This file contains the Game State controlling
//					the main game of the tech demo. The main game
//					consists of the different states of play, and
//					allows the player to give input and control the
//					general gameplay.
//
//	Created:		January 20th, 2008
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_STATEMAINGAME_
#define _DB_STATEMAINGAME_

#include "GameState.h"

#include "./../Global.h"
#include "./../Controllers/TextureController.h"
#include "./../Controllers/MapController.h"
#include "./../Controllers/InputController.h"
#include "./../Controllers/FontController.h"
#include "./../Utilities/OpenGLFrame.h"
#include "./../Utilities/PickingRay.h"
#include "./../Utilities/RandomNumber.h"
#include "./../Utilities/LineToSphere.h"
#include "./../Objects/CharacterUnit.h"

#define	UNIT_COUNT	3

struct CEffect
{
	enum EffectTarget	{ ATMOSPHERE = 0, SINGLE_UNIT, UNIT_LOCATION, TEAM_UNITS, ALL_UNITS, EFFECTTARGET_COUNT	};
	unsigned int		m_nTargetType;
	unsigned int		m_nTarget;

	enum AtmosphereEffect { CLEAR = 0, RAIN, SNOW, SANDSTORM, CLOUDY, ATMOSPHERETYPE_COUNT	};
	enum SingleUnitEffect { ELECTRIC = 0, POISON, HEAL, COUNTER_DOWN, EXPLODE, STEAL, SINGLEUNITEFFECT_COUNT	};
	unsigned int		m_nEffect;
};

class CState_Battle : public CGameState
{
public:
	CState_Battle(float* TimeSlice);
	~CState_Battle()					{}

	void					Initialize(void);
	bool					Input(void);
	int						Update(void);
	void					Render3D(void);
	void					Render2D(void);

private:
	CTextureController*		m_pTexture;
	CMapController*			m_pMap;
	CInputController*		m_pInput;
	CFontController*		m_pFont;
	COpenGLFrame			m_Camera;

	CharacterUnit			m_UnitList[UNIT_COUNT];
	bool					m_bUnitExhausted[UNIT_COUNT];
	int						m_nSelectedUnit;


	unsigned int			m_nMapTexture;
	bool					m_bDebugMode;
	unsigned int			m_nTestFont;
	float*					m_pTimeSlice;
	vec3f					m_vPickingRay[2];
	unsigned int			m_nUnitSelector;
	unsigned int			m_nMenuChoice[3];
	unsigned int			m_nUnitExhausted;

	bool					EffectPhaseInput(void);
	bool					BattlePhaseInput(void);
	bool					ActionPhaseInput(void);

	int						EffectPhaseUpdate(void);
	int						BattlePhaseUpdate(void);
	int						ActionPhaseUpdate(void);

	enum BattlePhase		{	EFFECT = 0, BATTLE, ACTION, BATTLEPHASE_COUNT	};
	unsigned int			m_nCurrentBattlePhase;

	enum UnitPhase			{	ACTION_MENU = 0, MOVE_CHECK, MOVING, AIM_CHECK, AIMING, UNITPHASE_COUNT		};
	unsigned int			m_nCurrentUnitPhase;

	std::vector<CEffect>	m_vEffectList;

	unsigned int			m_nRaySphereCollision;

	unsigned int			m_nDebugTest;		// Used for various debugging
};

#endif