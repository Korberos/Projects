#include <fstream>
using namespace std;

#include <iostream>
using namespace std;

void WriteVector(fstream& F, float X, float Y, float Z);

void main(void)
{
	// Create an fstream binary file for writing to
	fstream F("EffectTest.bin", ios_base::binary|ios_base::out);

	// Write out the amount of points and each each vertex point in order
	int MaxParticles = 400;
	F.write((char*)&MaxParticles, sizeof(int));

	WriteVector(F, 0.0f,	2.0f,	0.0f);		// Minimum Start Position
	WriteVector(F, 10.24f,	4.0f,	10.24f);	// Maximum Start Position

	WriteVector(F, -0.4f,	-1.0f,	-0.4f);		// Minimum Direction
	WriteVector(F, 0.4f,	-0.5f,	0.4f);		// Maximum Direction

	WriteVector(F, 0.8f,	0.8f,	0.8f);		// Minimum Color
	WriteVector(F, 1.0f,	1.0f,	1.0f);		// Maximum Color

	float MinLifetime = 5.0f;
	F.write((char*)&MinLifetime, sizeof(float));

	float MaxLifetime = 1.0f;
	F.write((char*)&MaxLifetime, sizeof(float));

	WriteVector(F, 1.0f,	2.0f,	0.1f);		// Minimum Scale, Maximum Scale, and Scalar Growth

	char ParticleImage[32];
	strcpy_s(ParticleImage, sizeof("Particle.bmp"), "Particle.bmp");
	F.write((char*)&ParticleImage, sizeof(ParticleImage));

	F.close();
}

void WriteVector(fstream& F, float X, float Y, float Z)
{
	F.write((char*)&X, 		sizeof(float));
	F.write((char*)&Y, 		sizeof(float));
	F.write((char*)&Z, 		sizeof(float));
}