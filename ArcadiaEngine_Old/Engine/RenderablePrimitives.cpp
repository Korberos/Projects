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
/*  File:       RenderablePrimitives.cpp                         */
/*                                                               */
/*  Purpose:    This file contains a collection of  renderable   */
/*              derivations for basic primitives. These can be   */
/*              used for simple rendering or debugging purposes. */
/*                                                               */
/*  Created:    10/25/2010                                       */
/*  Last Edit:  10/25/2010                                       */
/*****************************************************************/

#include "RenderablePrimitives.h"

#include <SDL_opengl.h>
#include "OpenGLCamera.h"

//  RenderableQuad2D
RenderableQuad2D::RenderableQuad2D() : 
	X( -1 ),
	Y( -1 ),
	Width( 0 ),
	Height( 0 ),
	Color( 0.0f, 0.0f, 0.0f )
{}


RenderableQuad2D::RenderableQuad2D( const int x, const int y, const int width, const int height, const Vector3<float>& color ) : 
	X( x ),
	Y( y ),
	Width( width ),
	Height( height ),
	Color( color )
{}


RenderableQuad2D::~RenderableQuad2D()
{}


void RenderableQuad2D::Render( void )
{
	//  Render the quad (2D)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( Color.x, Color.y, Color.z );
	glBegin( GL_QUADS );
		glVertex2d( X, Y );
		glVertex2d( X + Width, Y );
		glVertex2d( X + Width, Y + Height );
		glVertex2d( X, Y + Height );
	glEnd();
}


//  RenderableQuad3D
RenderableQuad3D::RenderableQuad3D() : 
	Dimensions( 0.0f, 0.0f, 0.0f ),
	Color( 0.0f, 0.0f, 0.0f )
{}


RenderableQuad3D::RenderableQuad3D( const Vector3<float>& position, const Vector3<float>& dimensions, const Vector3<float>& color ) : 
	Dimensions( dimensions ),
	Color( color ),
	BillboardCamera( NULL )
{
	Set_Position( position );
}


RenderableQuad3D::~RenderableQuad3D()
{}


void RenderableQuad3D::Render( void )
{
	//  TODO:  Billboard the objects to a camera pointer if they are set to

	//  For the purposes of our rendering code being temporary, we will create a make-shift half-vector of the Dimensions
	Vector3<float> half_dimensions = Dimensions / 2.0f;

	//  Render the quad (3D)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( Color.x, Color.y, Color.z );
	glBegin( GL_QUADS );
		glVertex3f( Position.x - half_dimensions.x, Position.y, Position.z - half_dimensions.z );
		glVertex3f( Position.x + half_dimensions.x, Position.y + Dimensions.y, Position.z - half_dimensions.z );
		glVertex3f( Position.x + half_dimensions.x, Position.y + Dimensions.y, Position.z + half_dimensions.z );
		glVertex3f( Position.x - half_dimensions.x, Position.y, Position.z + half_dimensions.z );
	glEnd();
}


//  RenderableDiamond
RenderableDiamond::RenderableDiamond() : 
	Dimensions( 0.0f, 0.0f, 0.0f ),
	Color( 0.0f, 0.0f, 0.0f )
{}


RenderableDiamond::RenderableDiamond( const Vector3<float>& position, const Vector3<float>& dimensions, const Vector3<float>& color ) : 
	Dimensions( dimensions ),
	Color( color )
{
	Set_Position( position );
}


RenderableDiamond::~RenderableDiamond()
{}


void RenderableDiamond::Render( void )
{
	//  Draw two intersecting diamond shapes
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( Color.x, Color.y, Color.z );
	glBegin( GL_QUADS );
		glVertex3f( Position.x, Position.y, Position.z );
		glVertex3f( Position.x + Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + Dimensions.y, Position.z );
		glVertex3f( Position.x - Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		
		glVertex3f( Position.x, Position.y, Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z + Dimensions.z );
		glVertex3f( Position.x, Position.y + Dimensions.y, Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z - Dimensions.z );
		
		glVertex3f( Position.x + Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z + Dimensions.z );
		glVertex3f( Position.x - Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z - Dimensions.z );
	glEnd();
	
	//  Redraw two intersecting diamond shapes as outlines
	glLineWidth( 1.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f( 0.0f, 0.0f, 0.0f );
	glBegin( GL_QUADS );
		glVertex3f( Position.x, Position.y, Position.z );
		glVertex3f( Position.x + Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + Dimensions.y, Position.z );
		glVertex3f( Position.x - Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		
		glVertex3f( Position.x, Position.y, Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z + Dimensions.z );
		glVertex3f( Position.x, Position.y + Dimensions.y, Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z - Dimensions.z );
		
		glVertex3f( Position.x + Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z + Dimensions.z );
		glVertex3f( Position.x - Dimensions.x, Position.y + ( Dimensions.y / 2.0f ), Position.z );
		glVertex3f( Position.x, Position.y + ( Dimensions.y / 2.0f ), Position.z - Dimensions.z );
	glEnd();
}