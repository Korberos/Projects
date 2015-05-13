#pragma once

#include <vector>
using namespace std;

#include "../../Utilities/vec3.h"
#include "../../Utilities/OpenGL/OGL_Includes.h"

class MapControl
{
private:
	MapControl();
	~MapControl();

	struct Vertex
	{
	public:
		vec3f	Position;
		vec3f	Normal;
		float	TextureCoordinates[2];
	};

	struct Triangle
	{
	public:
		int		VertexID[3];
	};

	struct Volume
	{
	public:
		Volume()
		{
			m_pChildren = 0;
			Color.x = (rand() % 1000) / 1000.0f;
			Color.y = (rand() % 1000) / 1000.0f;
			Color.y = (rand() % 1000) / 1000.0f;
		}
		~Volume()
		{
			if (m_pChildren) delete [] m_pChildren;
		}

		float X1;
		float X2;
		float Z1;
		float Z2;
		float XL;
		float ZL;
		vec3f Color;
		vector<Triangle*>	m_vTriangleList;
		Volume*				m_pChildren;
	};

	int			m_nVertexCount;
	Vertex*		m_pVertexList;

	int			m_nTriangleCount;
	Triangle*	m_pTriangleList;

	Volume*		m_pVolume;

public:
	static MapControl* GetInstance(void);

	bool LoadMapFromFile(char* Filename);
	void Update(GLFrame* Camera);
	void Render(Volume* V = 0, bool Initialized = false);
	bool Shutdown(void);

	size_t DefineVolume(Volume* V, float X1, float X2, float Z1, float Z2, unsigned int SplitCount);
	void RenderVolumes(Volume* V = 0, float Y = 1.0f, bool Begun = true);

	Volume* GetMainVolume(void)		{	return m_pVolume;	}
};