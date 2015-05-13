////////////////////////////////////////////////////////////////////////////////
//
//	Name:			MapController.cpp
//
//	Description:	This file is control for one instance of 
//					an in-game map. This controller will create, 
//					initialize, and fully render a map.
//
//	Created:		December 8th, 2008
//
//	Last Modified:	January 1st, 2009
//
////////////////////////////////////////////////////////////////////////////////

#include "MapController.h"

#include <fstream>

#include "./../Utilities/PointToCylinder.h"
#include "./../Utilities/LineToTriangle.h"

CMapController* CMapController::GetInstance(void)
{
	static CMapController INSTANCE;
	return &INSTANCE;
}

CMapController::CMapController()
{
		m_nHorizontal = 0;
		m_nVertical = 0;

		m_pHeightList = 0;
		m_pVertexList = 0;
		m_pTriangleList = 0;

		m_nDrawList_Geometry			= -1;
		m_nDrawList_GeometryOutline		= -1;
		m_nDrawList_VertexNormals		= -1;
		m_nDrawList_GeometryNormals		= -1;
		m_nDrawList_LowerGrid			= -1;
		m_nDrawList_UnculledTriangles	= -1;

		m_WorldY.x = 0.0f;
		m_WorldY.y = 1.0f;
		m_WorldY.z = 0.0f;
}

CMapController::~CMapController()
{
	delete [] m_pHeightList;
	delete [] m_pVertexList;
	delete [] m_pTriangleList;
	delete [] m_pUnculledTriangles;
}

bool CMapController::LoadMap(char *MapFileName)
{
	// Open the map file
	std::fstream ifMapStream(MapFileName, std::ios_base::binary || std::ios_base::in);
	if (ifMapStream.bad())		return false;
	if (!ifMapStream.good())	return false;

	// Read in the horizontal and vertical lengths
	ifMapStream.read((char*)&m_nHorizontal, sizeof(m_nHorizontal));
	ifMapStream.read((char*)&m_nVertical, sizeof(m_nVertical));

	// Create data based on the lengths
	m_nVertexCount		= m_nHorizontal * m_nVertical;
	m_nTriangleCount	= (((m_nHorizontal - 1) * 2) * (m_nVertical - 1));
	m_fXLength			= X_LENGTH_MOD * (float)m_nHorizontal;
	m_fZLength			= Z_LENGTH_MOD * (float)m_nVertical;

	// Allocate memory based on the lengths
	m_pHeightList		= new float[m_nHorizontal * m_nVertical];
	m_pVertexList		= new tMapVertex[m_nVertexCount];
	m_pTriangleList		= new tMapTriangle[m_nTriangleCount];

	// Read in all height data for the grid
	for (unsigned int i = 0; i < (m_nHorizontal * m_nVertical); i++)
		ifMapStream.read((char*)&m_pHeightList[i], sizeof(float));

	// Close the map file
	ifMapStream.close();

	// Define all Vertices based on the grid
	for (unsigned int i = 0; i < m_nHorizontal; i++)
		for (unsigned int j = 0; j < m_nVertical; j++)
			m_pVertexList[(i * m_nHorizontal) + j].Define(j * X_LENGTH_MOD, m_pHeightList[i * m_nHorizontal + j], i * Z_LENGTH_MOD);

	// Define all Triangles based on the lengths
	int nTriangleID = 0;
	for (unsigned int i = 0; i < (m_nVertical - 1); i++)
		for (unsigned int j = 0; j < (m_nHorizontal - 1); j++)
		{
			m_pTriangleList[nTriangleID++].Define((i * m_nHorizontal) + j, ((i + 1) * m_nHorizontal) + j, (i * m_nHorizontal) + j + 1);
			m_pTriangleList[nTriangleID++].Define(((i + 1) * m_nHorizontal) + j, (i * m_nHorizontal) + j + 1, ((i + 1) * m_nHorizontal) + j + 1);
		}

	// Define the normals of all Vertices
	DetermineNormals();

	// Build draw lists for later
	m_nDrawList_Geometry = glGenLists(4);
	m_nDrawList_GeometryOutline = m_nDrawList_Geometry + 1;
	m_nDrawList_VertexNormals = m_nDrawList_GeometryOutline + 1;
	m_nDrawList_GeometryNormals = m_nDrawList_VertexNormals + 1;
	m_nDrawList_LowerGrid = m_nDrawList_GeometryNormals + 1;

	// Write up all draw lists
	DrawList_Geometry();
	DrawList_GeometryOutline();
	DrawList_VertexNormals();
	DrawList_GeometryNormals();
	DrawList_LowerGrid();

	return true;
}

