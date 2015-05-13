#pragma once

#include "Helpers/Stopwatch.h"
#include "MapController.h"

class MainProgram
{
private:
	GLFrame			Camera;
	
	CStopWatch		Timer;
	float			ElapsedTime;

	MapController*	MAP;
	GLuint			MapImage;

	MainProgram(void)	{}
	~MainProgram(void)	{}

public:
	static MainProgram*	GetInstance(void)
	{
		static MainProgram Instance;
		return &Instance;
	}

	bool			Initialize(HWND WindowHandle, HINSTANCE AppInstance, int WindowWidth, int WindowHeight);
	bool			MainProcess(void);
	bool			Shutdown(void);

	
private:
	// Initialize Sub-Functions
	void Init_OpenGL(void);

	// MainProcess Sub-Functions
	bool Update(void);
	bool Input(void);
	bool Render(void);
	/*-*/void Draw_FloorGrid(void);
};
