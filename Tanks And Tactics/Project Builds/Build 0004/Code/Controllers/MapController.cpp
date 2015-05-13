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

#include <windows.h>
#include <stdio.h>

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

		for (unsigned int i = 0; i < DRAWLIST_COUNT; ++i)
			m_nDrawList[i] = -1;

		m_vWorldY = vec3f(0.0f, 1.0f, 0.0f);
}

CMapController::~CMapController()
{
	delete [] m_pHeightList;
	delete [] m_pVertexList;
	delete [] m_pTriangleList;

	glDeleteLists(m_nDrawList[0], DRAWLIST_COUNT);
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
	m_nVertexCount		= ((m_nHorizontal + 1) * (m_nVertical + 1)) + m_nHorizontal * m_nVertical;
	m_nTriangleCount	= m_nHorizontal * m_nVertical * 4;
	m_fXLength			= POS_X_LENGTH * (float)m_nHorizontal;
	m_fZLength			= POS_X_LENGTH * (float)m_nVertical;

	// Allocate memory based on the lengths
	m_pHeightList		= new float[m_nHorizontal * m_nVertical];
	m_pVertexList		= new tMapVertex[m_nVertexCount];
	m_pTriangleList		= new tMapTriangle[m_nTriangleCount];

	// Read in all height data for the grid
	for (unsigned int i = 0; i < (m_nHorizontal * m_nVertical); ++i)
		ifMapStream.read((char*)&m_pHeightList[i], sizeof(float));

	// Close the map file
	ifMapStream.close();

	// Define all Vertices based on the grid
	for (unsigned int j = 0; j < m_nVertical + 1; ++j)
	{
		for (unsigned int i = 0; i < m_nHorizontal + 1; ++i)
		{
			// For each vertex, determine if there is a grid position in each direction, and take the average of what exists
			unsigned int	Count = 0;
			float			Height = 0.0f;

			// Left Side
			if (i > 0)
			{
				// Upper Side
				if (j > 0)
				{
					++Count;
					Height += m_pHeightList[(i - 1) + (m_nHorizontal * (j - 1))];
				}

				// Lower Side
				if (j < m_nVertical)
				{
					++Count;
					Height += m_pHeightList[(i - 1) + (m_nHorizontal * j)];
				}
			}
			
			// Right Side
			if (i < m_nHorizontal)
			{
				// Upper Side
				if (j > 0)
				{
					++Count;
					Height += m_pHeightList[i + (m_nHorizontal * (j - 1))];
				}

				// Lower Side
				if (j < m_nVertical)
				{
					++Count;
					Height += m_pHeightList[i + (m_nHorizontal * j)];
				}
			}

			// Determine the average height
			Height /= (float)Count;

			// Define the vertex
			m_pVertexList[i + ((m_nHorizontal + 1) * j)].Define(i * POS_X_LENGTH, Height, j * POS_Z_LENGTH);

			// Update the lowest and highest values of the map
			if (Height < m_fLowestY)	m_fLowestY = Height;
			if (Height > m_fHighestY)	m_fHighestY = Height;
		}
	}
	for (unsigned int i = 0; i < m_nHorizontal * m_nVertical; ++i)
		m_pVertexList[i + ((m_nHorizontal + 1) * (m_nVertical + 1))].Define((i % m_nHorizontal) * POS_X_LENGTH + (POS_X_LENGTH / 2.0f), m_pHeightList[i], (i / m_nHorizontal) * POS_Z_LENGTH + (POS_Z_LENGTH / 2.0f));

	// Define all Triangles based on the lengths
	for (unsigned int i = 0; i < m_nHorizontal * m_nVertical; ++i)
	{
		unsigned int Row = i % m_nHorizontal;
		unsigned int Col = i / m_nHorizontal;
		unsigned int Tri = i * 4;

		unsigned int V0;
		unsigned int V1;
		unsigned int V2;

		// Triangle 1 out of 4
		V0 = ((Row + 0) + (m_nHorizontal + 1) * (Col + 0));
		V1 = ((m_nHorizontal + 1) * (m_nVertical + 1)) + i;
		V2 = ((Row + 0) + (m_nHorizontal + 1) * (Col + 1));
		m_pTriangleList[Tri + 0].Define(V0, V1, V2);

		// Triangle 2 out of 4
		V0 = ((Row + 0) + (m_nHorizontal + 1) * (Col + 0));
		V1 = ((Row + 1) + (m_nHorizontal + 1) * (Col + 0));
		V2 = ((m_nHorizontal + 1) * (m_nVertical + 1)) + i;
		m_pTriangleList[Tri + 1].Define(V0, V1, V2);

		// Triangle 3 out of 4
		V0 = ((Row + 1) + (m_nHorizontal + 1) * (Col + 0));
		V1 = ((Row + 1) + (m_nHorizontal + 1) * (Col + 1));
		V2 = ((m_nHorizontal + 1) * (m_nVertical + 1)) + i;
		m_pTriangleList[Tri + 2].Define(V0, V1, V2);

		// Triangle 4 out of 4
		V0 = ((Row + 0) + (m_nHorizontal + 1) * (Col + 1));
		V1 = ((m_nHorizontal + 1) * (m_nVertical + 1)) + i;
		V2 = ((Row + 1) + (m_nHorizontal + 1) * (Col + 1));
		m_pTriangleList[Tri + 3].Define(V0, V1, V2);
	}

	// Define the normals of all Vertices
	DetermineVertexNormals();

	// Generate draw lists for later
	m_nDrawList[0] = glGenLists(DRAWLIST_COUNT);
	for (unsigned int i = 1; i < DRAWLIST_COUNT; ++i)
		m_nDrawList[i] = m_nDrawList[i - 1] + 1;
	
	// Load in the color palette
	LoadColorPalette("./Assets/Winter_Palette.tatpal");

	// Write up all draw lists (except Lower Grid)
	DrawList_Geometry();
	DrawList_GeometryOutline();
	DrawList_VertexNormals();
	DrawList_GeometryNormals();

	return true;
}

