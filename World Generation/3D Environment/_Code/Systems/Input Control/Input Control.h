#pragma once

class InputControl
{
private:
	InputControl()		{}
	~InputControl()		{}

	char		KeyArray[32];
	char		BufferedKeyArray[32];
;	long		MouseAxis[2];
	bool		MouseButtons[3];
	bool		BufferedMouseButtons[3];

public:
	static InputControl* GetInstance(void);
		
	void Update(void);
	void Update_KeyDown(char Key);
	void Update_KeyUp(char Key);
	void Update_MouseMove(long X, long Y);
	void Update_MouseButton(char Button, bool Down);

	bool GetKey(char Key);
	bool GetBufferedKey(char Key);
	void GetMousePosition(long &X, long &Y);
	bool GetMouseButton(char Button);
	bool GetBufferedMouseButton(char Button);

	inline long GetMouseX(void)	{ return MouseAxis[0]; }
	inline long GetMouseY(void)	{ return MouseAxis[1]; }
};