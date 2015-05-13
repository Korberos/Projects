////////////////////////////////////////////////////////////////////////////////
//
//	Name:			3D_Model.cpp
//
//	Description:	This file is a self-controlled model to be 
//					used by way of rendering within the game.
//
//	Created:		December 22nd, 2008
//
//	Last Modified:	December 22nd, 2008
//
////////////////////////////////////////////////////////////////////////////////
#include "3D_Model.h"

C3D_Model::tModel_Animation::tModel_Animation()
{
	m_nElementCount = 0;
	m_pElementList = NULL;
}

C3D_Model::tModel_Animation::~tModel_Animation()
{
	m_nElementCount = 0;
	if (m_pElementList != NULL) delete [] m_pElementList;
}

C3D_Model::tModel_Part::tModel_Part()
{
	m_nVertexCount = 0;
	m_pVertexList = NULL;
	m_nTriangleCount = 0;
	m_pTriangleList = NULL;
	m_nTextureID = 0;
	m_nDrawListID = 0;
}

C3D_Model::tModel_Part::~tModel_Part()
{
	m_nVertexCount = 0;
	if (m_pVertexList != NULL) delete [] m_pVertexList;
	m_nTriangleCount = 0;
	if (m_pTriangleList != NULL) delete [] m_pTriangleList;
	m_nTextureID = 0;
	m_nDrawListID = 0;
}

C3D_Model::C3D_Model()
{
	m_nPartCount = 0;
	m_pPartList = NULL;
	m_nAnimationCount = 0;
	m_pAnimationList = NULL;
	m_pCurrentAnimation = NULL;
}

C3D_Model::~C3D_Model()
{
	m_nPartCount = 0;
	if (m_pPartList != NULL) delete [] m_pPartList;
	m_nAnimationCount = 0;
	if (m_pAnimationList != NULL) delete [] m_pAnimationList;
	m_pCurrentAnimation = NULL;
}

int C3D_Model::LoadModel(char* szModelFile)
{
	if (szModelFile == 0) return -1;

	// Open the model file
	std::fstream ifModelStream(szModelFile, std::ios_base::binary | std::ios_base::in);
	if (ifModelStream.bad())	return -2;
	if (!ifModelStream.good())	return -2;

	// Read in the amount of parts the model has, and allocate the memory needed
	ifModelStream.read((char*)&m_nPartCount, sizeof(m_nPartCount));
	if (m_nPartCount < 0)	return -3;
	if (m_nPartCount > 100)	return -3;
	m_pPartList = new tModel_Part[m_nPartCount];

	// For each part, read in the part information
	for (unsigned int i = 0; i < m_nPartCount; i++)
	{
		// Read in the amount of vertices the part has, and allocate the memory
		ifModelStream.read((char*)&m_pPartList[i].m_nVertexCount, sizeof(m_pPartList[i].m_nVertexCount));
		if (m_pPartList[i].m_nVertexCount < 0)		return -4;
		if (m_pPartList[i].m_nVertexCount > 100000)	return -4;
		m_pPartList[i].m_pVertexList = new tModel_Part::tModel_Vertex[m_pPartList[i].m_nVertexCount];

		// Read in the vertex list
		for (unsigned int j = 0; j < m_pPartList[i].m_nVertexCount; j++)
		{
			ifModelStream.read((char*)m_pPartList[i].m_pVertexList[j].m_vPosition, sizeof(m_pPartList[i].m_pVertexList[j].m_vPosition[0]) * 3);
			ifModelStream.read((char*)m_pPartList[i].m_pVertexList[j].m_fTextureCoordinate, sizeof(m_pPartList[i].m_pVertexList[j].m_fTextureCoordinate[0]) * 2);
		}

		// Read in the amount of triangles the part has, and allocate the memory
		ifModelStream.read((char*)&m_pPartList[i].m_nTriangleCount, sizeof(m_pPartList[i].m_nTriangleCount));
		if (m_pPartList[i].m_nTriangleCount < 0)		return -5;
		if (m_pPartList[i].m_nTriangleCount > 100000)	return -5;
		m_pPartList[i].m_pTriangleList = new tModel_Part::tModel_Triangle[m_pPartList[i].m_nTriangleCount];

		// Read in the triangle list
		for (unsigned int j = 0; j < m_pPartList[i].m_nTriangleCount; j++)
		{
			ifModelStream.read((char*)&m_pPartList[i].m_pTriangleList[j].m_nVertexID, sizeof(m_pPartList[i].m_pTriangleList[j].m_nVertexID[0]) * 3);
		}

		// Read in the texture, then convert it to an OpenGL texture
		char TextureName[32];
		ifModelStream.read((char*)TextureName, sizeof(char) * 32);
		if (TextureName[0] != 0)	m_pPartList[i].m_nTextureID = CTextureController::GetInstance()->LoadTexture(TextureName);
		else						m_pPartList[i].m_nTextureID = -1;
	}

	// Read in the amount of animations the model has, and allocate the memory needed
	ifModelStream.read((char*)&m_nAnimationCount, sizeof(m_nAnimationCount));
	if (m_nAnimationCount < 0)		return -6;
	if (m_nAnimationCount > 100)	return -6;
	m_pAnimationList = new tModel_Animation[m_nAnimationCount];

	// For each animation, read in the animation information
	for (unsigned int i = 0; i < m_nAnimationCount; i++)
	{
		// Read in the amount of elements the animation has, and allocate the memory
		ifModelStream.read((char*)&m_pAnimationList[i].m_nElementCount, sizeof(m_pAnimationList[i].m_nElementCount));
		if (m_pAnimationList[i].m_nElementCount < 0)	return -7;
		if (m_pAnimationList[i].m_nElementCount > 100)	return -7;
		m_pAnimationList[i].m_pElementList = new tModel_Animation::tModel_AnimationElement[m_pAnimationList[i].m_nElementCount];

		// Read in the animation element list
		ifModelStream.read((char*)&m_pAnimationList[i].m_pElementList, sizeof(tModel_Animation::tModel_AnimationElement) * m_pAnimationList[i].m_nElementCount);
	}

	return 0;
}

