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