void CMapController::PositionHeight(vec3f& Position)
{
	//  If the position is outside the bounds of the map, return false.
	if (Position.x < 0.0f)			return;
	if (Position.z < 0.0f)			return;
	if (Position.x > m_fXLength)	return;
	if (Position.z > m_fZLength)	return;

	//  Create the start and end points to the line for testing
	vec3f Top		= Position + (m_WorldY * 5.0f);
	vec3f Bottom	= Position + (m_WorldY * -5.0f);

	//! Based on the position given, we can pin-point the two possible triangles the
	//! unit can be centered on based on the grid. From that, we can just do a basic
	//! LineToTriangle test on only two triangles to find the Intersection.

	vec3f IntersectPoint;
	unsigned int Row	= (unsigned int)(Position.z / m_fZLength * m_nVertical);
	unsigned int Column	= (unsigned int)(Position.x / m_fXLength * m_nHorizontal);
	
	if (Row >= m_nHorizontal - 1) return;
	if (Column >= m_nVertical - 1) return;

	for (unsigned int i = (Row * (m_nHorizontal - 1) * 2) + (Column * 2); i < (Row * (m_nHorizontal - 1) * 2) + (Column * 2) + 2; ++i)
	{
		if (LineToTriangle(IntersectPoint, tL2T(m_pVertexList[m_pTriangleList[i].nVertex[0]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[1]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[2]].vPosition), Top, Bottom) == true)
		{
			Position.y = IntersectPoint.y;
			break;
		}
	}
}

bool CMapController::RayCollision(vec3f& Point1, vec3f& Point2, vec3f& Intersect)
{
	//! Loop through all triangles in the map and do a LineToTriangle test to find
	//! out if the ray collides. If it does, define the intersection point and
	//! return true. If no triangles pass the collision check, return false.

	for (unsigned int i = 0; i < m_nTriangleCount; ++i)
		if (LineToTriangle(Intersect, tL2T(m_pVertexList[m_pTriangleList[i].nVertex[0]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[1]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[2]].vPosition), Point1, Point2) == true)
			return true;

	return false;
}

void CMapController::CalculateCulled(vec3f Start, vec3f End)
{
	DrawList_CulledTriangles(Start, End);
}

void CMapController::RenderAll(void)
{
	// Set the fill the geometry when rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Enable the 2D Texture module in OpenGL
	glEnable(GL_TEXTURE_2D);

	// Render the geometry (triangles)
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallList(m_nDrawList_Geometry);
	
	// Render the outline of the geometry
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor3f(0.0f, 0.0f, 0.0f);
	glCallList(m_nDrawList_GeometryOutline);

	//// Render the underlying space grid
	//glLineWidth(3);
	//glDisable(GL_TEXTURE_2D);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glCallList(m_nDrawList_LowerGrid);

	//// Render the geometry (triangles) normals
	//glLineWidth(2);
	//glColor3f(0.0f, 0.0f, 1.0f);
	//glCallList(m_nDrawList_GeometryNormals);
}

