/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       Renderable.h                                     */
/*                                                               */
/*  Purpose:    This file contains a base renderable class       */
/*              meant to be over-written by renderable types     */
/*              that can be managed by game objects.             */
/*                                                               */
/*  Created:    10/25/2010                                       */
/*  Last Edit:  10/25/2010                                       */
/*****************************************************************/

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#include "Vector3.h"


//  Renderable
class Renderable
{
public:
	Renderable() :
		Position( 0.0f, 0.0f, 0.0f )
	{}

	~Renderable() {}

	static enum RenderableType { RENDERABLE_2D, RENDERABLE_3D };

	virtual const RenderableType Get_Renderable_Type( void ) const = 0;
	virtual void Render( void ) = 0;

	//  Accessors
	const Vector3<float> Get_Position( void )		const { return Position; }

	//  Modifiers
	void Set_Position( const Vector3<float>& position )		{ Position = position; }

protected:
	Vector3<float> Position;
};

#endif