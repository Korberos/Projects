#include "MainProgram.h"

MainProgram* MainProgram::GetInstance(void)
{
	static MainProgram INSTANCE;
	return &INSTANCE;
}

void MainProgram::Initialize(HWND Handle, HINSTANCE Instance, int W_Width, int W_Height, bool* KeyArray)
{
	// Hold onto any variables needed to be taken in
	Window_Handle	= Handle;
	App_Instance	= Instance;
	Window_Width	= W_Width;
	Window_Height	= W_Height;
	Key_Array		= KeyArray;

	// Mouse Position
	m_nMouseX		= W_Width / 2;
	m_nMouseY		= W_Height / 2;

	// Initialize OpenGL
	InitializeOpenGL();

	// Initialize VBOs
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");

	// Load the height field
	m_hHeight.m_nLOD = 8;
	m_hHeight.Create("./Assets/heightField.raw", 1024, 1024);
}

MainProgram::MainProgram(void)
{
	// Performance counter
	m_bResetTimeTrigger = false;
	__int64 n64CntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&n64CntsPerSec);
	m_nSecsPerCnt = 1.0f / (float) n64CntsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_n64PrevTimeStamp);
	m_fTimeSlice = 0.0f;
}

MainProgram::~MainProgram(void)
{
}

bool MainProgram::MainProcess(void)
{
	// Calculate the time since the previous frame
	__int64 n64CurrTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&n64CurrTimeStamp );
	m_fTimeSlice = min((n64CurrTimeStamp - m_n64PrevTimeStamp) * m_nSecsPerCnt, 0.1f);
	m_n64PrevTimeStamp = n64CurrTimeStamp;

	// Take Input
	if (Key_Array[87])	// W
		m_Camera.MoveForward(0.01f);

	if (Key_Array[65])	// A
		m_Camera.MoveRight(0.01f);

	if (Key_Array[83])	// S
		m_Camera.MoveForward(-0.01f);

	if (Key_Array[68])	// D
		m_Camera.MoveRight(-0.01f);

	if (Key_Array[38])	// Up
		m_Camera.RotateLocalX(-0.01f);

	if (Key_Array[40])	// Down
		m_Camera.RotateLocalX(0.01f);

	if (Key_Array[37])	// Left
		m_Camera.RotateWorld(0.1f, 0.00f, 0.01f, 0.0f);

	if (Key_Array[39])	// Right
		m_Camera.RotateWorld(-0.1f, 0.0f, 0.01f, 0.0f);

	if (Key_Array[81])	// Q
		m_Camera.MoveUp(0.01f);

	if (Key_Array[69])	// E
		m_Camera.MoveUp(-0.01f);

	// 3D Rendering Context
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)Window_Width / (GLfloat)Window_Width, 0.1f, 100.0f);
	m_Camera.ApplyCameraTransform();

	// All 3D rendering
	// NOTE: This is just a test for rendering
	glPushMatrix();
	{
		glDisable(GL_LIGHTING);
		
		GLfloat FloorHeight		= -0.1f;
		GLfloat GridLength		= 10.0f;
		GLfloat GridSeparation	= 0.25f;

		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 0.7f, 0.7f);
		glLineWidth(2);

		glBegin(GL_LINES);
		for (GLfloat X = -GridLength; X <= GridLength; X += GridSeparation)
		{
			glVertex3f(X, FloorHeight, GridLength);
			glVertex3f(X, FloorHeight, -GridLength);
			
			glVertex3f(GridLength, FloorHeight, X);
			glVertex3f(-GridLength, FloorHeight, X);
		}
		glEnd();

		glLineWidth(1);
		glEnable(GL_TEXTURE_2D);
	}
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	m_hHeight.Render();

	// 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Window_Width, Window_Width, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// All 2D rendering
	glPushMatrix();
	{
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(100.0f, 0.0f);
			glVertex2f(100.0f, 100.0f);
			glVertex2f(0.0f, 100.0f);
		glEnd();
	}
	glPopMatrix();

	if(m_bResetTimeTrigger)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_n64PrevTimeStamp);
		m_bResetTimeTrigger = false;
	}

	return true;
}