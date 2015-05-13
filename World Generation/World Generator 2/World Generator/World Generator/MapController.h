#pragma once

#include "OGL_Includes.h"
#include "vec3.h"
#include "ParticleSystem3D.h"

#define VERTICES			3
#define	OGL_GEOMETRY_TYPE	GL_TRIANGLES

struct MC_Geometry
{
	int		VertexID[VERTICES];
};

struct MC_Vertex
{
	vec3f	Position;
	vec3f	Normal;
};

class MapController
{
private:
	bool			Active;

	GLuint			DL_Geometry;
	GLuint			DL_Lines;
	GLuint			DL_Normals;

	int				VertexCount;
	MC_Vertex*		Vertices;

	int				GeometryCount;
	MC_Geometry*	Geometry;

	float 			X_Lowest;
	float 			X_Highest;
	float 			X_Length;
	float 			Z_Lowest;
	float 			Z_Highest;
	float 			Z_Length;

	// Particle Systems within the map
	int					ParticleSystemCount;
	ParticleSystem3D*	ParticleSystems;



public:
	MapController(char* MapFile);
	~MapController(void);

	void Update(float ElapsedTime);
	void Render(void);

	inline float			Get_X_Lowest(void)			{	return X_Lowest;		}
	inline float			Get_X_Highest(void)			{	return X_Highest;		}
	inline float			Get_Z_Lowest(void)			{	return Z_Lowest;		}
	inline float			Get_Z_Highest(void)			{	return Z_Highest;		}

	inline int				Get_VertexCount(void)		{	return VertexCount;		}
	inline MC_Vertex*		Get_Vertices(void)			{	return Vertices;		}

	inline int				Get_GeometryCount(void)		{	return GeometryCount;	}
	inline MC_Geometry*		Get_Geometry(void)			{	return Geometry;		}
};
