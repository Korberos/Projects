#include "Input Control.h"

#include <windows.h>

InputControl* InputControl::GetInstance(void)
{
	static InputControl INSTANCE;
	return &INSTANCE;
}

void InputControl::Update(void)
{
	memset(BufferedKeyArray, 0, 32);
	memset(BufferedMouseButtons, false, 3);
}

void InputControl::Update_KeyDown(char Key)
{
	Key = min(max(Key, 0), 255);
	char Byte	= (Key >> 3) + 1;
	char Bit	= ((unsigned char)(Key << 5) >> 5);

	BufferedKeyArray[Byte]	|= ~KeyArray[Byte] & (1 << Bit);
	KeyArray[Byte]			|= (1 << Bit);
}

void InputControl::Update_KeyUp(char Key)
{
	Key = min(max(Key, 0), 255);
	char Byte	= (Key >> 3) + 1;
	char Bit	= ((unsigned char)(Key << 5) >> 5);

	BufferedKeyArray[Byte]	&= ~(1 << Bit);
	KeyArray[Byte]			&= ~(1 << Bit);
}

void InputControl::Update_MouseMove(long X, long Y)
{
	MouseAxis[0] = X;
	MouseAxis[1] = Y;
}

void InputControl::Update_MouseButton(char Button, bool Down)
{
	BufferedMouseButtons[Button] = (!MouseButtons[Button] && Down);
	MouseButtons[Button] = Down;
}

bool InputControl::GetKey(char Key)
{
	Key = min(max(Key, 0), 255);
	char Byte	= (Key >> 3) + 1;
	char Bit	= ((unsigned char)(Key << 5) >> 5);

	return ((KeyArray[Byte] & (1 << Bit)) != 0);
}

bool InputControl::GetBufferedKey(char Key)
{
	Key = min(max(Key, 0), 255);
	char Byte	= Key >> 3;
	char Bit	= ((unsigned char)(Key << 4) >> 4);
	if (Bit == 8) Byte += (Bit = 1);
	return ((BufferedKeyArray[Byte] & (1 << Bit)) != 0);
}

void InputControl::GetMousePosition(long &X, long &Y)
{
	X = MouseAxis[0];
	Y = MouseAxis[1];
}

bool InputControl::GetMouseButton(char Button)
{
	return MouseButtons[min(max(Button, 0), 2)];
}

bool InputControl::GetBufferedMouseButton(char Button)
{
	return BufferedMouseButtons[min(max(Button, 0), 2)];
}