void CMapController::PositionHeight(vec3f& Position)
{
	//  If the position is outside the bounds of the map, return false.
	if (Position.x <= 0.0f)			return;
	if (Position.z <= 0.0f)			return;
	if (Position.x >= m_fXLength)	return;
	if (Position.z >= m_fZLength)	return;

	//  Create the start and end points to the line for testing
	vec3f Top		= Position + (m_vWorldY * 5.0f);
	vec3f Bottom	= Position + (m_vWorldY * -5.0f);

	//! Based on the position given, we can pin-point the four possible triangles the
	//! unit can be centered on based on the grid. From that, we can just do a basic
	//! LineToTriangle test on only four triangles to find the Intersection.

	vec3f IntersectPoint;
	unsigned int Row	= (unsigned int)(Position.x / POS_X_LENGTH);
	unsigned int Column	= (unsigned int)(Position.z / POS_Z_LENGTH);
	
	if  ((Row >= m_nHorizontal) || (Column >= m_nVertical) || (Row < 0) ||	(Column < 0))
	{
		return;
	}

	for (unsigned int i = (Row + (m_nHorizontal * Column)) * 4; i < (Row + (m_nHorizontal * Column)) * 4 + 4; ++i)
	{
		if (LineToTriangle(IntersectPoint, tL2T(m_pVertexList[m_pTriangleList[i].nVertex[0]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[1]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[2]].vPosition), Top, Bottom) == true)
		{
			Position.y = IntersectPoint.y + 0.001f;
			break;
		}
	}
}

bool CMapController::RayCollision(vec3f& Point1, vec3f& Point2, vec3f& Intersect)
{
	//! Loop through all triangles in the map and do a LineToTriangle test to find
	//! out if the ray collides. If it does, define the intersection point and add
	//! it to a list. Afterwards, go through the list and define the returned point
	//! as the one closest to Point1

	bool Collision = false;
	std::vector<vec3f> Intersections;

	for (unsigned int i = 0; i < m_nTriangleCount; ++i)
	{
		if (LineToTriangle(Intersect, tL2T(m_pVertexList[m_pTriangleList[i].nVertex[0]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[1]].vPosition, m_pVertexList[m_pTriangleList[i].nVertex[2]].vPosition), Point1, Point2) == true)
		{
			Intersections.push_back(Intersect);
			Collision = true;
		}
	}

	if (Intersections.empty()) return false;
	
	Intersect = Intersections[0];
	for (unsigned int i = 1; i < Intersections.size(); ++i)
		if ((Intersect - Point1).magnitude() > (Intersections[i] - Point1).magnitude())
			Intersect = Intersections[i];

	return true;
}


void CMapController::CalculateCulled(vec3f Start, vec3f End)
{
	DrawList_CulledTriangles(Start, End);
}

