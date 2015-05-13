#include "Map Control.h"

#include <fstream>
using namespace std;

MapControl::MapControl(void)
{
	m_nVertexCount		= 0;
	m_pVertexList		= 0;

	m_nTriangleCount	= 0;
	m_pTriangleList		= 0;

	m_pVolume			= 0;
}

MapControl::~MapControl(void)
{
	Shutdown();
}

MapControl* MapControl::GetInstance(void)
{
	static MapControl INSTANCE;
	return &INSTANCE;
}

bool MapControl::LoadMapFromFile(char* Filename)
{
	//  Open the file for reading
	fstream MapFile(Filename, ios_base::in | ios_base::binary);
	if ((MapFile.bad()) || (!MapFile.good()))
	{
		MapFile.close();
		return false;
	}

	//  Read in the vertex list
	MapFile.read((char*)&m_nVertexCount, sizeof(m_nVertexCount));
	m_pVertexList = new Vertex[m_nVertexCount];
	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		MapFile.read((char*)&m_pVertexList[i].Position.x, sizeof(m_pVertexList[i].Position.x));
		MapFile.read((char*)&m_pVertexList[i].Position.y, sizeof(m_pVertexList[i].Position.y));
		MapFile.read((char*)&m_pVertexList[i].Position.z, sizeof(m_pVertexList[i].Position.z));
	}

	//  Read in the triangle list
	MapFile.read((char*)&m_nTriangleCount, sizeof(m_nTriangleCount));
	m_pTriangleList = new Triangle[m_nTriangleCount];
	for (int i = 0; i < m_nTriangleCount; i += 1)
	{
		MapFile.read((char*)&m_pTriangleList[i].VertexID[0], sizeof(m_pTriangleList[i].VertexID[0]));
		MapFile.read((char*)&m_pTriangleList[i].VertexID[1], sizeof(m_pTriangleList[i].VertexID[1]));
		MapFile.read((char*)&m_pTriangleList[i].VertexID[2], sizeof(m_pTriangleList[i].VertexID[2]));
	}

	//  Close the file
	MapFile.close();

	if (m_nVertexCount == 0)	return false;
	if (m_nTriangleCount == 0)	return false;

	//  Find the extremes in the X and Z axis
	float X1	= m_pVertexList[0].Position.x;
	float Z1	= m_pVertexList[0].Position.z;
	float X2	= m_pVertexList[0].Position.x;
	float Z2	= m_pVertexList[0].Position.z;
	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		if (m_pVertexList[i].Position.x < X1)	X1 = m_pVertexList[i].Position.x;
		if (m_pVertexList[i].Position.x > X2)	X2 = m_pVertexList[i].Position.x;
		if (m_pVertexList[i].Position.z < Z1)	Z1 = m_pVertexList[i].Position.z;
		if (m_pVertexList[i].Position.z > Z2)	Z2 = m_pVertexList[i].Position.z;
	}
	float XL = X2 - X1;
	float ZL = Z2 - Z1;

	//  Compute the normals for each vertex
	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		vec3f Coordinates[3];
		for (int j = 0; j < 3; j += 1)
		{
			Coordinates[j].x = 0.0f;
			Coordinates[j].y = 0.0f;
			Coordinates[j].z = 0.0f;
		}
		int Count = 0;
		for (int j = 0; j < m_nTriangleCount; j += 1)
		{
			if ((m_pTriangleList[j].VertexID[0] != i) && (m_pTriangleList[j].VertexID[1] != i) && (m_pTriangleList[j].VertexID[2] != i)) continue;
			Count += 1;
			for (int k = 0; k < 3; k += 1)
				Coordinates[k] += m_pVertexList[m_pTriangleList[j].VertexID[k]].Position;
		}

		if (Count == 0) continue;
		for (int j = 0; j < 3; j += 1) Coordinates[j] /= ((float)Count);
	
		cross_product(m_pVertexList[i].Normal, (Coordinates[2] - Coordinates[0]), (Coordinates[1] - Coordinates[0]));
		m_pVertexList[i].Normal.normalize();
	}

	// Compute the texture coordinates for each vertex
	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		for (int j = 0; j < 3; j += 1)
		{
			m_pVertexList[i].TextureCoordinates[0] = (m_pVertexList[i].Position.z - Z1) / ZL;
			m_pVertexList[i].TextureCoordinates[1] = (m_pVertexList[i].Position.x - X1) / XL;
		}
	}

	//  Define all volumes
	m_pVolume = new Volume[1];
	DefineVolume(m_pVolume, X1, X2, Z1, Z2, 10);

	return true;
}

