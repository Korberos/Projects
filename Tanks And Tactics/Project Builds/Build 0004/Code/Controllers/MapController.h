////////////////////////////////////////////////////////////////////////////////
//
//	Name:			MapController.h
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

#ifndef _DB_MAPCONTROLLER_
#define _DB_MAPCONTROLLER_

#include <SDL_opengl.h>
#include <fstream>
#include <vector>
#include "./../Objects/CharacterUnit.h"
#include "./../Utilities/vec3.h"
#include "./../Utilities/PointToCylinder.h"
#include "./../Utilities/LineToTriangle.h"
#include "./../Utilities/RandomNumber.h"

class CMapController
{
private:
	struct tMapVertex
	{
	public:
		vec3f	vPosition;
		vec3f	vNormal;
		void Define(float X, float Y, float Z)
		{
			vPosition = vec3f(X, Y, Z);
			vNormal = vec3f(0.0f, 0.0f, 0.0f);
		}
	};

	struct tMapTriangle
	{
	public:
		unsigned int	nVertex[3];
		vec3f			vNormal;
		void Define(unsigned int V1, unsigned int V2, unsigned int V3)
		{
			nVertex[0] = V1;
			nVertex[1] = V2;
			nVertex[2] = V3;
			CMapController::GetInstance()->DetermineTriangleNormal(this);
		}
	};

	struct tMapPalette
	{
		unsigned int	W;
		unsigned int	H;
		unsigned char*	ColorList;

		tMapPalette()
		{
			W = 0;
			H = 0;
			ColorList = 0;
		}

		~tMapPalette()
		{
			if (ColorList) delete [] ColorList;
		}
	};

	

	unsigned int	m_nHorizontal;			// The number of positions on the map from -X to X
	unsigned int	m_nVertical;			// The number of positions on the map from -Y to Y
	float			m_fXLength;				// The length of the map across the X axis
	float			m_fZLength;				// The length of the map across the Z axis
	float			m_fLowestY;				// The lowest Y value in the map
	float			m_fHighestY;			// The highest Y value in the map

	vec3f			m_vWorldY;				// The World Up vector
	tMapPalette		m_Palette;				// The color palette for the world

	unsigned int	m_nVertexCount;			// The number of vertices
	unsigned int	m_nTriangleCount;		// The number of triangles
	float*			m_pHeightList;			// The array of heights that make up the Level 1 geometry
	tMapVertex*		m_pVertexList;			// The array of vertices that make up the Level 2 geometry
	tMapTriangle*	m_pTriangleList;		// The array of triangles that make up the Level 2 geometry

	std::vector<MovementArea> m_vCurrentPaths;	// The pathfinding data for the current unit

	enum			m_eDrawLists	{ GEOMETRY = 0, GEOMETRYOUTLINE, VERTEXNORMALS, GEOMETRYNORMALS, LOWERGRID, UNCULLEDTRIANGLES, DRAWLIST_COUNT };
	unsigned int	m_nDrawList[DRAWLIST_COUNT];

	unsigned int	m_nGridSelection_Row;
	unsigned int	m_nGridSelection_Col;
	float			m_fGridSelection_X;
	float			m_fGridSelection_Y;
	float			m_fGridSelection_Z;

public:
	static CMapController* GetInstance(void);

	bool LoadMap(char* MapFileName);

	void PositionHeight(vec3f& Position);
	bool RayCollision(vec3f& Point1, vec3f& Point2, vec3f& Intersect);
	void CalculateCulled(vec3f Start, vec3f End);
	void CalculateGrid(CharacterUnit* Char = 0);

	void RenderAll(void);
	void RenderCulled(void);
	void RenderGrid(void);

	bool IsMovePossible(vec3f Position, MovementArea& M);		// Will return the MovementArea struct if a position can be moved to
	void GridSelection(unsigned int R, unsigned int C);			// Defines the grid selection area to be rendered

	// Accessors
	inline	unsigned int	GetHorizontal(void)				{ return m_nHorizontal;		}
	inline	unsigned int	GetVertical(void)				{ return m_nVertical;		}


private:
	CMapController();
	~CMapController();

	void ApplyVertex(unsigned int Index);
	void ApplyTriangle(unsigned int Index);
	
	void LoadColorPalette(char* PaletteFile);					// Loads the color palette for the map

	void DetermineTriangleNormal(tMapTriangle* T);				// Determines the normal of a triangle
	void DetermineVertexNormals(void);							// Determines the normals for all vertices
	void DrawList_Geometry(void);								// Creates a draw list of the geometry
	void DrawList_GeometryOutline(void);						// Creates a draw list of the outline of the geometry
	void DrawList_VertexNormals(void);							// Creates a draw list of lines representing normals of all vertices
	void DrawList_GeometryNormals(void);						// Creates a draw list of lines representing normals of all geometry
	void DrawList_LowerGrid(void);								// Creates a draw list of the blocks making up the lower grid
	void DrawList_CulledTriangles(vec3f vBegin, vec3f vEnd);	// Creates a draw list of the culled triangles (when aiming)

	void FindMoves(CharacterUnit* Char);						// Creates a list of possible paths and returns the list
};

#endif