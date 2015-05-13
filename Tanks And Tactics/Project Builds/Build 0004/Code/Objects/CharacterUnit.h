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

#include <SDL_opengl.h>
#include "./3D_Model.h"
#include "./../Utilities/OpenGLFrame.h"
#include "./../Utilities/vec3.h"

#define POS_X_LENGTH	0.05f
#define POS_Z_LENGTH	0.05f

struct MovementArea
{
	unsigned int	Row;
	unsigned int	Col;
	unsigned int	Path;
};

class CharacterUnit
{
private:
	unsigned int	m_nRow;
	unsigned int	m_nCol;
	unsigned int	m_nMove;
	float			m_fJump;

	vec3f			m_vPosition;
	float			m_fYRotation;
	C3D_Model*		m_Model;

	bool			m_bMoving;
	MovementArea	m_vMovePath;
	vec3f			m_vMovePosition;

public:
	CharacterUnit();
	virtual ~CharacterUnit();

	virtual	bool	Initialize(char* szCharacterFile);
	virtual	void	Update(float fTime);
	virtual	void	Render(void);

			bool	RaySelect(COpenGLFrame& Cam, vec3f Ray);

	void			SetPosition(unsigned int R, unsigned int C);
	void			SetMovePath(MovementArea Path);

	// Accessors
	inline	vec3f			GetPosition(void)							{	return m_vPosition;			}
	inline	vec3f			GetMovePosition(void)						{	return m_vMovePosition;		}
	inline	C3D_Model*		GetModel(void)								{	return m_Model;				}
	inline	bool			GetMoving(void)								{	return m_bMoving;			}
	inline	unsigned int	GetRow(void)								{	return m_nRow;				}
	inline	unsigned int	GetCol(void)								{	return m_nCol;				}
	inline	unsigned int	GetMoves(void)								{	return m_nMove;				}
	inline	float			GetJump(void)								{	return m_fJump;				}

	// Modifiers
	inline	void			SetMovePosition(vec3f vMove)				{	m_bMoving = true;	m_vMovePosition = vMove;	}
	inline	void			SetMovePosition(float X, float Z)			{	m_bMoving = true;	m_vMovePosition = vec3f(X, 0.0f, Z);	}
	inline	void			SetModel(C3D_Model* Model)					{	m_Model = Model;			}

private:
	inline	void			CalculateRowAndCol(void)					{	m_nRow = (unsigned int)(m_vPosition.x / POS_X_LENGTH);	m_nCol = (unsigned int)(m_vPosition.z / POS_Z_LENGTH);	}
};

#endif