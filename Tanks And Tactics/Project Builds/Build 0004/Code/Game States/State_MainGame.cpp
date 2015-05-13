////////////////////////////////////////////////////////////////////////////////
//
//	Name:			State_MainGame.cpp
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
#include "State_MainGame.h"

CState_Battle::CState_Battle(float* TimeSlice)
{
	//  Set up preliminary variables
	m_pTimeSlice = TimeSlice;
	m_bDebugMode = true;

	//  Set the current battle status flags
	m_nCurrentBattlePhase	= EFFECT;
	m_nCurrentUnitPhase		= ACTION_MENU;
	
	//  Gather the necessary controllers
	m_pTexture	= CTextureController::GetInstance();
	m_pMap		= CMapController::GetInstance();
	m_pInput	= CInputController::GetInstance();
	m_pFont		= CFontController::GetInstance();

	//  Initialize the state
	Initialize();
}

void CState_Battle::Initialize(void)
{
	//  Load the test map
	m_pMap->LoadMap("./Assets/TestMapFile.ttm");

	//  Load the test font
	m_nTestFont = m_pFont->LoadBitmapFont("./Assets/BitmapFont_01.png", 256, 256);

	//  Load the character selector image
	m_nUnitSelector = m_pTexture->LoadTexture("./Assets/UnitSelector.png");
	m_nMenuChoice[0] = m_pTexture->LoadTexture("./Assets/Move.png");
	m_nMenuChoice[1] = m_pTexture->LoadTexture("./Assets/Attack.png");
	m_nMenuChoice[2] = m_pTexture->LoadTexture("./Assets/Cancel.png");
	m_nUnitExhausted = m_pTexture->LoadTexture("./Assets/UnitExhausted.png");
	
	// Set the camera to a default area
	m_Camera.SetOrigin(1.0f, 1.0f, -5.0f);
	m_Camera.SetForward(0.0f, 0.0f, 1.0f);
	m_Camera.Normalize();

	// Initialize the Test Units
	for (unsigned int i = 0; i < UNIT_COUNT; ++i)
	{
		m_UnitList[i].Initialize("./Assets/Character.char");
		m_UnitList[i].SetPosition((unsigned int)RandInt(0, m_pMap->GetHorizontal() - 1), (unsigned int)RandInt(0, m_pMap->GetVertical() - 1));

	}
	m_nSelectedUnit = -1;
}

bool CState_Battle::Input(void)
{
	if (m_pInput->GetKey(SDLK_ESCAPE))			return false;
	if (m_pInput->GetKey(SDLK_BACKQUOTE) == 1)	m_bDebugMode = !m_bDebugMode;

	if (m_bDebugMode == true)
	{
		if (m_pInput->GetKey(SDLK_a))		m_Camera.MoveRight(0.01f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_d))		m_Camera.MoveRight(-0.01f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_w))		m_Camera.MoveForward(0.03f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_s))		m_Camera.MoveForward(-0.03f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_q))		m_Camera.MoveUp(0.025f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_e))		m_Camera.MoveUp(-0.025f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_UP))		m_Camera.RotateLocalX(-0.01f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_DOWN))	m_Camera.RotateLocalX(0.01f * (*m_pTimeSlice));
		if (m_pInput->GetKey(SDLK_LEFT))	m_Camera.RotateWorld(0.013f * (*m_pTimeSlice), 0.0f, 1.0f, 0.0f);
		if (m_pInput->GetKey(SDLK_RIGHT))	m_Camera.RotateWorld(-0.013f * (*m_pTimeSlice), 0.0f, 1.0f, 0.0f);
	}

	if		(m_nCurrentBattlePhase == EFFECT)	return EffectPhaseInput();
	else if	(m_nCurrentBattlePhase == BATTLE)	return BattlePhaseInput();
	else if	(m_nCurrentBattlePhase == ACTION)	return ActionPhaseInput();

	return false;
}

