////////////////////////////////////////////////////////////////////////////////
//
//	Name:			InputController.h
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
#ifndef _DB_INPUTCONTROLLER_
#define _DB_INPUTCONTROLLER_

class CInputController
{
	//~  Member Functions
	private:
		CInputController();
		~CInputController()		{}

	public:
		static CInputController*	GetInstance(void);

		void			InvalidateOldInput(void);

		//  Accessors
		inline	char			GetKey(unsigned int i)	const			{	return m_cKeyboard[i];		}
		inline	unsigned int	GetMouseX(void)			const			{	return m_nMousePosition[0];	}
		inline	unsigned int	GetMouseY(void)			const			{	return m_nMousePosition[1];	}
		inline	unsigned int	GetMouseButton(unsigned int i)	const	{	return m_cMouseButton[i];	}

		//  Modifiers
		inline	void			SetKey(unsigned int i, char v)			{	m_cKeyboard[i] = v;			}
		inline	void			SetMouseX(unsigned int x)				{	m_nMousePosition[0] = x;	}
		inline	void			SetMouseY(unsigned int y)				{	m_nMousePosition[1] = y;	}
		inline	void			SetMouseButton(unsigned int i, char v)	{	m_cMouseButton[i] = v;		}

	//~ Member Variables
	private:
		unsigned int	m_nMousePosition[2];
		char			m_cMouseButton[3];
		char			m_cKeyboard[323];

	public:
};

#endif