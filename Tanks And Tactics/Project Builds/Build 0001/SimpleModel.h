////////////////////////////////////////////////////////////////////////////////
//
//	Name:			SimpleModel.h
//
//	Description:	This file is a self-controlled model to be 
//					used by way of rendering within the game.
//					It contains a list of triangles and draws 
//					them when called to do so.
//
//	Created:		December 17th, 2008
//
//	Last Modified:	December 18th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_SIMPLEMODEL_
#define _DB_SIMPLEMODEL_

#include <SDL/SDL_opengl.h>
#include <vector>
#include <fstream>

#include "Utilities/vec3.h"

class CSimpleModel
{
private:
	struct tModelVertex
	{
		vec3f vPosition;
		vec3f vNormal;
		void Define(float X, float Y, float Z)
		{
			vPosition.x = X;
			vPosition.y = Y;
			vPosition.z = Z;
		}
	};

	struct tModelTriangle
	{
		int nVertex[3];
		void Define(int V1, int V2, int V3)
		{
			nVertex[0] = V1;
			nVertex[1] = V2;
			nVertex[2] = V3;
		}
	};

	struct tModelPart
	{
		char				m_szName[256];
		unsigned int		m_nVertexCount;
		unsigned int		m_nTriangleCount;
		tModelVertex*		m_pVertexList;
		tModelTriangle*		m_pTriangleList;

	};

	std::vector<tModelPart>	m_vPartList;
	unsigned int			m_nDrawList_Model;

public:
	CSimpleModel(char* szModelFile)
	{
		tModelPart*			ModelPart = 0;

		// Open the model file
		std::fstream ifModelStream(szModelFile, std::ios_base::binary | std::ios_base::in);
		if (ifModelStream.bad())	return;
		if (!ifModelStream.good())	return;

		while (!ifModelStream.eof())
		{
			char Check = ifModelStream.peek();
			if (Check == -1) break;

			ModelPart = new tModelPart;

			// Read in the name of the model part
			ifModelStream.read((char*)ModelPart->m_szName, 256);

			// Read in the number of Indices
			ifModelStream.read((char*)&ModelPart->m_nVertexCount, sizeof(ModelPart->m_nVertexCount));

			// Allocate memory for the vertex list and read in each vertex
			ModelPart->m_pVertexList = new tModelVertex[ModelPart->m_nVertexCount];
			for (unsigned int i = 0; i < ModelPart->m_nVertexCount; i++)
			{
				ifModelStream.read((char*)&(ModelPart->m_pVertexList[i].vPosition.x), sizeof(ModelPart->m_pVertexList[i].vPosition.x));
				ifModelStream.read((char*)&(ModelPart->m_pVertexList[i].vPosition.y), sizeof(ModelPart->m_pVertexList[i].vPosition.y));
				ifModelStream.read((char*)&(ModelPart->m_pVertexList[i].vPosition.z), sizeof(ModelPart->m_pVertexList[i].vPosition.z));
				ModelPart->m_pVertexList[i].vNormal.x = 0.0f;
				ModelPart->m_pVertexList[i].vNormal.y = 0.0f;
				ModelPart->m_pVertexList[i].vNormal.z = 0.0f;
			}

			// Read in the number of Triangles
			ifModelStream.read((char*)&ModelPart->m_nTriangleCount, sizeof(ModelPart->m_nTriangleCount));

			// Allocate memory for the triangle list and read in each triangle
			ModelPart->m_pTriangleList = new tModelTriangle[ModelPart->m_nTriangleCount];
			for (unsigned int i = 0; i < ModelPart->m_nTriangleCount; i++)
			{
				ifModelStream.read((char*)&(ModelPart->m_pTriangleList[i].nVertex[0]), sizeof(ModelPart->m_pTriangleList[i].nVertex[0]));
				ifModelStream.read((char*)&(ModelPart->m_pTriangleList[i].nVertex[1]), sizeof(ModelPart->m_pTriangleList[i].nVertex[1]));
				ifModelStream.read((char*)&(ModelPart->m_pTriangleList[i].nVertex[2]), sizeof(ModelPart->m_pTriangleList[i].nVertex[2]));
			}

			m_vPartList.push_back(*ModelPart);
		}

		// Build draw lists for later use
		m_nDrawList_Model = glGenLists(1);
		glNewList(m_nDrawList_Model, GL_COMPILE);
			glBegin(GL_TRIANGLES);
				for (unsigned int i = 0; i < m_vPartList.size(); i++)
					for (unsigned int j = 0; j < m_vPartList[i].m_nTriangleCount; j++)
						for (int k = 0; k < 3; k++)
							glVertex3f(m_vPartList[i].m_pVertexList[m_vPartList[i].m_pTriangleList[j].nVertex[k]].vPosition.x, m_vPartList[i].m_pVertexList[m_vPartList[i].m_pTriangleList[j].nVertex[k]].vPosition.y, m_vPartList[i].m_pVertexList[m_vPartList[i].m_pTriangleList[j].nVertex[k]].vPosition.z);
			glEnd();
		glEndList();
	}

	~CSimpleModel()
	{
		for (unsigned int i = 0; i < m_vPartList.size(); i++)
		{
			delete [] m_vPartList[i].m_pVertexList;
			delete [] m_vPartList[i].m_pTriangleList;
		}
	}

	void Render(void)
	{
		glCallList(m_nDrawList_Model);
	}
};

#endif