void CMapController::RenderCulled(void)
{
	// Set the fill the geometry when rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Enable the 2D Texture module in OpenGL
	glEnable(GL_TEXTURE_2D);

	// Render the geometry (triangles)
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallList(m_nDrawList_UnculledTriangles);
	
	// Render the outline of the geometry
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0.0f, 0.0f, 0.0f, 0.03f);
	glCallList(m_nDrawList_GeometryOutline);
}

void CMapController::ApplyVertex(unsigned int Index)
{
	glNormal3f(m_pVertexList[Index].vNormal.x, m_pVertexList[Index].vNormal.y, m_pVertexList[Index].vNormal.z);
	glTexCoord2d(m_pVertexList[Index].vPosition.x / m_fXLength, m_pVertexList[Index].vPosition.z / m_fZLength);
	glVertex3f(m_pVertexList[Index].vPosition.x, m_pVertexList[Index].vPosition.y, m_pVertexList[Index].vPosition.z);		
}

void CMapController::ApplyTriangle(unsigned int Index)
{
	ApplyVertex(m_pTriangleList[Index].nVertex[0]);
	ApplyVertex(m_pTriangleList[Index].nVertex[1]);
	ApplyVertex(m_pTriangleList[Index].nVertex[2]);		
}


void CMapController::DetermineNormals(void)
{
	// Determine the normals of all vertices based on the triangles they connect with
	for (unsigned int i = 0; i < m_nVertexCount; i++)
	{
		vec3f Coordinates[3];
		for (int j = 0; j < 3; j += 1)
		{
			Coordinates[j].x = 0.0f;
			Coordinates[j].y = 0.0f;
			Coordinates[j].z = 0.0f;
		}
		int nCount = 0;
		for (unsigned int j = 0; j < m_nTriangleCount; j++)
		{
			if ((m_pTriangleList[j].nVertex[0] != i) && (m_pTriangleList[j].nVertex[1] != i) && (m_pTriangleList[j].nVertex[2] != i)) continue;
			nCount += 1;
			for (int k = 0; k < 3; k++)
				Coordinates[k] += m_pVertexList[m_pTriangleList[j].nVertex[k]].vPosition;
		}

		if (nCount == 0) continue;
		for (int j = 0; j < 3; j++) Coordinates[j] /= ((float)nCount);
	
		cross_product(m_pVertexList[i].vNormal, (Coordinates[1] - Coordinates[0]), (Coordinates[2] - Coordinates[0]));
		m_pVertexList[i].vNormal.normalize();
		m_pVertexList[i].vNormal /= 6.0f;
	}
}