void MapControl::Update(GLFrame* Camera)
{
}

void MapControl::Render(Volume* V, bool Initialized)
{
	if (V == 0) return;
	if (Initialized == false)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_FILL);
		glPushMatrix();
			glBegin(GL_TRIANGLES);
	}

	//if (V->m_pChildren)
	//{
	//	Render(&V->m_pChildren[0], true);
	//	Render(&V->m_pChildren[1], true);
	//	Render(&V->m_pChildren[2], true);
	//	Render(&V->m_pChildren[3], true);
	//	return;
	//}
	//else
	{
		for (unsigned int i = 0; i < V->m_vTriangleList.size(); i += 1)
		{
			for (int j = 0; j < 3; j += 1)
			{
				glTexCoord2d(m_pVertexList[V->m_vTriangleList[i]->VertexID[j]].TextureCoordinates[0], m_pVertexList[V->m_vTriangleList[i]->VertexID[j]].TextureCoordinates[1]);
				glVertex3f(m_pVertexList[V->m_vTriangleList[i]->VertexID[j]].Position.x, m_pVertexList[V->m_vTriangleList[i]->VertexID[j]].Position.y, m_pVertexList[V->m_vTriangleList[i]->VertexID[j]].Position.z); 
			}
		}
	}

	if (Initialized == false)
	{
			glEnd();
		glPopMatrix();
	}
}

bool MapControl::Shutdown(void)
{
	if (m_pVertexList)		delete [] m_pVertexList;
	m_pVertexList = 0;

	if (m_pTriangleList)	delete [] m_pTriangleList;
	m_pTriangleList = 0;

	if (m_pVolume)			delete [] m_pVolume;
	m_pVolume = 0;

	return true;
}

size_t MapControl::DefineVolume(Volume* V, float X1, float X2, float Z1, float Z2, unsigned int SplitCount)
{
	V->X1 = X1;
	V->X2 = X2;
	V->Z1 = Z1;
	V->Z2 = Z2;
	V->XL = X2 - X1;
	V->ZL = Z2 - Z1;

	for (int i = 0; i < m_nTriangleCount; i += 1)
	{

		if (m_pVertexList[m_pTriangleList[i].VertexID[0]].Position.x < X1) continue;
		if (m_pVertexList[m_pTriangleList[i].VertexID[0]].Position.x > X2) continue;
		if (m_pVertexList[m_pTriangleList[i].VertexID[0]].Position.z < Z1) continue;
		if (m_pVertexList[m_pTriangleList[i].VertexID[0]].Position.z > Z2) continue;

		V->m_vTriangleList.push_back(&m_pTriangleList[i]);
	}

	if (V->m_vTriangleList.size() > SplitCount)
	{
		//V->m_vTriangleList.clear();
		V->m_pChildren = new Volume[4];
		DefineVolume(&V->m_pChildren[0], X1, X1 + ((X2 - X1) / 2.0f), Z1, Z1 + ((Z2 - Z1) / 2.0f), SplitCount);
		DefineVolume(&V->m_pChildren[1], X1 + ((X2 - X1) / 2.0f), X2, Z1, Z1 + ((Z2 - Z1) / 2.0f), SplitCount);
		DefineVolume(&V->m_pChildren[2], X1, X1 + ((X2 - X1) / 2.0f), Z1 + ((Z2 - Z1) / 2.0f), Z2, SplitCount);
		DefineVolume(&V->m_pChildren[3], X1 + ((X2 - X1) / 2.0f), X2, Z1 + ((Z2 - Z1) / 2.0f), Z2, SplitCount);
	}

	return V->m_vTriangleList.size();
}

void MapControl::RenderVolumes(Volume* V, float Y, bool Initialized)
{
	if (V == 0) return;

	if (Initialized == true)
	{
	glDisable(GL_TEXTURE_2D);
	glColor4f(V->Color.x, V->Color.y, V->Color.z, 1.0f);
	glPushMatrix();
		glBegin(GL_QUADS);
			glVertex3f(V->X1, Y, V->Z1);
			glVertex3f(V->X2, Y, V->Z1);
			glVertex3f(V->X2, Y, V->Z2);
			glVertex3f(V->X1, Y, V->Z2);
		glEnd();
	glPopMatrix();
	}

	if (V->m_pChildren == 0) return;
	RenderVolumes(&V->m_pChildren[0], Y + 0.05f);
	RenderVolumes(&V->m_pChildren[1], Y + 0.05f);
	RenderVolumes(&V->m_pChildren[2], Y + 0.05f);
	RenderVolumes(&V->m_pChildren[3], Y + 0.05f);

}