void C3D_Model::RenderModel(void)
{
	// Loop through each part in the model
	for (unsigned int i = 0; i < m_nPartCount; i++)
	{
		// If the model has a texture, prepare to draw it. Otherwise draw a wireframe
		if (m_pPartList[i].m_nTextureID != -1)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_pPartList[i].m_nTextureID);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		// Draw the triangle list with the texture centered correctly
		glBegin(GL_TRIANGLES);
			for (unsigned int j = 0; j < m_pPartList[i].m_nTriangleCount; j++)
				for (unsigned int k = 0; k < 3; k++)
				{
					glTexCoord2f(m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[k]].m_fTextureCoordinate[0], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[k]].m_fTextureCoordinate[1]);
					glVertex3f(m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[k]].m_vPosition[0], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[k]].m_vPosition[1], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[k]].m_vPosition[2]);
				}
		glEnd();
	}
}

bool C3D_Model::RayCollision(vec3f Start, vec3f End)
{
	//! Loop through each part in the model, and each triangle in the
	//! part. Perform a LineToTriangle test on the triangle and if any
	//! hit, return true. Otherwise, return false

	for (unsigned int i = 0; i < m_nPartCount; ++i)
	{
		for (unsigned int j = 0; j < m_pPartList[i].m_nTriangleCount; ++j)
		{
			tL2T T;
			T.Position[0] = vec3f(m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[0]].m_vPosition[0], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[0]].m_vPosition[1], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[0]].m_vPosition[2]);
			T.Position[1] = vec3f(m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[1]].m_vPosition[0], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[1]].m_vPosition[1], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[1]].m_vPosition[2]);
			T.Position[2] = vec3f(m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[2]].m_vPosition[0], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[2]].m_vPosition[1], m_pPartList[i].m_pVertexList[m_pPartList[i].m_pTriangleList[j].m_nVertexID[2]].m_vPosition[2]);
			T.CalculateNormal();

			vec3f Intersection;
			if (LineToTriangle(Intersection, T, Start, End)) return true;
		}
	}

	return false;
}