int CState_Battle::Update(void)
{
	if		(m_nCurrentBattlePhase == EFFECT)	return EffectPhaseUpdate();
	else if	(m_nCurrentBattlePhase == BATTLE)	return BattlePhaseUpdate();
	else if	(m_nCurrentBattlePhase == ACTION)	return ActionPhaseUpdate();
	return 0;
}

void CState_Battle::Render3D(void)
{
	//  Apply the transform of the camera
	m_Camera.ApplyCameraTransform();

	// Render the entire map
	if		((m_nSelectedUnit != -1) && (m_nCurrentUnitPhase == AIMING))		m_pMap->RenderCulled();
	else if ((m_nSelectedUnit != -1) && (m_nCurrentUnitPhase == MOVE_CHECK))	m_pMap->RenderGrid();
	else																		m_pMap->RenderAll();

	//  Render the Test Units
	for (int i = 0; i < UNIT_COUNT; ++i)
		m_UnitList[i].Render();

	// If a unit is selected, render the unit selector above them
	if (m_nSelectedUnit != -1)
	{
		vec3f Up		= m_Camera.GetUp() / 20.0f;
		vec3f Right		= m_Camera.GetRight() / 20.0f;
		vec3f Unit		= m_Camera.GetOrigin() - (m_Camera.GetOrigin() - m_UnitList[m_nSelectedUnit].GetPosition()).normalize() * 2.0f;

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTexture(m_nUnitSelector));
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);		glVertex3f(Unit.x - Right.x * 0.25f + Up.x * 2.0f, Unit.y + Up.y * 2.0f, Unit.z - Right.z * 0.25f + Up.z * 2.0f);
			glTexCoord2f(0.0f, 0.0f);		glVertex3f(Unit.x + Right.x * 0.25f + Up.x * 2.0f, Unit.y + Up.y * 2.0f, Unit.z + Right.z * 0.25f + Up.z * 2.0f);
			glTexCoord2f(0.0f, 1.0f);		glVertex3f(Unit.x + Right.x * 0.25f + Up.x * 1.0f, Unit.y + Up.y * 1.0f, Unit.z + Right.z * 0.25f + Up.z * 1.0f);
			glTexCoord2f(1.0f, 1.0f);		glVertex3f(Unit.x - Right.x * 0.25f + Up.x * 1.0f, Unit.y + Up.y * 1.0f, Unit.z - Right.z * 0.25f + Up.z * 1.0f);
		glEnd();
		glEnable(GL_DEPTH_TEST);

		// If the unit has not exhausted their turn, and is not moving, throw up an action menu
		if (m_nCurrentUnitPhase == ACTION_MENU)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTexture(m_nMenuChoice[0]));
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 0.0f);		glVertex3f(Unit.x + Right.x * 0.5f + Up.x * 2.5f, Unit.y + Up.y * 2.5f, Unit.z + Right.z * 0.5f + Up.z * 2.5f);
				glTexCoord2f(0.0f, 0.0f);		glVertex3f(Unit.x + Right.x * 1.5f + Up.x * 2.5f, Unit.y + Up.y * 2.5f, Unit.z + Right.z * 1.5f + Up.z * 2.5f);
				glTexCoord2f(0.0f, 1.0f);		glVertex3f(Unit.x + Right.x * 1.5f + Up.x * 0.5f, Unit.y + Up.y * 0.5f, Unit.z + Right.z * 1.5f + Up.z * 0.5f);
				glTexCoord2f(1.0f, 1.0f);		glVertex3f(Unit.x + Right.x * 0.5f + Up.x * 0.5f, Unit.y + Up.y * 0.5f, Unit.z + Right.z * 0.5f + Up.z * 0.5f);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTexture(m_nMenuChoice[1]));
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 0.0f);		glVertex3f(Unit.x - Right.x * 0.5f + Up.x * 4.0f, Unit.y + Up.y * 4.0f, Unit.z - Right.z * 0.5f + Up.z * 4.0f);
				glTexCoord2f(0.0f, 0.0f);		glVertex3f(Unit.x + Right.x * 0.5f + Up.x * 4.0f, Unit.y + Up.y * 4.0f, Unit.z + Right.z * 0.5f + Up.z * 4.0f);
				glTexCoord2f(0.0f, 1.0f);		glVertex3f(Unit.x + Right.x * 0.5f + Up.x * 2.0f, Unit.y + Up.y * 2.0f, Unit.z + Right.z * 0.5f + Up.z * 2.0f);
				glTexCoord2f(1.0f, 1.0f);		glVertex3f(Unit.x - Right.x * 0.5f + Up.x * 2.0f, Unit.y + Up.y * 2.0f, Unit.z - Right.z * 0.5f + Up.z * 2.0f);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTexture(m_nMenuChoice[2]));
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 0.0f);		glVertex3f(Unit.x - Right.x * 1.5f + Up.x * 2.5f, Unit.y + Up.y * 2.5f, Unit.z - Right.z * 1.5f + Up.z * 2.5f);
				glTexCoord2f(0.0f, 0.0f);		glVertex3f(Unit.x - Right.x * 0.5f + Up.x * 2.5f, Unit.y + Up.y * 2.5f, Unit.z - Right.z * 0.5f + Up.z * 2.5f);
				glTexCoord2f(0.0f, 1.0f);		glVertex3f(Unit.x - Right.x * 0.5f + Up.x * 0.5f, Unit.y + Up.y * 0.5f, Unit.z - Right.z * 0.5f + Up.z * 0.5f);
				glTexCoord2f(1.0f, 1.0f);		glVertex3f(Unit.x - Right.x * 1.5f + Up.x * 0.5f, Unit.y + Up.y * 0.5f, Unit.z - Right.z * 1.5f + Up.z * 0.5f);
			glEnd();

			glEnable(GL_DEPTH_TEST);
		}
	}

	// Debugging (Display the picking ray)
	if (m_bDebugMode == true)
	{
		glDisable(GL_TEXTURE_2D);
		glPointSize(5);
		glLineWidth(3);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_POINTS);
			glVertex3f(m_vPickingRay[0].x, m_vPickingRay[0].y, m_vPickingRay[0].z);
		glEnd();

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glBegin(GL_POINTS);
			glVertex3f(m_vPickingRay[1].x, m_vPickingRay[1].y, m_vPickingRay[1].z);
		glEnd();

		glColor3f(0.5f, 0.5f, 1.0f);
		glBegin(GL_LINES);
			glVertex3f(m_vPickingRay[0].x, m_vPickingRay[0].y, m_vPickingRay[0].z);
			glVertex3f(m_vPickingRay[1].x, m_vPickingRay[1].y, m_vPickingRay[1].z);
		glEnd();
	}
}

