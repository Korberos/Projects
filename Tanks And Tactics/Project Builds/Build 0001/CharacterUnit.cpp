////////////////////////////////////////////////////////////////////////////////
//
//	Name:			CharacterUnit.cpp
//
//	Description:	This file contains the class that forms the 
//					basis of all characters/units in the game. The 
//					class contains base functionality for the unit 
//					object and can carry over to any type of unit.
//
//	Created:		December 19th, 2008
//
//	Last Modified:	December 19th, 2008
//
////////////////////////////////////////////////////////////////////////////////
#include "CharacterUnit.h"

#include "MapController.h"

CharacterUnit::CharacterUnit()
{
	m_Model = NULL;
}

CharacterUnit::~CharacterUnit()
{
	if (m_Model != NULL) delete m_Model;
}

bool CharacterUnit::Initialize(char* szCharacterFile)
{
	std::fstream ifCharFile(szCharacterFile, std::ios_base::binary | std::ios_base::in);
	if (ifCharFile.bad())	return false;
	if (!ifCharFile.good())	return false;

	// For now, manually set variables
	m_vPosition.x = 0.0f;
	m_vPosition.y = 0.0f;
	m_vPosition.z = 0.0f;
	m_fYRotation = 0.0f;
	m_Model = new C3D_Model;
	m_Model->LoadModel("Output.ttmodel");

	m_bMoving = true;
	m_vMovePosition.x = 2.0f;
	m_vMovePosition.y = 1.0f;
	m_vMovePosition.z = -2.0f;

	ifCharFile.close();
	return true;
}

void CharacterUnit::Update(float fTime)
{
	// If you're moving, calculate necessary information
	if (m_bMoving == true)
	{
		// Move the X and Z based on the Heading
		vec3f m_vHeading = ((m_vMovePosition - m_vPosition).normalize());
		m_vPosition.x += m_vHeading.x * fTime / 400.0f;
		m_vPosition.z += m_vHeading.z * fTime / 400.0f;

		// Update the Y Rotation based on the Heading and its quadrant
		m_fYRotation = acos(abs(m_vHeading.x) / (sqrt(m_vHeading.x * m_vHeading.x + m_vHeading.z * m_vHeading.z))) * 180.0f / 3.14159f;
		if		((m_vHeading.x < 0.0f) && (m_vHeading.z > 0.0f)) m_fYRotation += 270.0f;
		else if ((m_vHeading.x < 0.0f) && (m_vHeading.z < 0.0f)) m_fYRotation += 180.0f;
		else if ((m_vHeading.x > 0.0f) && (m_vHeading.z < 0.0f)) m_fYRotation += 90.0f;

		// Check the MapController to see if the unit can be clamped to the ground in it's current position
		vec3f Intersection;
		if (CMapController::GetInstance()->MapIntersection(&Intersection, m_vPosition) == true) m_vPosition.y = Intersection.y;

		// If you've reached your destination, clamp to it and stop moving
		if (abs((m_vPosition - m_vMovePosition).magnitude()) < 0.1f)
		{
			m_vPosition.x = m_vMovePosition.x;
			m_vPosition.z = m_vMovePosition.z;
			m_bMoving = false;
		}
	}
}

void CharacterUnit::Render(void)
{
	glLineWidth(1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
		glTranslatef(m_vPosition.x, m_vPosition.y, m_vPosition.z);
		glRotatef(m_fYRotation, 0.0f, 1.0f, 0.0f);
		if (m_Model != NULL) m_Model->RenderModel();
	glPopMatrix();
}