void CMapController::CalculateGrid(CharacterUnit* Char)
{
	GridSelection(Char->GetRow(), Char->GetCol());
	FindMoves(Char);
	DrawList_LowerGrid();
}

void CMapController::RenderAll(void)
{
	// Set the fill the geometry when rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render the geometry (triangles)
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallList(m_nDrawList[GEOMETRY]);
	
	// Render the outline of the geometry
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0.0f, 0.0f, 0.0f, 0.05f);
	glCallList(m_nDrawList[GEOMETRYOUTLINE]);
}

void CMapController::RenderCulled(void)
{
	// Set the fill the geometry when rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render the geometry (triangles)
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallList(m_nDrawList[UNCULLEDTRIANGLES]);
	
	// Render the outline of the geometry
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0.0f, 0.0f, 0.0f, 0.025f);
	glCallList(m_nDrawList[GEOMETRYOUTLINE]);
}

void CMapController::RenderGrid(void)
{
	// Set the fill the geometry when rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render the geometry (triangles)
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
	glCallList(m_nDrawList[GEOMETRY]);
	
	// Render the outline of the geometry
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0.0f, 0.0f, 0.0f, 0.05f);
	glCallList(m_nDrawList[GEOMETRYOUTLINE]);

	// Render the lower grid
	glDisable(GL_DEPTH_TEST);
	glLineWidth(2);
	glColor4f(1.0f, 0.0f, 1.0f, 0.3f);
	glCallList(m_nDrawList[LOWERGRID]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4f(0.3f, 0.0f, 1.0f, 0.6f);
	glBegin(GL_QUADS);
		glVertex3f(m_fGridSelection_X, m_fGridSelection_Y, m_fGridSelection_Z);
		glVertex3f(m_fGridSelection_X + POS_X_LENGTH, m_fGridSelection_Y, m_fGridSelection_Z);
		glVertex3f(m_fGridSelection_X + POS_X_LENGTH, m_fGridSelection_Y, m_fGridSelection_Z + POS_Z_LENGTH);
		glVertex3f(m_fGridSelection_X, m_fGridSelection_Y, m_fGridSelection_Z + POS_Z_LENGTH);
	glEnd();
	glEnable(GL_DEPTH_TEST);
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

void CMapController::LoadColorPalette(char* PaletteFile)
{
	// Open the palette file
	std::fstream Pal(PaletteFile, std::ios_base::in | std::ios_base::binary);
	if (Pal.bad())		return;
	if (!Pal.good())	return;

	// Delete any current palette color list
	if (m_Palette.ColorList) delete [] m_Palette.ColorList;

	// Read in the Width and Height
	Pal.read((char*)&m_Palette.W, sizeof(unsigned int));
	Pal.read((char*)&m_Palette.H, sizeof(unsigned int));

	// Allocate the color list
	m_Palette.ColorList = new unsigned char[m_Palette.W * m_Palette.H * 3];

	// Read in the color list
	Pal.read((char*)m_Palette.ColorList, sizeof(unsigned char) * m_Palette.W * m_Palette.H * 3);
}

void CMapController::DetermineTriangleNormal(tMapTriangle* T)
{
	// Calculate the Normal
	cross_product(T->vNormal, (m_pVertexList[T->nVertex[1]].vPosition - m_pVertexList[T->nVertex[0]].vPosition), (m_pVertexList[T->nVertex[2]].vPosition - m_pVertexList[T->nVertex[0]].vPosition));
	T->vNormal.normalize();
}

void CMapController::DetermineVertexNormals(void)
{
	// Determine the normals of all vertices based on the triangles they connect with
	for (unsigned int i = 0; i < m_nVertexCount; i++)
	{
		vec3f Coordinates[3];
		for (int j = 0; j < 3; j += 1) Coordinates[j] = vec3f(0.0f, 0.0f, 0.0f);

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
	float Divide = (m_fHighestY - m_fLowestY) / (float)m_Palette.W;
	unsigned int Row;
	unsigned int Col;
	float R, G, B;
	float TriangleHeight;

	for ( unsigned int i = 0; i < m_nHorizontal * m_nVertical; ++i )
	{
		wchar_t float_string[64];
		_snwprintf_s( float_string, 63, L"%f \n", m_pHeightList[i] );
		OutputDebugString( float_string );
	}

	// Create the Draw List for the basic geometry
	glNewList(m_nDrawList[GEOMETRY], GL_COMPILE);
		glBegin(GL_TRIANGLES);
			for (unsigned int i = 0; i < m_nTriangleCount; ++i)
			{
				if (m_Palette.ColorList != 0)
				{
					TriangleHeight = 0.0f;
					for (unsigned int j = 0; j < 3; ++j) TriangleHeight += m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.y;
					TriangleHeight /= 3.0f;
					
					Row = std::min((unsigned int)((TriangleHeight - m_fLowestY) / Divide), (unsigned int)19);
					Col = (unsigned int)RandInt(0, m_Palette.H - 1);
					R = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 0] / 255.0f);
					G = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 1] / 255.0f);
					B = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 2] / 255.0f);
					glColor4f(R, G, B, 1.0f);
				}
				ApplyTriangle(i);
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_GeometryOutline()
{
	// Create the Draw List for the lines connecting the vertices in the geometry
	glNewList(m_nDrawList[GEOMETRYOUTLINE], GL_COMPILE);
		glBegin(GL_LINES);
			for (unsigned int i = 0; i < m_nTriangleCount; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					glVertex3f(m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.x, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.y + 0.0001f, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.z);
					glVertex3f(m_pVertexList[m_pTriangleList[i].nVertex[(j+1) % 3]].vPosition.x, m_pVertexList[m_pTriangleList[i].nVertex[(j+1) % 3]].vPosition.y + 0.0005f, m_pVertexList[m_pTriangleList[i].nVertex[(j+1) % 3]].vPosition.z);
				}
			}
		glEnd();
	glEndList();
}