void CState_Battle::Render2D(void)
{
	// Output debug information
	if (m_bDebugMode == true)
	{
		m_pFont->RenderText(m_nTestFont, 10, 10, "Debug Mode: Enabled");

		// Debugging (Draw out mouse coordinates to help debug ray picking)
		float fWindowWidth  = (WINDOW_WIDTH != 0) ? (float)WINDOW_WIDTH : 1.0f;
		float fWindowHeight = (float)WINDOW_HEIGHT;
		float fViewX =  ((float)m_pInput->GetMouseX() - (fWindowWidth  / 2.0f)) / (fWindowWidth  / 2.0f);
		float fViewY = -((float)m_pInput->GetMouseY() - (fWindowHeight / 2.0f)) / (fWindowHeight / 2.0f);
		char DebugString[64];
		
		sprintf_s(DebugString, 64, "Mouse Position: (%d, %d)", m_pInput->GetMouseX(), m_pInput->GetMouseY());
		m_pFont->RenderText(m_nTestFont, 10, 40, DebugString);
		
		sprintf_s(DebugString, 64, "Mouse Relative: (%f, %f)", m_pInput->GetMouseX() / fWindowWidth, m_pInput->GetMouseY() / fWindowHeight);
		m_pFont->RenderText(m_nTestFont, 10, 60, DebugString);
		
		sprintf_s(DebugString, 64, "Mouse View: (%f, %f)", fViewX, fViewY);
		m_pFont->RenderText(m_nTestFont, 10, 80, DebugString);
		
		sprintf_s(DebugString, 64, "Selected Unit: %d", m_nSelectedUnit);
		m_pFont->RenderText(m_nTestFont, 10, 100, DebugString);
	}
	else m_pFont->RenderText(m_nTestFont, 10, 10, "Debug Mode: Disabled");
}

