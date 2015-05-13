////////////////////////////////////////////////////////////////////////////////
//
//	Name:			3D_Model.h
//
//	Description:	This file is a self-controlled model to be 
//					used by way of rendering within the game.
//
//	Created:		December 22nd, 2008
//
//	Last Modified:	December 22nd, 2008
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DB_3DMODEL_
#define _DB_3DMODEL_

#include <fstream>
#include "./../Utilities/vec3.h"
#include "./../Utilities/SDL_image/SDL_image.h"
#include "./../Utilities/SDL_Surface_to_OpenGL.h"
#include "./../Utilities/LineToTriangle.h"
#include "./../Controllers/TextureController.h"

class C3D_Model
{
private:
	struct tModel_Part
	{
	public:
		struct tModel_Vertex
		{
		public:
			float	m_vPosition[3];
			float	m_fTextureCoordinate[2];
		};

		struct tModel_Triangle
		{
		public:
			unsigned int	m_nVertexID[3];
		};

		unsigned int		m_nVertexCount;
		tModel_Vertex*		m_pVertexList;
		unsigned int		m_nTriangleCount;
		tModel_Triangle*	m_pTriangleList;
		int					m_nTextureID;
		unsigned int		m_nDrawListID;


		tModel_Part();
		~tModel_Part();
	};

	struct tModel_Animation
	{
	public:

		struct tModel_AnimationElement
		{
		public:
			unsigned int	m_nPartID;
			float			m_fAnimationTime;
			vec3f			m_vRotationPos;
			vec3f			m_vRotation;
			vec3f			m_vTranslation;
			vec3f			m_vScale;
		};

		unsigned int				m_nElementCount;
		tModel_AnimationElement*	m_pElementList;

		tModel_Animation();
		~tModel_Animation();
	};

	unsigned int		m_nPartCount;
	tModel_Part*		m_pPartList;
	unsigned int		m_nAnimationCount;
	tModel_Animation*	m_pAnimationList;
	tModel_Animation*	m_pCurrentAnimation;

public:
	C3D_Model();
	~C3D_Model();

	int		LoadModel(char* szModelFile);
	void	RenderModel(void);
	bool	RayCollision(vec3f Start, vec3f End);
};

#endif