void CMapController::DrawList_VertexNormals()
{
	// Create the Draw List for the lines representing vertex normals
	glNewList(m_nDrawList[VERTEXNORMALS], GL_COMPILE);
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
	glNewList(m_nDrawList[GEOMETRYNORMALS], GL_COMPILE);
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

void CMapController::DrawList_LowerGrid(void)
{
	// Render the boxes, given the positions that the unit can move to
	glNewList(m_nDrawList[LOWERGRID], GL_COMPILE);
		glBegin(GL_LINES);
			for (unsigned int k = 0; k < m_vCurrentPaths.size(); ++k)
			{
				unsigned int i = m_vCurrentPaths[k].Row;
				unsigned int j = m_vCurrentPaths[k].Col;

				glVertex3f((i * POS_X_LENGTH), m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH));
				glVertex3f((i * POS_X_LENGTH) + POS_X_LENGTH, m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH));

				glVertex3f((i * POS_X_LENGTH) + POS_X_LENGTH, m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH));
				glVertex3f((i * POS_X_LENGTH) + POS_X_LENGTH, m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH) + POS_Z_LENGTH);
				
				glVertex3f((i * POS_X_LENGTH) + POS_X_LENGTH, m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH) + POS_Z_LENGTH);
				glVertex3f((i * POS_X_LENGTH), m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH) + POS_Z_LENGTH);
				
				glVertex3f((i * POS_X_LENGTH), m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH) + POS_Z_LENGTH);
				glVertex3f((i * POS_X_LENGTH), m_pHeightList[i + (m_nHorizontal * j)] + 0.05f, (j * POS_Z_LENGTH));
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

	float Divide = (m_fHighestY - m_fLowestY) / (float)m_Palette.W;
	unsigned int Row;
	unsigned int Col;
	float R, G, B;
	float TriangleHeight;

	// Create the Draw List for the basic geometry
	glNewList(m_nDrawList[UNCULLEDTRIANGLES], GL_COMPILE);
		glBegin(GL_TRIANGLES);
			for (unsigned int i = 0; i < m_nTriangleCount; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					if (Point_Cylinder_Collision_IgnoreHeight(vBegin, vEnd, 0.025f, m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition))
					{
						if (m_Palette.ColorList != 0)
						{
							TriangleHeight = 0.0f;
							for (unsigned int j = 0; j < 3; ++j) TriangleHeight += m_pVertexList[m_pTriangleList[i].nVertex[j]].vPosition.y;
							TriangleHeight /= 3.0f;
							
							Row = std::min((unsigned int)((TriangleHeight - m_fLowestY) / Divide), (unsigned int)19);
							Col = (unsigned int)RandInt(0, m_Palette.H - 1);
							R = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 0] / 255.0f);
							G = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 1] / 255.0f);
							B = (float)(m_Palette.ColorList[(Col * m_Palette.W + Row) * 3 + 2] / 255.0f);
							glColor4f(R, G, B, 1.0f);
						}
						ApplyTriangle(i);
						break;
					}
				}
			}
		glEnd();
	glEndList();
}

