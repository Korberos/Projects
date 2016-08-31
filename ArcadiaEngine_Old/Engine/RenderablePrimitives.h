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
/*  File:       RenderablePrimitives.h                           */
/*                                                               */
/*  Purpose:    This file contains a collection of  renderable   */
/*              derivations for basic primitives. These can be   */
/*              used for simple rendering or debugging purposes. */
/*                                                               */
/*  Created:    10/25/2010                                       */
/*  Last Edit:  10/25/2010                                       */
/*****************************************************************/

#ifndef _RENDERABLE_PRIMITIVES_H_
#define _RENDERABLE_PRIMITIVES_H_

#include "Renderable.h"

class COpenGLFrame;

//  RenderableQuad2D
class RenderableQuad2D : public Renderable
{
public:
	RenderableQuad2D();
	RenderableQuad2D( const int x, const int y, const int width, const int height, const Vector3<float>& color );
	~RenderableQuad2D();

	virtual const RenderableType Get_Renderable_Type( void ) const { return RENDERABLE_2D; }

	virtual void Render( void );

	//  Accessors
	const int Get_X( void )							const { return X; }
	const int Get_Y( void )							const { return Y; }
	const int Get_Width( void )					const { return Width; }
	const int Get_Height( void )					const { return Height; }
	const Vector3<float> Get_Color( void )		const { return Color; }

	//  Modifiers
	void Set_X( int x )										{ X = x; }
	void Set_Y( int y )										{ Y = y; }
	void Set_Width( int width )							{ Width = width; }
	void Set_Height( int height )							{ Height = height; }
	void Set_Color( Vector3<float>& color )			{ Color = color; }

private:
	int X;
	int Y;
	int Width;
	int Height;
	Vector3<float> Color;
};


//  RenderableQuad3D
class RenderableQuad3D : public Renderable
{
public:
	RenderableQuad3D();
	RenderableQuad3D( const Vector3<float>& Position, const Vector3<float>& dimensions, const Vector3<float>& color );
	~RenderableQuad3D();

	virtual const RenderableType Get_Renderable_Type( void ) const { return RENDERABLE_3D; }

	virtual void Render( void );

	//  Accessors
	const Vector3<float> Get_Dimensions( void )	const { return Dimensions; }
	const Vector3<float> Get_Color( void )			const { return Color; }
	COpenGLFrame*& Get_Camera( void )						{ return BillboardCamera; }
	const bool Get_Billboard_X( void )				const { return BillboardX; }
	const bool Get_Billboard_Z( void )				const { return BillboardZ; }

	//  Modifiers
	void Set_Dimension( Vector3<float>& dimensions )	{ Dimensions = dimensions ; }
	void Set_Color( Vector3<float>& color )				{ Color = color; }
	void Set_Camera( COpenGLFrame* camera )				{ BillboardCamera = camera; }
	void Set_Billboard_X( bool billboard_x )				{ BillboardX = billboard_x; }
	void Set_Billboard_Z( bool billboard_z )				{ BillboardZ = billboard_z; }

private:
	Vector3<float> Dimensions;
	Vector3<float> Color;
	COpenGLFrame* BillboardCamera;
	bool BillboardX;
	bool BillboardZ;
};


//  RenderableQuad3D
class RenderableDiamond : public Renderable
{
public:
	RenderableDiamond();
	RenderableDiamond( const Vector3<float>& Position, const Vector3<float>& dimensions, const Vector3<float>& color );
	~RenderableDiamond();

	virtual const RenderableType Get_Renderable_Type( void ) const { return RENDERABLE_3D; }

	virtual void Render( void );

	//  Accessors
	const Vector3<float> Get_Dimensions( void )	const { return Dimensions; }
	const Vector3<float> Get_Color( void )			const { return Color; }

	//  Modifiers
	void Set_Dimension( Vector3<float>& dimensions )	{ Dimensions = dimensions ; }
	void Set_Color( Vector3<float>& color )				{ Color = color; }

private:
	Vector3<float> Dimensions;
	Vector3<float> Color;
};

#endif