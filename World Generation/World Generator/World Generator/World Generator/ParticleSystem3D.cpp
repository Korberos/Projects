#include "ParticleSystem3D.h"

#include <fstream>
using namespace std;

#define RandomFloat(min, max) (((rand()/(float)RAND_MAX)*((max)-(min)))+(min))

ParticleSystem3D::ParticleSystem3D()
{
	Active = false;
}

ParticleSystem3D::~ParticleSystem3D()
{
	delete [] ParticleList;
}

void ParticleSystem3D::Initialize(char* EffectFile)
{
	fstream File(EffectFile, ios_base::in|ios_base::binary);
	if ((File.bad()) || (!File.good()))
	{
		ParticleList = 0;
		File.close();
		return;
	}

	File.read((char*)&MaxParticles, sizeof(int));
	ParticleList = new Particle3D[MaxParticles];

	File.read((char*)&MinStartPosition.x, sizeof(float));
	File.read((char*)&MinStartPosition.y, sizeof(float));
	File.read((char*)&MinStartPosition.z, sizeof(float));
	
	File.read((char*)&MaxStartPosition.x, sizeof(float));
	File.read((char*)&MaxStartPosition.y, sizeof(float));
	File.read((char*)&MaxStartPosition.z, sizeof(float));
	
	File.read((char*)&MinDirection.x, sizeof(float));
	File.read((char*)&MinDirection.y, sizeof(float));
	File.read((char*)&MinDirection.z, sizeof(float));
	
	File.read((char*)&MaxDirection.x, sizeof(float));
	File.read((char*)&MaxDirection.y, sizeof(float));
	File.read((char*)&MaxDirection.z, sizeof(float));
	
	File.read((char*)&MinColor.x, sizeof(float));
	File.read((char*)&MinColor.y, sizeof(float));
	File.read((char*)&MinColor.z, sizeof(float));
	
	File.read((char*)&MaxColor.x, sizeof(float));
	File.read((char*)&MaxColor.y, sizeof(float));
	File.read((char*)&MaxColor.z, sizeof(float));

	File.read((char*)&MinLifetime, sizeof(float));
	File.read((char*)&MaxLifetime, sizeof(float));

	File.read((char*)&StartScale, sizeof(float));
	File.read((char*)&EndScale, sizeof(float));
	File.read((char*)&ScalarGrowth, sizeof(float));

	char TextureFile[32];
	File.read((char*)&TextureFile, sizeof(TextureFile));
	LoadAndBindPNG(TextureFile, &ParticleTexture);

	for (int i = 0; i < MaxParticles; i += 1) ParticleList[i].Life = 0.0f;

	File.close();

	// Set up prerequisites for point sprites
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &MaxPointSize);
	MaxPointSize = min(MaxPointSize, 10.0f);

	glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
	glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

	Active = true;
}

void ParticleSystem3D::Update(float ElapsedTime)
{
	for (int i = 0; i < MaxParticles; i += 1)
	{
		if (ParticleList[i].Life > 0.0f)
		{
			ParticleList[i].Position += ParticleList[i].Direction * ElapsedTime;

			ParticleList[i].Life -= ElapsedTime;

			if (ParticleList[i].Scale < EndScale) ParticleList[i].Scale += ScalarGrowth;
		}
		else
		{
			ParticleList[i].Position.x = RandomFloat(MinStartPosition.x, MaxStartPosition.x);
			ParticleList[i].Position.y = RandomFloat(MinStartPosition.y, MaxStartPosition.y);
			ParticleList[i].Position.z = RandomFloat(MinStartPosition.z, MaxStartPosition.z);

			ParticleList[i].Direction.x = RandomFloat(MinDirection.x, MaxDirection.x);
			ParticleList[i].Direction.y = RandomFloat(MinDirection.y, MaxDirection.y);
			ParticleList[i].Direction.z = RandomFloat(MinDirection.z, MaxDirection.z);

			ParticleList[i].Color.x = RandomFloat(MinColor.x, MaxColor.x);
			ParticleList[i].Color.y = RandomFloat(MinColor.y, MaxColor.y);
			ParticleList[i].Color.z = RandomFloat(MinColor.z, MaxColor.z);

			ParticleList[i].Scale = StartScale;

			ParticleList[i].Life = RandomFloat(MinLifetime, MaxLifetime);
		}
	}
}

void ParticleSystem3D::Render(void)
{
	if (Active == false) return;

	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);
    glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
    glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, MaxPointSize);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ParticleTexture);

	//glDisable(GL_LIGHTING);
	glPointSize(MaxPointSize);
	glBegin(GL_POINTS);
	for (int i = 0; i < MaxParticles; i += 1)
	{
		if (ParticleList[i].Life < 0.0f) continue;

		glColor3f(ParticleList[i].Color.x, ParticleList[i].Color.y, ParticleList[i].Color.z);
		glVertex3f(ParticleList[i].Position.x, 	ParticleList[i].Position.y, ParticleList[i].Position.z);
	}
	glEnd();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_POINT_SPRITE_ARB);
}