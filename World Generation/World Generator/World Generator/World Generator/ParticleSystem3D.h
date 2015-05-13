#pragma once

#include <vector>
using namespace std;

#include "vec3.h"

#include "OGL_Functions.h"

class ParticleSystem3D
{
public:
	// Constructor / Destructor
	ParticleSystem3D();
	~ParticleSystem3D();

	// Functions
	void Initialize(char* EffectFile);
	void Update(float ElapsedTime);
	void Render(void);

private:
	bool Active;

	// Particle struct
	struct Particle3D
	{
	public:
		vec3f	Position;
		vec3f	Direction;
		vec3f	Color;
		float	Scale;
		float	Life;
	};

	// Flyweight Data
	vec3f	MinStartPosition;
	vec3f	MaxStartPosition;
	vec3f	MinDirection;
	vec3f	MaxDirection;
	vec3f	MinColor;
	vec3f	MaxColor;
	float	MinLifetime;
	float	MaxLifetime;
	float	StartScale;
	float	EndScale;
	float	ScalarGrowth;
	GLuint	ParticleTexture;

	float	MaxPointSize;
	float	Quadratic[3];

	// Maximum amount of particles to manage
	int MaxParticles;

	// Pointer (for the allocated array of particles)
	Particle3D* ParticleList;
};