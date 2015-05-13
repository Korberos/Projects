////////////////////////////////////////////////////////////////////////////////
//
//	Name:			InputController.cpp
//
//	Description:	This file is the control for the program input
//					and is utilized through accessors and modifiers
//					only while maintaining itself.
//
//	Created:		January 20th, 2009
//
//	Last Modified:	January 20th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#include "InputController.h"

CInputController* CInputController::GetInstance(void)
{
	static CInputController INSTANCE;
	return &INSTANCE;
}

void CInputController::InvalidateOldInput(void)
{
	//  Loop through the values in the keyboard array and
	//  change any 1 to a 2, indicating that the key has
	//  been held down and is not in the process of being 
	//  pushed down this frame.
	for (unsigned int i = 0; i < 256; ++i)
		if (m_cKeyboard[i] == 1)
			m_cKeyboard[i] = 2;

	//  Loop through any values in the mouse array and
	//  change any 1 to a 2, indicating that the button has
	//  been held down and is not in the process of being
	//  pushed down this frame
	for (unsigned int i = 0; i < 3; ++i)
		if (m_cMouseButton[i] == 1)
			m_cMouseButton[i] = 2;
}