////////////////////
//~ Per-Phase Input
////////////////////

bool CState_Battle::EffectPhaseInput(void)
{
	return true;
}

bool CState_Battle::BattlePhaseInput(void)
{
	//! If TAB is hit, select the next unit in the list. If no unit is
	//! selected, select the first unit.
	if (m_pInput->GetKey(SDLK_TAB) == 1)
	{
		if (m_nSelectedUnit == -1)	m_nSelectedUnit = 0;
		else
		{
			++m_nSelectedUnit;
			if (m_nSelectedUnit >= UNIT_COUNT) m_nSelectedUnit = 0;
			m_nCurrentUnitPhase = ACTION_MENU;
		}
	}

	//! If the left mouse button is clicked, execute the appropriate action
	if (m_pInput->GetMouseButton(0) == 1)
	{
		//! Cast a ray from the camera outward based on the mouse position.
		vec3f Cam = m_Camera.GetOrigin();
		vec3f Ray = CalculatePickingRay(m_pInput->GetMouseX(), m_pInput->GetMouseY(), m_Camera);

		if (m_nSelectedUnit == -1)
		{
			//! If no unit is selected, find an intersection with a unit if one
			//! exists, and select it.
			for (int i = 0; i < UNIT_COUNT; ++i)
			{
				if (m_UnitList[i].RaySelect(m_Camera, Ray))
				{
					//m_nCurrentUnitPhase = ACTION_MENU;
					m_nSelectedUnit = i;
					return true;
				}
			}
		}
		else
		{
			//! Depending on the current unit phase, enact input choices
			if (m_nCurrentUnitPhase == ACTION_MENU)
			{
				//! Check if the click will select a menu choice, and act accordingly
				//! The keyboard arrow keys can be used as an alternative

				vec3f Up		= m_Camera.GetUp() / 20.0f;
				vec3f Right		= m_Camera.GetRight() / 20.0f;
				vec3f Unit		= m_Camera.GetOrigin() - (m_Camera.GetOrigin() - m_UnitList[m_nSelectedUnit].GetPosition()).normalize() * 2.0f;
				vec3f SpherePos;

				// Check the first button
				SpherePos = vec3f(Unit.x + Right.x * 0.5f + Up.x, Unit.y + Up.y, Unit.z + Right.z * 0.5f + Up.z);
				if (LineToSphereCollision(Cam, Ray, SpherePos, Up.magnitude()))
				{
					m_pMap->CalculateGrid(&m_UnitList[m_nSelectedUnit]);
					m_nCurrentUnitPhase = MOVE_CHECK;
				}

				SpherePos = vec3f(Unit.x + Up.x * 3.0f, Unit.y + Up.y * 3.0f, Unit.z + Up.z * 3.0f);
				if (LineToSphereCollision(Cam, Ray, SpherePos, Up.magnitude()))
				{
					m_nCurrentUnitPhase = AIM_CHECK;
				}

				SpherePos = vec3f(Unit.x - Right.x * 0.5f + Up.x, Unit.y + Up.y, Unit.z - Right.z * 0.5f + Up.z);
				if (LineToSphereCollision(Cam, Ray, SpherePos, Up.magnitude()))
				{
					m_nSelectedUnit = -1;
				}
			}
			else if (m_nCurrentUnitPhase == MOVE_CHECK)
			{
				//! If a unit is selected, find the interesection point within the
				//! geometry (if one exists), and direct the selected unit to it.
				vec3f MoveTo;
				if (m_pMap->RayCollision(Cam, Cam + Ray, MoveTo))
				{
					MovementArea Path;
					if (m_pMap->IsMovePossible(MoveTo, Path))
					{
						m_UnitList[m_nSelectedUnit].SetMovePath(Path);
						m_nSelectedUnit = -1;
						m_nCurrentUnitPhase = ACTION_MENU;
					}
				}
			}
			else if (m_nCurrentUnitPhase == AIM_CHECK)
			{
				//! If a unit is selected, find the interesection point within the
				//! geometry (if one exists), and direct the selected unit to aim at it.
				vec3f AimAt;
				if (m_pMap->RayCollision(Cam, Cam + Ray, AimAt))
				{
					m_pMap->CalculateCulled(m_UnitList[m_nSelectedUnit].GetPosition(), AimAt);
					m_nCurrentUnitPhase = AIMING;
				}
			}
		}


		//Debugging (Define a line from the camera to the point selected)
		if (m_bDebugMode == true)
		{
			m_vPickingRay[0] = m_Camera.GetOrigin();
			m_vPickingRay[1] = m_vPickingRay[0] + Ray;
		}
	}

	// NOTE: THIS SLOWS THE GAME DOWN SOOO MUCH
	if (m_nCurrentUnitPhase == MOVE_CHECK)
	{
		// Only execute this action every X frames
		static unsigned int GridSelectCount = 0;
		if (++GridSelectCount >= 5)
		{
			//! Define the ray to the ground and highlight the grid area
			//! if it is within the area that the selected unit can move to
			vec3f Cam = m_Camera.GetOrigin();
			vec3f Ray = CalculatePickingRay(m_pInput->GetMouseX(), m_pInput->GetMouseY(), m_Camera);
			vec3f GridSelect;
			MovementArea Area;
			if (m_pMap->RayCollision(Cam, Cam + Ray, GridSelect))
				if (m_pMap->IsMovePossible(GridSelect, Area))
					m_pMap->GridSelection((unsigned int)(GridSelect.x / POS_X_LENGTH), (unsigned int)(GridSelect.z / POS_Z_LENGTH));

			GridSelectCount = 0;
		}
	}

	if (m_pInput->GetMouseButton(1) == 1)
	{
		m_nSelectedUnit = -1;
		m_nCurrentUnitPhase = ACTION_MENU;
	}

	return true;
}

