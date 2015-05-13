////////////////////////////////////////////////////////////////////////////////
//
//	Name:			CharacterUnit.h
//
//	Description:	This file contains the class that forms the 
//					basis of all characters/units in the game. The 
//					class contains base functionality for the unit 
//					object and can carry over to any type of unit.
//
//	Created:		December 19th, 2008
//
//	Last Modified:	December 19th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_CHARACTERUNIT_
#define _DB_CHARACTERUNIT_

#include <SDL/SDL_opengl.h>
#include "./3D_Model.h"
#include "./../Utilities/OpenGLFrame.h"
#include "./../Utilities/vec3.h"
#include "./../Controllers/MapController.h"

class CharacterUnit
{
private:
	vec3f			m_vPosition;
	float			m_fYRotation;
	C3D_Model*		m_Model;

	bool			m_bMoving;
	vec3f			m_vMovePosition;

public:
	CharacterUnit();
	virtual ~CharacterUnit();

	virtual	bool	Initialize(char* szCharacterFile);
	virtual	void	Update(float fTime);
	virtual	void	Render(void);

			bool	RaySelect(COpenGLFrame& Cam, vec3f Ray);

	// Accessors
	inline	vec3f			GetPosition(void)							{	return m_vPosition;			}
	inline	vec3f			GetMovePosition(void)						{	return m_vMovePosition;		}
	inline	C3D_Model*		GetModel(void)								{	return m_Model;				}
	inline	bool			GetMoving(void)								{	return m_bMoving;			}

	// Modifiers
	inline	void			SetPosition(vec3f vPosition)				{	m_vPosition = vPosition;	CMapController::GetInstance()->PositionHeight(m_vPosition);	}
	inline	void			SetPosition(float X, float Z)				{	m_vPosition = vec3f(X, 0.0f, Z);	CMapController::GetInstance()->PositionHeight(m_vPosition);	}
	inline	void			SetMovePosition(vec3f vMove)				{	m_bMoving = true;	m_vMovePosition = vMove;	}
	inline	void			SetMovePosition(float X, float Z)			{	m_bMoving = true;	m_vMovePosition = vec3f(X, 0.0f, Z);	}
	inline	void			SetModel(C3D_Model* Model)					{	m_Model = Model;			}
};

#endif