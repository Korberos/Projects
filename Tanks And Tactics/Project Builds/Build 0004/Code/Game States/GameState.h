////////////////////////////////////////////////////////////////////////////////
//
//	Name:			GameState.h
//
//	Description:	This file contains the parent class for all
//					Game States within the program. The states
//					will contain virtual function overloads from
//					this class and will all publicly derive from it
//
//	Created:		January 20th, 2008
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _DB_GAMESTATE_
#define _DB_GAMESTATE_

class CGameState
{
public:
	virtual void	Initialize(void)		= 0;
	virtual bool	Input(void)				= 0;
	virtual int		Update(void)			= 0;
	virtual void	Render3D(void)			= 0;
	virtual void	Render2D(void)			= 0;
};

#endif