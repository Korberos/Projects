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

#include "Utilities/vec3.h"
#include <SDL/SDL_opengl.h>

#define X_LENGTH_MOD	0.05f
#define Z_LENGTH_MOD	0.05f

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
			vPosition.x = X;
			vPosition.y = Y;
			vPosition.z = Z;
		}
	};

	struct tMapTriangle
	{
	public:
		int		nVertex[3];
		vec3f	vNormal;
		void Define(int V1, int V2, int V3)
		{
			nVertex[0] = V1;
			nVertex[1] = V2;
			nVertex[2] = V3;
			CalculateNormal();
		}

		void CalculateNormal(void)
		{
			// Calculate the Normal
			CMapController* MAP = CMapController::GetInstance();
			cross_product(vNormal, (MAP->GetVertexList()[nVertex[1]].vPosition - MAP->GetVertexList()[nVertex[0]].vPosition), (MAP->GetVertexList()[nVertex[2]].vPosition - MAP->GetVertexList()[nVertex[0]].vPosition));
			vNormal.normalize();
			vNormal /= 20.0f;
		}
	};

	unsigned int	m_nHorizontal;			// The number of positions on the map from -X to X
	unsigned int	m_nVertical;			// The number of positions on the map from -Y to Y
	float			m_fXLength;				// The length of the map across the X axis
	float			m_fZLength;				// The length of the map across the Z axis
	unsigned int	m_nVertexCount;			// The number of vertices
	unsigned int	m_nTriangleCount;		// The number of triangles
	float*			m_pHeightList;			// The array of heights that make up the Level 1 geometry
	tMapVertex*		m_pVertexList;			// The array of vertices that make up the Level 2 geometry
	tMapTriangle*	m_pTriangleList;		// The array of triangles that make up the Level 2 geometry
	tMapTriangle*	m_pUnculledTriangles;	// The array of triangles left after a culling for the attack animation
	vec3f			m_WorldY;				// The World Up vector

	unsigned int	m_nDrawList_Geometry;
	unsigned int	m_nDrawList_GeometryOutline;
	unsigned int	m_nDrawList_VertexNormals;
	unsigned int	m_nDrawList_GeometryNormals;
	unsigned int	m_nDrawList_LowerGrid;
	unsigned int	m_nDrawList_UnculledTriangles;

	CMapController();
	~CMapController();
	CMapController*		INSTANCE;

public:
	static CMapController* GetInstance(void);

	bool LoadMap(char* MapFileName);

	bool MapIntersection(vec3f* Intersection, vec3f Position);

	void RenderAll(void);
	void RenderCull(void);

	// Accessors
	inline	unsigned int	GetTriangleCount(void)			{ return m_nTriangleCount;	}
	inline	tMapTriangle*	GetTriangleList(void)			{ return m_pTriangleList;	}
	inline	unsigned int	GetVertexCount(void)			{ return m_nVertexCount;	}
	inline	tMapVertex*		GetVertexList(void)				{ return m_pVertexList;		}

private:
	void ApplyVertex(unsigned int Index);

	void DetermineNormals(void);			// Determines the normals for all vertices
	void DrawList_Geometry(void);			// Creates a draw list of the geometry
	void DrawList_GeometryOutline(void);	// Creates a draw list of the outline of the geometry
	void DrawList_VertexNormals(void);	// Creates a draw list of lines representing normals of all vertices
	void DrawList_GeometryNormals(void);	// Creates a draw list of lines representing normals of all geometry
	void DrawList_LowerGrid(void);			// Creates a draw list of the blocks making up the lower grid
	void DrawList_CulledTriangles(vec3f vBegin, vec3f vEnd);
};

#endif