bool CState_Battle::ActionPhaseInput(void)
{
	return true;
}

////////////////////
//~ Per-Phase Update
////////////////////

int CState_Battle::EffectPhaseUpdate(void)
{
	//! Loop through all effects listed in the vector. If the effect
	//! is on a unit that no longer exists, skip the effect. Otherwise,
	//! create the effect using special objects and unit functionality

	while (m_vEffectList.size())
	{
		// TODO: Enact effects
	}

	// Once all effects are completed, prepare for BATTLE Phase
	m_nSelectedUnit = -1;
	for (unsigned int i = 0; i < UNIT_COUNT; ++i) m_bUnitExhausted[i] = false;
	m_nCurrentUnitPhase = ACTION_MENU;

	//  Move to the next BattlePhase
	m_nCurrentBattlePhase++;
	if (m_nCurrentBattlePhase == BATTLEPHASE_COUNT) m_nCurrentBattlePhase = 0;

	return 1;
}

int CState_Battle::BattlePhaseUpdate(void)
{
	//  Update the Test Units
	for (int i = 0; i < UNIT_COUNT; ++i)
	{
		m_UnitList[i].Update((*m_pTimeSlice));
	}

	return 1;
}

int CState_Battle::ActionPhaseUpdate(void)
{
	//  Move to the next BattlePhase
	m_nCurrentBattlePhase++;
	if (m_nCurrentBattlePhase == BATTLEPHASE_COUNT) m_nCurrentBattlePhase = 0;

	return 1;
}