void CMapController::DrawList_Geometry()
{
	// Create the Draw List for the basic geometry
	glNewList(m_nDrawList_Geometry, GL_COMPILE);
		glBegin(GL_TRIANGLE_STRIP);
			for (unsigned int i = 0; i < (m_nVertical - 1); i++)
			{
				ApplyVertex(i * m_nHorizontal);
				ApplyVertex((i + 1) * m_nHorizontal);
				for (unsigned int j = 0; j < (m_nHorizontal - 1); j++)
				{
					ApplyVertex((i * m_nHorizontal) + j + 1);
					ApplyVertex((i + 1) * m_nHorizontal + j + 1);
				}
				if (i < (m_nVertical - 2))
				{
					ApplyVertex((i + 1) * m_nHorizontal + m_nHorizontal - 1);
					ApplyVertex((i + 2) * m_nHorizontal);
					ApplyVertex((i + 2) * m_nHorizontal);
				}
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_GeometryOutline()
{
	// Create the Draw List for the lines connecting the vertices in the geometry
	glNewList(m_nDrawList_GeometryOutline, GL_COMPILE);
		glBegin(GL_LINES);
			for (unsigned int i = 0; i < m_nTriangleCount; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					glVertex3f(m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.x, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.y + 0.0001f, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.z);
					glVertex3f(m_pVertexList[m_pTriangleList[i].nVertex[(j+1)%3]].vPosition.x, m_pVertexList[m_pTriangleList[i].nVertex[(j+1)%3]].vPosition.y + 0.0001f, m_pVertexList[m_pTriangleList[i].nVertex[(j+1)%3]].vPosition.z);
				}
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_VertexNormals()
{
	// Create the Draw List for the lines representing vertex normals
	glNewList(m_nDrawList_VertexNormals, GL_COMPILE);
		glBegin(GL_LINES);
			for (unsigned int i = 0; i < m_nVertexCount; i++)
			{
				glVertex3f(m_pVertexList[i].vPosition.x, m_pVertexList[i].vPosition.y, m_pVertexList[i].vPosition.z);
				glVertex3f(m_pVertexList[i].vPosition.x + m_pVertexList[i].vNormal.x, m_pVertexList[i].vPosition.y + m_pVertexList[i].vNormal.y, m_pVertexList[i].vPosition.z + m_pVertexList[i].vNormal.z);
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_GeometryNormals()
{
	// Create the Draw List for the lines representing vertex normals
	glNewList(m_nDrawList_GeometryNormals, GL_COMPILE);
		glBegin(GL_LINES);
			for (unsigned int i = 0; i < m_nTriangleCount; i++)
			{
				vec3f TriangleCenter = (m_pVertexList[m_pTriangleList[i].nVertex[0]].vPosition + m_pVertexList[m_pTriangleList[i].nVertex[1]].vPosition + m_pVertexList[m_pTriangleList[i].nVertex[2]].vPosition) / 3.0f;
				glVertex3f(TriangleCenter.x, TriangleCenter.y, TriangleCenter.z);
				glVertex3f(TriangleCenter.x + m_pTriangleList[i].vNormal.x, TriangleCenter.y + m_pTriangleList[i].vNormal.y, TriangleCenter.z + m_pTriangleList[i].vNormal.z);
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_LowerGrid()
{
	// Create the Draw List for the lower grid, an array of quads representing spaces for movement
	glNewList(m_nDrawList_LowerGrid, GL_COMPILE);
		glBegin(GL_QUADS);
			for (unsigned int i = 0; i < m_nHorizontal; i++)
				for (unsigned int j = 0; j < m_nVertical; j++)
				{
					glVertex3f((-m_fXLength * 0.5f) + j * X_LENGTH_MOD, m_pHeightList[i * m_nHorizontal + j], (-m_fZLength * 0.5f) + i * Z_LENGTH_MOD);
					glVertex3f((-m_fXLength * 0.5f) + (j + 1) * X_LENGTH_MOD, m_pHeightList[i * m_nHorizontal + j], (-m_fZLength * 0.5f) + i * Z_LENGTH_MOD);
					glVertex3f((-m_fXLength * 0.5f) + (j + 1) * X_LENGTH_MOD, m_pHeightList[i * m_nHorizontal + j], (-m_fZLength * 0.5f) + (i + 1) * Z_LENGTH_MOD);
					glVertex3f((-m_fXLength * 0.5f) + j * X_LENGTH_MOD, m_pHeightList[i * m_nHorizontal + j], (-m_fZLength * 0.5f) + (i + 1) * Z_LENGTH_MOD);
				}
		glEnd();
	glEndList();
}

void CMapController::DrawList_CulledTriangles(vec3f vBegin, vec3f vEnd)
{
	// Move both Begin and End back from eachother (1.1 times)
	vec3f vDirection = (vBegin - vEnd) /= 10.0f;
	vBegin	+= vDirection;
	vEnd	-= vDirection;

	// Create the Draw List for the triangles within the area being shown during an attack
	glNewList(m_nDrawList_UnculledTriangles, GL_COMPILE);
		glBegin(GL_TRIANGLES);
			for (unsigned int i = 0; i < m_nTriangleCount; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					if (Point_Cylinder_Collision_IgnoreHeight(vBegin, vEnd, 0.2f, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition))
					{
						ApplyTriangle(i);
						break;
					}
				}
			}
		glEnd();
	glEndList();
}