////////////////////////////////////////////////////////////////////////////////
//
//	Name:			InputSystem.h
//
//	Description:	This file is the control for the program input
//					and is utilized through accessors and modifiers
//					only while maintaining itself.
//
//	Created:		January 20th, 2009
//
//	Last Modified:	April 18th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUT_SYSTEM_
#define _INPUT_SYSTEM_

#include <string>

class InputSystem
{
	//~  Member Functions
	private:
		InputSystem();
		~InputSystem();

	public:
		static InputSystem* Get_Instance( void );

		void			Invalidate_Old_Input( void );
		void			Add_Key_To_String( int k );

		//  Accessors
		inline	char			Get_Key( unsigned int i )			const	{	return Keyboard[i];			}
		inline	unsigned int	Get_Mouse_X( void )					const	{	return MousePosition[0];	}
		inline	unsigned int	Get_Mouse_Y( void )					const	{	return MousePosition[1];	}
		inline	unsigned int	Get_Mouse_Button( unsigned int i )	const	{	return MouseButton[i];		}
		inline	std::string		Get_Text_String( void )				const	{	return TextString;			}

		//  Modifiers
		inline	void			Set_Key( unsigned int i , char c )			{	Keyboard[i] = c;			}
		inline	void			Set_Mouse_X( unsigned int x )				{	MousePosition[0] = x;		}
		inline	void			Set_Mouse_Y( unsigned int y )				{	MousePosition[1] = y;		}
		inline	void			Set_Mouse_Button( unsigned int i , char c )	{	MouseButton[i] = c;			}
		inline	void			Set_Text_String( std::string s )			{	TextString = s;				}


	//~ Member Variables
	private:
		unsigned int	MousePosition[2];
		char			MouseButton[3];
		char			Keyboard[323];
		std::string		TextString;

	public:
};

#endif