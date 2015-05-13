#include "InputSystem.h"

InputSystem::InputSystem()
{
	for (unsigned int i = 0; i < 323; ++i)
		Keyboard[i] = 0;
	
	for (unsigned int i = 0; i < 3; ++i)
		MouseButton[i] = 0;
}

InputSystem::~InputSystem()
{
}

InputSystem* InputSystem::Get_Instance( void )
{
	static InputSystem INSTANCE;
	return &INSTANCE;
}

void InputSystem::Invalidate_Old_Input( void )
{
	//  Loop through the values in the keyboard array and
	//  change any 1 to a 2, indicating that the key has
	//  been held down and is not in the process of being 
	//  pushed down this frame.
	for (unsigned int i = 0; i < 256; ++i)
		if (Keyboard[i] == 1)
			Keyboard[i] = 2;

	//  Loop through any values in the mouse array and
	//  change any 1 to a 2, indicating that the button has
	//  been held down and is not in the process of being
	//  pushed down this frame
	for (unsigned int i = 0; i < 3; ++i)
		if (MouseButton[i] == 1)
			MouseButton[i] = 2;

	//  Clear the Text String for the next group of keys to
	//  be taken in by the message system
	TextString = "";
}

void InputSystem::Add_Key_To_String( int k )
{
	if (k < 32) return;
	if ((k > 126) && ((k < 256) || (k > 265))) return;

	//  If the symbol is a letter and shift is held down, capitalize it
	if ((k >= 97) && (k <= 122) && (Get_Key(303) || Get_Key(304)))
		k -= 32;

	if ((Get_Key(303) || Get_Key(304)))
	{
		switch (k)
		{
		case 39:
			k = 34;
			break;

		case 44:
			k = 60;
			break;

		case 45:
			k = 95;
			break;

		case 46:
			k = 62;
			break;

		case 47:
			k = 63;
			break;

		case 48:
			k = 41;
			break;

		case 49:
			k = 33;
			break;

		case 50:
			k = 64;
			break;

		case 51:
			k = 35;
			break;

		case 52:
			k = 36;
			break;

		case 53:
			k = 37;
			break;

		case 54:
			k = 94;
			break;

		case 55:
			k = 38;
			break;

		case 56:
			k = 42;
			break;

		case 57:
			k = 40;
			break;

		case 59:
			k = 58;
			break;

		case 61:
			k = 43;
			break;

		case 91:
			k = 123;
			break;

		case 92:
			k = 124;
			break;

		case 93:
			k = 125;
			break;

		case 96:
			k = 126;
			break;
		}
	}

	switch (k)
	{
	case 256:
	case 257:
	case 258:
	case 259:
	case 260:
	case 261:
	case 262:
	case 263:
	case 264:
	case 265:
		k -= 208;
		break;
	}

	TextString += (unsigned char)k;
}
