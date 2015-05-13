#include "MapController.h"

#include <fstream>
using namespace std;

#include <vector>
using namespace std;

#define KEYDOWN(key) ((GetAsyncKeyState(key) & 0x8000) ? true : false)

MapController::MapController(char* MapFile)
{
	////////////////////////////////////////
	/**/// Open the map file for reading
	////////////////////////////////////////

	// Open the map file, checking if it is successful
	fstream File(MapFile, ios_base::binary|ios_base::in);
	if ((File.bad()) || (!File.good()))
	{
		Active = false;
		File.close();
		return;
	}
	Active = true;

	////////////////////////////////////////
	/**/// Read in the vertex list
	////////////////////////////////////////

	// Read in the number of Vertices
	File.read((char*)&VertexCount, sizeof(int));
	Vertices = new MC_Vertex[VertexCount];

	// Read in each Vertex
	for (int i = 0; i < VertexCount; i += 1)
	{
		File.read((char*)&Vertices[i].Position.x, sizeof(float));
		File.read((char*)&Vertices[i].Position.y, sizeof(float));
		File.read((char*)&Vertices[i].Position.z, sizeof(float));
	}

	////////////////////////////////////////
	/**/// Read in the geometry unit list
	////////////////////////////////////////

	// Read in the number of Geometry units
	File.read((char*)&GeometryCount, sizeof(int));
	Geometry = new MC_Geometry[GeometryCount];

	// Read in each Geometry unit
	for (int i = 0; i < GeometryCount; i += 1)
		for (int j = 0; j < VERTICES; j += 1)
			File.read((char*)&Geometry[i].VertexID[j], sizeof(int));

	////////////////////////////////////////
	/**/// Read in any particle functions
	////////////////////////////////////////
	File.read((char*)&ParticleSystemCount, sizeof(int));
	ParticleSystems = new ParticleSystem3D[ParticleSystemCount];

	for (int i = 0; i < ParticleSystemCount; i += 1)
	{
		char EffectFile[32];
		File.read((char*)&EffectFile, sizeof(EffectFile));
		ParticleSystems[i].Initialize(EffectFile);
	}

	////////////////////////////////////////
	/**/// Close the map file
	////////////////////////////////////////
	
	File.close();

	////////////////////////////////////////
	/**/// Find the X and Z extremes
	////////////////////////////////////////

	// Find the extremes on the X and Z axis
	X_Lowest	= Vertices[0].Position.x;
	X_Highest	= Vertices[0].Position.x;
	Z_Lowest	= Vertices[0].Position.z;
	Z_Highest	= Vertices[0].Position.z;

	for (int i = 0; i < VertexCount; i += 1)
	{
		if (Vertices[i].Position.x > X_Highest)		X_Highest	= Vertices[i].Position.x;
		if (Vertices[i].Position.x < X_Lowest)		X_Lowest	= Vertices[i].Position.x;
		if (Vertices[i].Position.z > Z_Highest)		Z_Highest	= Vertices[i].Position.z;
		if (Vertices[i].Position.z < Z_Lowest)		Z_Lowest	= Vertices[i].Position.z;
	}

	X_Length	= X_Highest - X_Lowest;
	Z_Length	= Z_Highest - Z_Lowest;
	
	////////////////////////////////////////
	/**/// Calculate the normal for each vertex
	////////////////////////////////////////

	for (int i = 0; i < VertexCount; i += 1)
	{
		vec3f	Coordinates[VERTICES];
		for (int j = 0; j < VERTICES; j += 1)
		{
			Coordinates[j].x = 0.0f;
			Coordinates[j].y = 0.0f;
			Coordinates[j].z = 0.0f;
		}
		int Count = 0;
		for (int j = 0; j < GeometryCount; j += 1)
		{
			if ((Geometry[j].VertexID[0] != i) && (Geometry[j].VertexID[1] != i) && (Geometry[j].VertexID[2] != i)) continue;
			Count += 1;
			for (int k = 0; k < VERTICES; k += 1)
				Coordinates[k] += Vertices[Geometry[j].VertexID[k]].Position;
		}

		if (Count == 0) continue;
		for (int j = 0; j < VERTICES; j += 1) Coordinates[j] /= ((float)Count);
	
		cross_product(Vertices[i].Normal, (Coordinates[2] - Coordinates[0]), (Coordinates[1] - Coordinates[0]));
		Vertices[i].Normal.normalize();
	}

	////////////////////////////////////////
	/**/// Create the draw lists
	////////////////////////////////////////

	// Allocate memory for all draw lists
	DL_Geometry		= glGenLists(2);
	DL_Lines		= DL_Geometry	+ 1;
	DL_Normals		= DL_Lines		+ 1;

	// Create the DL_Geometry list (All Geometry units)
	glNewList(DL_Geometry, GL_COMPILE);
		glBegin(OGL_GEOMETRY_TYPE);
			for (int i = 0; i < GeometryCount; i += 1)
			{
				for (int j = 0; j < VERTICES; j += 1)
				{
					glNormal3f(Vertices[Geometry[i].VertexID[j]].Normal.x, Vertices[Geometry[i].VertexID[j]].Normal.y, Vertices[Geometry[i].VertexID[j]].Normal.z);
					glTexCoord2d((Vertices[Geometry[i].VertexID[j]].Position.z - Z_Lowest) / Z_Length, (Vertices[Geometry[i].VertexID[j]].Position.x - X_Lowest) / X_Length);
					glVertex3f(Vertices[Geometry[i].VertexID[j]].Position.x, Vertices[Geometry[i].VertexID[j]].Position.y, Vertices[Geometry[i].VertexID[j]].Position.z);
				}
			}
		glEnd();
	glEndList();

	// Create the DL_Lines list (Lines connecting vertices)
	glNewList(DL_Lines, GL_COMPILE);
		glBegin(GL_LINES);
			for (int i = 0; i < GeometryCount; i += 1)
			{
				for (int j = 0; j < VERTICES; j += 1)
				{
					glVertex3f(Vertices[Geometry[i].VertexID[j]].Position.x, Vertices[Geometry[i].VertexID[j]].Position.y, Vertices[Geometry[i].VertexID[j]].Position.z);
					glVertex3f(Vertices[Geometry[i].VertexID[(j+1)%VERTICES]].Position.x, Vertices[Geometry[i].VertexID[(j+1)%VERTICES]].Position.y, Vertices[Geometry[i].VertexID[(j+1)%VERTICES]].Position.z);
				}
			}
		glEnd();
	glEndList();

	// Create the DL_Normals list (Normal indicators for each vertex)
	glNewList(DL_Normals, GL_COMPILE);
		glBegin(GL_LINES);
			for (int i = 0; i < VertexCount; i += 1)
			{
				glVertex3f(Vertices[i].Position.x, Vertices[i].Position.y, Vertices[i].Position.z);
				glVertex3f(Vertices[i].Position.x + Vertices[i].Normal.x, Vertices[i].Position.y + Vertices[i].Normal.y, Vertices[i].Position.z + Vertices[i].Normal.z);
			}
		glEnd();
	glEndList();
}

MapController::~MapController(void)
{
	// Free all allocated memory
	if (Active == true)
	{
		glDeleteLists(DL_Geometry, 2);
		delete [] Geometry;
		delete [] Vertices;
		delete [] ParticleSystems;
	}
}

void MapController::Update(float ElapsedTime)
{
	for (int i = 0; i < ParticleSystemCount; i += 1) ParticleSystems[i].Update(ElapsedTime);
}

void MapController::Render(void)
{
	if (Active == false) return;

	if (KEYDOWN(VK_RSHIFT))	glShadeModel(GL_FLAT);
	else					glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallList(DL_Geometry);

	if (KEYDOWN(VK_LSHIFT))
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glLineWidth(2);
		glColor4f(0.4f, 0.4f, 1.0f, 1.0f);
		glCallList(DL_Lines);
	}

	if (KEYDOWN(VK_SPACE))
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glLineWidth(2);
		glColor4f(0.6f, 0.6f, 0.2f, 1.0f);
		glCallList(DL_Normals);
	}
}