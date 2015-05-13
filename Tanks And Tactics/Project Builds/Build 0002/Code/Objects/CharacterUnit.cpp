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
	// NOTE: Character file will be needed later
	std::fstream ifCharFile(szCharacterFile, std::ios_base::binary | std::ios_base::in);
	if (ifCharFile.bad())	return false;
	if (!ifCharFile.good())	return false;
	ifCharFile.close();

	// For now, manually set variables
	SetPosition(0.0f, 0.0f);
	m_fYRotation = 0.0f;

	m_Model = new C3D_Model;
	m_Model->LoadModel("./Assets/Tank.ttmodel");

	m_bMoving = false;
	m_vMovePosition.x = 0.0f;
	m_vMovePosition.y = 0.0f;
	m_vMovePosition.z = 0.0f;

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
		m_fYRotation = acos(abs(m_vHeading.z) / (sqrt(m_vHeading.x * m_vHeading.x + m_vHeading.z * m_vHeading.z))) * 180.0f / 3.14159f;
		if		((m_vHeading.x > 0.0f) && (m_vHeading.z < 0.0f)) m_fYRotation = 180.0f - m_fYRotation;
		else if ((m_vHeading.x < 0.0f) && (m_vHeading.z < 0.0f)) m_fYRotation += 180.0f;
		else if	((m_vHeading.x < 0.0f) && (m_vHeading.z > 0.0f)) m_fYRotation *= -1.0f;

		// Clamp to the map height if you are within the borders
		CMapController::GetInstance()->PositionHeight(m_vPosition);

		// If you've reached your destination, clamp to it and stop moving
		if ((m_vPosition - m_vMovePosition).magnitude() < 0.005f)
		{
			m_vPosition.x = m_vMovePosition.x;
			m_vPosition.z = m_vMovePosition.z;
			CMapController::GetInstance()->PositionHeight(m_vPosition);
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
	glPushMatrix();
		glTranslatef(m_vPosition.x, m_vPosition.y, m_vPosition.z);
		glRotatef(m_fYRotation, 0.0f, 1.0f, 0.0f);
		if (m_Model != NULL) m_Model->RenderModel();
	glPopMatrix();
}

bool CharacterUnit::RaySelect(COpenGLFrame& Cam, vec3f Ray)
{
	//! If the character has no model, immediately return false.
	//! Obtain the camera position, and return the RayCollision
	//! function's return from the model on the character.

	if (m_Model == 0) return false;

	vec3f CameraPos = Cam.GetOrigin();
	return m_Model->RayCollision(CameraPos - m_vPosition, CameraPos + Ray - m_vPosition);
}