void CMapController::FindMoves(CharacterUnit* Char)
{
	// If no character is passed in, return an empty vector
	if (Char == 0) return;

	// Create all required objects
	std::vector<MovementArea>	Pathfinders;
	std::vector<MovementArea>	Pathfinders_Remove;
	std::vector<MovementArea>	Pathfinders_Add;
	bool UnusedArea;
	unsigned int				nRemainingMoves;

	// Before anything, clear the m_vCurrentPaths so that a new path list can fill it's place
	m_vCurrentPaths.clear();

	// Create the starter MovementArea where the player is and push it into the Pathfinders list
	MovementArea	StartPosition;
	StartPosition.Row	= Char->GetRow();
	StartPosition.Col	= Char->GetCol();
	StartPosition.Path	= 0;
	Pathfinders.push_back(StartPosition);
	m_vCurrentPaths.push_back(StartPosition);

	//! The number of moves needs to be limited to 9 due to the path being kept as 
	//! an unsigned int. If this needs to be altered later, it can be.
	nRemainingMoves = Char->GetMoves();
	if (nRemainingMoves > 9) nRemainingMoves = 9;
	
	// Begin Pathfinding
	while (Pathfinders.size())
	{
		// Remove any old MovementAreas
		while (Pathfinders_Remove.size())
		{
			for (unsigned int i = 0; i < Pathfinders.size(); ++i)
				if (Pathfinders[i].Row == Pathfinders_Remove.back().Row)
					if (Pathfinders[i].Col == Pathfinders_Remove.back().Col)
					{
						Pathfinders[i] = Pathfinders.back();
						Pathfinders.pop_back();
						break;
					}
			Pathfinders_Remove.pop_back();
		}

		// Add any new MovementAreas
		while (Pathfinders_Add.size())
		{
			Pathfinders.push_back(Pathfinders_Add.back());
			Pathfinders_Add.pop_back();
		}

		if (nRemainingMoves == 0) break;
		nRemainingMoves -= 1;

		// Find new areas, and eliminate any old areas
		for (unsigned int i = 0; i < Pathfinders.size(); ++i)
		{
			// Create a path LEFT if possible (-Row)
			if (Pathfinders[i].Row > 0)
			{
				// If the new position is already in the Pathfinders list, don't add it
				UnusedArea = true;
				for (unsigned int j = 0; j < Pathfinders.size(); ++j)
					if (Pathfinders[j].Row == Pathfinders[i].Row - 1)
						if (Pathfinders[j].Col == Pathfinders[i].Col)
						{
							UnusedArea = false;
							break;
						}

				// If the new position is already in the Pathfinders_Add list, don't add it
				if (UnusedArea == true)
				{
					for (unsigned int j = 0; j < Pathfinders_Add.size(); ++j)
						if (Pathfinders_Add[j].Row == Pathfinders[i].Row - 1)
							if (Pathfinders_Add[j].Col == Pathfinders[i].Col)
							{
								UnusedArea = false;
								break;
							}
				}

				// If the position has not been discovered, add it
				if (UnusedArea == true)
				{
					MovementArea NewArea;
					NewArea = Pathfinders[i];
					NewArea.Row -= 1;
					NewArea.Path = NewArea.Path * 10 + 1;
					Pathfinders_Add.push_back(NewArea);
					m_vCurrentPaths.push_back(NewArea);
				}
			}

			// Create a path RIGHT if possible (+Row)
			if (Pathfinders[i].Row < m_nHorizontal - 1)
			{
				// If the new position is already in the Pathfinders list, don't add it
				UnusedArea = true;
				for (unsigned int j = 0; j < Pathfinders.size(); ++j)
					if (Pathfinders[j].Row == Pathfinders[i].Row + 1)
						if (Pathfinders[j].Col == Pathfinders[i].Col)
						{
							UnusedArea = false;
							break;
						}

				// If the new position is already in the Pathfinders_Add list, don't add it
				if (UnusedArea == true)
				{
					for (unsigned int j = 0; j < Pathfinders_Add.size(); ++j)
						if (Pathfinders_Add[j].Row == Pathfinders[i].Row + 1)
							if (Pathfinders_Add[j].Col == Pathfinders[i].Col)
							{
								UnusedArea = false;
								break;
							}
				}

				// If the position has not been discovered, add it
				if (UnusedArea == true)
				{
					MovementArea NewArea;
					NewArea = Pathfinders[i];
					NewArea.Row += 1;
					NewArea.Path = NewArea.Path * 10 + 2;
					Pathfinders_Add.push_back(NewArea);
					m_vCurrentPaths.push_back(NewArea);
				}
			}

			// Create a path UP if possible (-Col)
			if (Pathfinders[i].Col > 0)
			{
				// If the new position is already in the Pathfinders list, don't add it
				UnusedArea = true;
				for (unsigned int j = 0; j < Pathfinders.size(); ++j)
					if (Pathfinders[j].Row == Pathfinders[i].Row)
						if (Pathfinders[j].Col == Pathfinders[i].Col - 1)
						{
							UnusedArea = false;
							break;
						}

				// If the new position is already in the Pathfinders_Add list, don't add it
				if (UnusedArea == true)
				{
					for (unsigned int j = 0; j < Pathfinders_Add.size(); ++j)
						if (Pathfinders_Add[j].Row == Pathfinders[i].Row)
							if (Pathfinders_Add[j].Col == Pathfinders[i].Col - 1)
							{
								UnusedArea = false;
								break;
							}
				}

				// If the position has not been discovered, add it
				if (UnusedArea == true)
				{
					MovementArea NewArea;
					NewArea = Pathfinders[i];
					NewArea.Col -= 1;
					NewArea.Path = NewArea.Path * 10 + 3;
					Pathfinders_Add.push_back(NewArea);
					m_vCurrentPaths.push_back(NewArea);
				}
			}

			// Create a path DOWN if possible (-Col)
			if (Pathfinders[i].Col < m_nVertical - 1)
			{
				// If the new position is already in the Pathfinders list, don't add it
				UnusedArea = true;
				for (unsigned int j = 0; j < Pathfinders.size(); ++j)
					if (Pathfinders[j].Row == Pathfinders[i].Row)
						if (Pathfinders[j].Col == Pathfinders[i].Col + 1)
						{
							UnusedArea = false;
							break;
						}

				// If the new position is already in the Pathfinders_Add list, don't add it
				if (UnusedArea == true)
				{
					for (unsigned int j = 0; j < Pathfinders_Add.size(); ++j)
						if (Pathfinders_Add[j].Row == Pathfinders[i].Row)
							if (Pathfinders_Add[j].Col == Pathfinders[i].Col + 1)
							{
								UnusedArea = false;
								break;
							}
				}

				// If the position has not been discovered, add it
				if (UnusedArea == true)
				{
					MovementArea NewArea;
					NewArea = Pathfinders[i];
					NewArea.Col += 1;
					NewArea.Path = NewArea.Path * 10 + 4;
					Pathfinders_Add.push_back(NewArea);
					m_vCurrentPaths.push_back(NewArea);
				}
			}

			// Remove the current path from the list
			Pathfinders_Remove.push_back(Pathfinders[i]);
		}
	}
}

bool CMapController::IsMovePossible(vec3f Position, MovementArea& M)
{
	unsigned int Row = (unsigned int)(Position.x / POS_X_LENGTH);
	unsigned int Col = (unsigned int)(Position.z / POS_Z_LENGTH);

	for (unsigned int i = 0; i < m_vCurrentPaths.size(); ++i)
	{
		if (m_vCurrentPaths[i].Row == Row)
			if (m_vCurrentPaths[i].Col == Col)
			{
				M = m_vCurrentPaths[i];
				return true;
			}
	}

	return false;
}

void CMapController::GridSelection(unsigned int R, unsigned int C)
{
	if ((m_nGridSelection_Row != R) || (m_nGridSelection_Col != C))
	{
		m_nGridSelection_Row = R;
		m_nGridSelection_Col = C;
		m_fGridSelection_X = ((float)R) * POS_X_LENGTH;
		m_fGridSelection_Z = ((float)C) * POS_Z_LENGTH;
		vec3f GetY = vec3f(m_fGridSelection_X + (POS_X_LENGTH * 0.5f), 0.0f, m_fGridSelection_Z + (POS_Z_LENGTH * 0.5f));
		PositionHeight(GetY);
		m_fGridSelection_Y = GetY.y + 0.05f;
	}
}