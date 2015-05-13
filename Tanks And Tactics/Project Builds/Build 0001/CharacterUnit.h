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

#include "Utilities/vec3.h"
#include "3D_Model.h"
#include <SDL/SDL_opengl.h>

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

	virtual bool	Initialize(char* szCharacterFile);
	virtual void	Update(float fTime);
	virtual void	Render(void);

	// Accessors
	inline	vec3f			GetPosition(void)					{	return m_vPosition;			}
	inline	C3D_Model*		GetModel(void)						{	return m_Model;				}

	// Modifiers
	inline	void			SetPosition(vec3f vPosition)		{	m_vPosition = vPosition;	}
	inline	void			SetModel(C3D_Model* Model)			{	m_Model = Model;			}
};

#endif